/*
 * 
 * Author information summary for YARP:
 *
 * Copyright (C) 2001-2004 Greg Kroah-Hartman (greg@kroah.com)
 * CopyPolicy: Released under the terms of the GNU GPL version 2
 *
 */
/*
 * USB H8 based motor controller driver 
 *
 * based on USB Skeleton driver.
 */
/*
 * USB Skeleton driver - 2.0
 *
 * Copyright (C) 2001-2004 Greg Kroah-Hartman (greg@kroah.com)
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License as
 *	published by the Free Software Foundation, version 2.
 *
 * This driver is based on the 2.6.3 version of drivers/usb/usb-skeleton.c 
 * but has been rewritten to be easy to read and use, as no locks are now
 * needed anymore.
 *
 */

#define DEBUG

#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kref.h>
#include <asm/uaccess.h>
#include <linux/usb.h>

#include "urbtc.h"
#include "urobotc.h"

static struct usb_device_id urbtc_table [] = {
	{ USB_DEVICE(0x8da, 0xfc00) },	/* base version */
	{ USB_DEVICE(0xff8, 0x001) },	/* iXs Reserch version */
	{ }					/* Terminating entry */
};
MODULE_DEVICE_TABLE (usb, urbtc_table);

#define USB_URBTC_MINOR_BASE	100

/* our private defines. if this grows any larger, use your own .h file */
#define MAX_TRANSFER		(PAGE_SIZE - 512)
#define WRITES_IN_FLIGHT	8

#define EP_READREQ	1
#define EP_SCMD		2
#define EP_READ		5
#define EP_CCMD		6

/* Structure to hold all of our device specific stuff */
struct usb_urbtc {
	struct usb_device *udev;
	struct usb_interface *interface;
	struct semaphore limit_sem;

	int read_status;
	unsigned char *readreq_buffer;
	size_t readreq_size;

	int readbuf_enabled;
	wait_queue_head_t readbuf_wait;
	struct urb *readbuf_urb;
	struct uin *readbuf_work;
	struct semaphore readbuf_sem;
	struct uin *readbuf_buffered;
	size_t readbuf_size;
	unsigned short readbuf_last_buffered; /* same to uin.time */
	unsigned short readbuf_last_read; /* same to uin.time */

	int write_status;
	unsigned char *write_counter_buffer;
	size_t write_counter_size;

	struct kref		kref;
};
#define to_urbtc_dev(d) container_of(d, struct usb_urbtc, kref)

static struct usb_driver urbtc_driver;

static void urbtc_delete(struct kref *kref)
{	
	struct usb_urbtc *dev = to_urbtc_dev(kref);

	usb_put_dev(dev->udev);
	kfree (dev->readreq_buffer);
	usb_free_urb(dev->readbuf_urb);
	kfree (dev->readbuf_work);
	kfree (dev->readbuf_buffered);
	kfree (dev);
}

static int urbtc_open(struct inode *inode, struct file *file)
{
	struct usb_urbtc *dev;
	struct usb_interface *interface;
	int subminor;
	int retval = 0;

	subminor = iminor(inode);

	interface = usb_find_interface(&urbtc_driver, subminor);
	if (!interface) {
		err ("%s - error, can't find device for minor %d",
		     __FUNCTION__, subminor);
		retval = -ENODEV;
		goto exit;
	}

	dev = usb_get_intfdata(interface);
	if (!dev) {
		retval = -ENODEV;
		goto exit;
	}

	dev->read_status = URBTC_STATUS_READ_REQUEST;
	dev->readbuf_enabled = 0;
	dev->write_status = URBTC_STATUS_WRITE_DESIRE;

	/* increment our usage count for the device */
	kref_get(&dev->kref);

	/* save our object in the file's private structure */
	file->private_data = dev;

exit:
	return retval;
}

static int urbtc_release(struct inode *inode, struct file *file)
{
	struct usb_urbtc *dev = (struct usb_urbtc *)file->private_data;

	if (dev == NULL)
		return -ENODEV;

	if (dev->interface) {
		if (dev->readbuf_enabled)
			usb_kill_urb(dev->readbuf_urb);
	}
		
	/* decrement the count on our device */
	kref_put(&dev->kref, urbtc_delete);
	return 0;
}

static void urbtc_read_bulk_callback(struct urb *urb, struct pt_regs *regs)
{
	struct usb_urbtc *dev = (struct usb_urbtc *)urb->context;
	int res;

	/* sync/async unlink faults aren't errors */
	if (urb->status && 
	    !(urb->status == -ENOENT || 
	      urb->status == -ECONNRESET ||
	      urb->status == -ESHUTDOWN)) {
		dbg("%s - nonzero read bulk status received: %d",
		    __FUNCTION__, urb->status);
		dev->readbuf_enabled = 0;
		return;
	}
	if (urb->actual_length > 0) {
		if (down_trylock(&dev->readbuf_sem) == 0) {
			memcpy(dev->readbuf_buffered, dev->readbuf_work, urb->actual_length);
			dev->readbuf_last_buffered = dev->readbuf_work->time;
			up(&dev->readbuf_sem);
			wake_up(&dev->readbuf_wait);
		}
	}
	if (dev->readbuf_enabled) {
		usb_fill_bulk_urb(dev->readbuf_urb, dev->udev,
				  usb_rcvbulkpipe(dev->udev, EP_READ),
				  dev->readbuf_urb->transfer_buffer,
				  dev->readbuf_urb->transfer_buffer_length,
				  urbtc_read_bulk_callback, dev);
		if ((res = usb_submit_urb(dev->readbuf_urb, GFP_ATOMIC))) {
			dev->readbuf_enabled = 0;
		}
	}
}

static ssize_t urbtc_read(struct file *file, char *buffer, size_t count, loff_t *ppos)
{
	struct usb_urbtc *dev = (struct usb_urbtc *)file->private_data;
	int retval = 0;
	int bytes_read;
	
	if (count != sizeof(struct uin))
		return -EINVAL;

	switch (dev->read_status) {
	case URBTC_STATUS_READ_REQUEST:
		/* do a blocking bulk read to get data from the device */
		retval = usb_bulk_msg(dev->udev,
				      usb_rcvbulkpipe(dev->udev, EP_READREQ),
				      dev->readreq_buffer,
				      min(dev->readreq_size, count),
				      &bytes_read, 10000);

		/* if the read was successful, copy the data to userspace */
		if (!retval) {
			if (copy_to_user(buffer, dev->readreq_buffer, bytes_read))
				retval = -EFAULT;
			else
				retval = bytes_read;
		}
		break;
	case URBTC_STATUS_READ_CONTINUOUS:
		if (dev->readbuf_enabled) {
			wait_event_interruptible(dev->readbuf_wait,
						 dev->readbuf_last_read != dev->readbuf_last_buffered || !dev->udev);
			if (signal_pending(current)) {
				retval = -EINTR;
				goto out;
			}
			if (!dev->udev) {
				retval = -ENODEV;
				goto out;
			}
			dev->readbuf_last_read = dev->readbuf_last_buffered;
			down(&dev->readbuf_sem);
			if (copy_to_user(buffer, dev->readbuf_buffered, dev->readbuf_size))
				retval = -EFAULT;
			else
				retval = dev->readbuf_size;
			up(&dev->readbuf_sem);
		} else {
			/* do a blocking bulk read to get data from the device */
			down(&dev->readbuf_sem);
			retval = usb_bulk_msg(dev->udev,
					      usb_rcvbulkpipe(dev->udev, EP_READ),
					      dev->readbuf_buffered,
					      min(dev->readbuf_size, count),
					      &bytes_read, 10000);

			/* if the read was successful, copy the data to userspace */
			if (!retval) {
				if (copy_to_user(buffer, dev->readbuf_buffered, bytes_read))
					retval = -EFAULT;
				else
					retval = bytes_read;
			}
			up(&dev->readbuf_sem);
		}
		break;
	default:
		/* should not occur */
		retval = -EINVAL;
		break;
	}
out:
	return retval;
}

static void urbtc_write_bulk_callback(struct urb *urb, struct pt_regs *regs)
{
	struct usb_urbtc *dev = (struct usb_urbtc *)urb->context;

	/* sync/async unlink faults aren't errors */
	if (urb->status && 
	    !(urb->status == -ENOENT || 
	      urb->status == -ECONNRESET ||
	      urb->status == -ESHUTDOWN)) {
		dbg("%s - nonzero write bulk status received: %d",
		    __FUNCTION__, urb->status);
	}

	/* free up our allocated buffer */
	usb_buffer_free(urb->dev, urb->transfer_buffer_length, 
			urb->transfer_buffer, urb->transfer_dma);
	up(&dev->limit_sem);
}

static ssize_t urbtc_write_sync(struct file *file, const char *user_buffer, size_t count, loff_t *ppos)
{
	struct usb_urbtc *dev = (struct usb_urbtc *)file->private_data;
	int retval = 0;
	size_t writesize = min(count, dev->write_counter_size);
	int bytes_written;

	if (copy_from_user(dev->write_counter_buffer, user_buffer, writesize)) {
		retval = -EFAULT;
		goto error;
	}

	/* do a blocking bulk write to the device */
	retval = usb_bulk_msg(dev->udev,
			      usb_sndbulkpipe(dev->udev, EP_CCMD),
			      dev->write_counter_buffer,
			      writesize,
			      &bytes_written, 10000);

	if (!retval)
		retval = bytes_written;

error:
	return retval;
}

static ssize_t urbtc_write_async(struct file *file, const char *user_buffer, size_t count, loff_t *ppos)
{
	struct usb_urbtc *dev = (struct usb_urbtc *)file->private_data;
	int retval = 0;
	struct urb *urb = NULL;
	char *buf = NULL;
	size_t writesize = min(count, (size_t)MAX_TRANSFER);

	/* limit the number of URBs in flight to stop a user from using up all RAM */
	if (down_interruptible(&dev->limit_sem)) {
		retval = -ERESTARTSYS;
		goto exit;
	}

	/* create a urb, and a buffer for it, and copy the data to the urb */
	urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!urb) {
		retval = -ENOMEM;
		goto error;
	}

	buf = usb_buffer_alloc(dev->udev, writesize, GFP_KERNEL, &urb->transfer_dma);
	if (!buf) {
		retval = -ENOMEM;
		goto error;
	}

	if (copy_from_user(buf, user_buffer, writesize)) {
		retval = -EFAULT;
		goto error;
	}

	/* initialize the urb properly */
	usb_fill_bulk_urb(urb, dev->udev,
			  usb_sndbulkpipe(dev->udev, EP_SCMD),
			  buf, writesize, urbtc_write_bulk_callback, dev);
	urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

	/* send the data out the bulk port */
	retval = usb_submit_urb(urb, GFP_KERNEL);
	if (retval) {
		err("%s - failed submitting write urb, error %d", __FUNCTION__, retval);
		goto error;
	}

	/* release our reference to this urb, the USB core will eventually free it entirely */
	usb_free_urb(urb);

exit:
	return writesize;

error:
	usb_buffer_free(dev->udev, writesize, buf, urb->transfer_dma);
	usb_free_urb(urb);
	up(&dev->limit_sem);
	return retval;
}

static ssize_t urbtc_write(struct file *file, const char *user_buffer, size_t count, loff_t *ppos)
{
	struct usb_urbtc *dev = (struct usb_urbtc *)file->private_data;

	/* verify that we actually have some data to write */
	if (count == 0)
		return 0;

	switch (dev->write_status) {
	case URBTC_STATUS_WRITE_COUNTER:
		return urbtc_write_sync(file, user_buffer, count, ppos);
	case URBTC_STATUS_WRITE_DESIRE:
		return urbtc_write_async(file, user_buffer, count, ppos);
	default:
		/* should not occur */
		return -EINVAL;
	}
}

static int
urbtc_ioctl(struct inode *inode, struct file *file,
	    unsigned int cmd, unsigned long arg)
{
	struct usb_urbtc *dev = (struct usb_urbtc *)file->private_data;
	struct usb_device *udev = dev->udev;
	int retval = 0;

	switch(cmd) {
	case URBTC_GET_VENDOR:
		if(copy_to_user((int*)arg, &udev->descriptor.idVendor, sizeof(int)))
			return -EFAULT;
		break;
	case URBTC_GET_PRODUCT:
		if(copy_to_user((int*)arg, &udev->descriptor.idProduct, sizeof(int)))
			return -EFAULT;
		break;
	case URBTC_REQUEST_READ:
		dev->read_status = URBTC_STATUS_READ_REQUEST;
		break;
	case URBTC_CONTINUOUS_READ:
		dev->read_status = URBTC_STATUS_READ_CONTINUOUS;
		break;
	case URBTC_BUFREAD:
		if (dev->read_status != URBTC_STATUS_READ_CONTINUOUS)
			return -EINVAL;
		if (!dev->readbuf_enabled) {
			usb_fill_bulk_urb(dev->readbuf_urb, dev->udev,
					  usb_rcvbulkpipe(dev->udev, EP_READ),
					  dev->readbuf_urb->transfer_buffer,
					  dev->readbuf_urb->transfer_buffer_length,
					  urbtc_read_bulk_callback, dev);
			if (!(retval = usb_submit_urb(dev->readbuf_urb, GFP_KERNEL)))
				dev->readbuf_enabled = 1;
		}
		break;
	case URBTC_WAITREAD:
		dev->readbuf_enabled = 0;
		break;
	case URBTC_GET_READ_STATUS:
		if (copy_to_user((int*)arg, &dev->read_status, sizeof(int)))
			return -EFAULT;
		break;

	case URBTC_COUNTER_SET:
		dev->write_status = URBTC_STATUS_WRITE_COUNTER;
		break;
	case URBTC_DESIRE_SET:
		dev->write_status = URBTC_STATUS_WRITE_DESIRE;
		break;
	case URBTC_GET_WRITE_STATUS:
		if (copy_to_user((int*)arg, &dev->write_status, sizeof(int)))
			return -EFAULT;
		break;
	default:
		return -ENOIOCTLCMD;
	}

	return retval;
}

static struct file_operations urbtc_fops = {
	.owner =	THIS_MODULE,
	.read =		urbtc_read,
	.write =	urbtc_write,
	.ioctl = 	urbtc_ioctl,
	.open =		urbtc_open,
	.release =	urbtc_release,
};

/* 
 * usb class driver info in order to get a minor number from the usb core,
 * and to have the device registered with the driver core
 */
static struct usb_class_driver urbtc_class = {
	.name =		"urbtc%d",
	.fops =		&urbtc_fops,
	.minor_base =	USB_URBTC_MINOR_BASE,
};

static int urbtc_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
	struct usb_urbtc *dev = NULL;
	struct usb_host_interface *iface_desc;
	struct usb_endpoint_descriptor *endpoint;
	size_t buffer_size;
	int i;
	int retval = -ENOMEM;

	/* allocate memory for our device state and initialize it */
	dev = kmalloc(sizeof(*dev), GFP_KERNEL);
	if (dev == NULL) {
		err("Out of memory");
		goto error;
	}
	memset(dev, 0, sizeof(*dev));
	kref_init(&dev->kref);
	sema_init(&dev->limit_sem, WRITES_IN_FLIGHT);
	init_MUTEX(&dev->readbuf_sem);
	init_waitqueue_head(&dev->readbuf_wait);

	dev->udev = usb_get_dev(interface_to_usbdev(interface));
	dev->interface = interface;

	dev->readreq_buffer = NULL;
	dev->readbuf_urb = NULL;
	dev->readbuf_work = NULL;
	dev->readbuf_buffered = NULL;
	dev->readbuf_last_read = 0;
	dev->readbuf_last_buffered = 0;

	iface_desc = interface->cur_altsetting;
	for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i) {
		endpoint = &iface_desc->endpoint[i].desc;

		dbg("endpoint %d(%d) %s",
		    endpoint->bEndpointAddress,
		    endpoint->bEndpointAddress & ~USB_ENDPOINT_DIR_MASK,
		    endpoint->bEndpointAddress & USB_ENDPOINT_DIR_MASK? "in": "out");
		if (((endpoint->bEndpointAddress & USB_ENDPOINT_DIR_MASK)
		     == USB_DIR_IN) &&
		    ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK)
		     == USB_ENDPOINT_XFER_BULK)) {
			switch (endpoint->bEndpointAddress & ~USB_ENDPOINT_DIR_MASK) {
			case EP_READREQ:
				buffer_size = le16_to_cpu(endpoint->wMaxPacketSize);
				dev->readreq_size = buffer_size;
				dev->readreq_buffer = kmalloc(buffer_size, GFP_KERNEL);
				if (!dev->readreq_buffer) {
					err("Could not allocate readreq_buffer");
					goto error;
				}
				break;
			case EP_READ:
				buffer_size = sizeof(struct uin);
				dev->readbuf_urb = usb_alloc_urb(0, GFP_KERNEL);
				if (!dev->readbuf_urb) {
					err("Could not allocate readbuf_urb");
					goto error;
				}
				dev->readbuf_size = buffer_size;
				dev->readbuf_work = kmalloc(buffer_size, GFP_KERNEL);
				if (!dev->readbuf_work) {
					err("Could not allocate readbuf_work");
					goto error;
				}
				dev->readbuf_buffered = kmalloc(buffer_size, GFP_KERNEL);
				if (!dev->readbuf_buffered) {
					err("Could not allocate readbuf_buffer");
					goto error;
				}
				usb_fill_bulk_urb(dev->readbuf_urb, dev->udev,
						  usb_rcvbulkpipe(dev->udev, endpoint->bEndpointAddress),
						  dev->readbuf_work, dev->readbuf_size,
						  urbtc_read_bulk_callback, dev);
				break;
			default:
				break;
			}
		}

		if (((endpoint->bEndpointAddress & USB_ENDPOINT_DIR_MASK)
		     == USB_DIR_OUT) &&
		    ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK)
		     == USB_ENDPOINT_XFER_BULK)) {
			switch (endpoint->bEndpointAddress & ~USB_ENDPOINT_DIR_MASK) {
			case EP_CCMD:
				buffer_size = le16_to_cpu(endpoint->wMaxPacketSize);
				dev->write_counter_size = buffer_size;
				dev->write_counter_buffer = kmalloc(buffer_size, GFP_KERNEL);
				if (!dev->write_counter_buffer) {
					err("Could not allocate write_counter_buffer");
					goto error;
				}
				break;
			case EP_SCMD:
			default:
				break;
			}
		}
	}

	/* save our data pointer in this interface device */
	usb_set_intfdata(interface, dev);

	/* we can register the device now, as it is ready */
	retval = usb_register_dev(interface, &urbtc_class);
	if (retval) {
		/* something prevented us from registering this driver */
		err("Not able to get a minor for this device.");
		usb_set_intfdata(interface, NULL);
		goto error;
	}

	/* let the user know what node this device is now attached to */
	info("USB Robot device now attached to urbtc - %d", interface->minor);
	return 0;

error:
	if (dev) {
		if (dev->readreq_buffer)
			kfree(dev->readreq_buffer);
		if (dev->readbuf_urb)
			usb_free_urb(dev->readbuf_urb);
		if (dev->readbuf_work)
			kfree(dev->readbuf_work);
		if (dev->readbuf_buffered)
			kfree(dev->readbuf_buffered);
		kref_put(&dev->kref, urbtc_delete);
	}
	return retval;
}

static void urbtc_disconnect(struct usb_interface *interface)
{
	struct usb_urbtc *dev;
	int minor = interface->minor;

	/* prevent urbtc_open() from racing urbtc_disconnect() */
	lock_kernel();

	dev = usb_get_intfdata(interface);
	usb_set_intfdata(interface, NULL);

	/* give back our minor */
	usb_deregister_dev(interface, &urbtc_class);

	unlock_kernel();

	/* decrement our usage count */
	kref_put(&dev->kref, urbtc_delete);
	wake_up(&dev->readbuf_wait);

	info("USB Robot #%d now disconnected", minor);
}

static struct usb_driver urbtc_driver = {
	.name =		"urbtc",
	.probe =	urbtc_probe,
	.disconnect =	urbtc_disconnect,
	.id_table =	urbtc_table,
};

static int __init usb_urbtc_init(void)
{
	int result;

	/* register this driver with the USB subsystem */
	result = usb_register(&urbtc_driver);
	if (result)
		err("usb_register failed. Error number %d", result);

	return result;
}

static void __exit usb_urbtc_exit(void)
{
	/* deregister this driver with the USB subsystem */
	usb_deregister(&urbtc_driver);
}

module_init (usb_urbtc_init);
module_exit (usb_urbtc_exit);

MODULE_LICENSE("GPL");
/*
  Local Variables:
  c-file-style: "linux"
  End:
*/

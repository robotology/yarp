/*
 * Copyright (C) 2010 Philipp Robbel
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 * Implements basic Kinect driver.
 *
 * NOTE: In its current form, the camera needs a few seconds of startup time
 * before it works. If launched too early, garbled images may result.
 */

#include "KinectDeviceDriver.h"
using namespace yarp::dev;

//! Global pointer for callback functions
KinectDeviceDriver *kinect;

//! Depth callback for libfreenect's C interface
static void depthimg(uint16_t *buf, int width, int height) {
  kinect->depthImgCb(buf);
}

//! Rgb callback for libfreenect's C interface
static void rgbimg(uint8_t *buf, int width, int height) {
  kinect->rgbImgCb(buf);
}

bool KinectDeviceDriver::open(const KinectDeviceDriverSettings & cfg) {
  libusb_init(NULL);
  libusb_device_handle *dev = libusb_open_device_with_vid_pid(NULL, 0x45e, 0x2ae);
  if (!dev) {
    printf("Could not open device\n");
    return false;
  }

  kinect = this; // set global pointer
  //cams_init(dev, depthimg, rgbimg);

  #error "not sure how cams_init is implemented now"

  // start usb grabbing thread
  usbSpinThread_.start();
  return true;
}

bool KinectDeviceDriver::close() {
  usbSpinThread_.stop();
  kinect = NULL;
  return true;
}

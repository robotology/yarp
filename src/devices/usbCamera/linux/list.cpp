/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */


#include "list.h"
#include "USBcameraLogComponent.h"

#include <yarp/os/LogStream.h>

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <linux/videodev2.h>
#include <sys/ioctl.h>


void enum_image_fmt_v4l2(int fd)
{
    struct v4l2_fmtdesc fmtd;

    yCInfo(USBCAMERA, "============================================");
    yCInfo(USBCAMERA, "Querying image format");
    yCInfo(USBCAMERA);

    memset(&fmtd, 0, sizeof(struct v4l2_fmtdesc));
    fmtd.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmtd.index = 0;

    while (ioctl(fd, VIDIOC_ENUM_FMT, &fmtd) >= 0) {
        yCInfo(USBCAMERA,
               "%d - %s (compressed : %d) (%#x)",
               fmtd.index,
               fmtd.description,
               fmtd.flags,
               fmtd.pixelformat);
        fmtd.index++;
    }

    yCInfo(USBCAMERA);
}

void query_current_image_fmt_v4l2(int fd)
{
    struct v4l2_format fmt;
    struct v4l2_fmtdesc fmtd; //to find a text description of the image format

    memset(&fmt, 0, sizeof(struct v4l2_format));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    memset(&fmtd, 0, sizeof(struct v4l2_fmtdesc));
    fmtd.index = 0;
    fmtd.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    yCInfo(USBCAMERA, "============================================");
    yCInfo(USBCAMERA, "Querying current image format settings");
    yCInfo(USBCAMERA);

    if (-1 == ioctl(fd, VIDIOC_G_FMT, &fmt)) {
        yCError(USBCAMERA, "Failed to get image format: %d, %s", errno, strerror(errno));
    } else {
        yCInfo(USBCAMERA, "Current width: %d", fmt.fmt.pix.width);
        yCInfo(USBCAMERA, "Current height: %d", fmt.fmt.pix.height);
        yCInfo(USBCAMERA, "Current bytes per line: %d", fmt.fmt.pix.bytesperline);
        yCInfo(USBCAMERA, "Current image size: %d", fmt.fmt.pix.sizeimage);
        yCInfo(USBCAMERA, "Current color space: %d", fmt.fmt.pix.colorspace);
        yCInfo(USBCAMERA, "Current pixel format: ");
        while (ioctl(fd, VIDIOC_ENUM_FMT, &fmtd) >= 0) {
            if (fmt.fmt.pix.pixelformat == fmtd.pixelformat) {
                yCInfo(USBCAMERA, "%s", fmtd.description);
                break;
            }
            fmtd.index++;
        }
    }

    yCInfo(USBCAMERA);
}

void query_capture_intf_v4l2(int fd)
{
    struct v4l2_input vin;
    struct v4l2_tuner tun;
    struct v4l2_frequency freq;

    memset(&vin, 0, sizeof(struct v4l2_input));
    vin.index = 0;

    yCInfo(USBCAMERA, "============================================");
    yCInfo(USBCAMERA, "Querying capture capabilities");
    yCInfo(USBCAMERA);

    while (ioctl(fd, VIDIOC_ENUMINPUT, &vin) >= 0) {
        yCInfo(USBCAMERA, "Input number: %d", vin.index);
        yCInfo(USBCAMERA, "Name: %s", vin.name);
        yCInfo(USBCAMERA, "Type: (%d) ", vin.type);
        if (vin.type & V4L2_INPUT_TYPE_TUNER) {
            yCInfo(USBCAMERA, "Tuner");
            yCInfo(USBCAMERA, "Tuner index: %d", vin.tuner);
            memset(&tun, 0, sizeof(struct v4l2_tuner));
            tun.index = vin.tuner;
            if (ioctl(fd, VIDIOC_G_TUNER, &tun) == 0) {
                yCInfo(USBCAMERA, "Name: %s", tun.name);
                if (tun.type == V4L2_TUNER_RADIO) {
                    yCInfo(USBCAMERA, "It is a RADIO tuner");
                }
                if (tun.type == V4L2_TUNER_ANALOG_TV) {
                    yCInfo(USBCAMERA, "It is a TV tuner");
                }
                if (tun.capability & V4L2_TUNER_CAP_LOW) {
                    yCInfo(USBCAMERA, "Frequencies in units of 62.5Hz");
                } else {
                    yCInfo(USBCAMERA, "Frequencies in units of 62.5kHz");
                }

                if (tun.capability & V4L2_TUNER_CAP_NORM) {
                    yCInfo(USBCAMERA, "Multi-standard tuner");
                }
                if (tun.capability & V4L2_TUNER_CAP_STEREO) {
                    yCInfo(USBCAMERA, "Stereo reception supported");
                }
                /* More flags here */
                yCInfo(USBCAMERA,
                       "lowest tunable frequency: %.2f %s",
                       tun.rangelow * 62.5,
                       (tun.capability & V4L2_TUNER_CAP_LOW) ? "Hz" : "kHz");
                yCInfo(USBCAMERA,
                       "highest tunable frequency: %.2f %s",
                       tun.rangehigh * 62.5,
                       (tun.capability & V4L2_TUNER_CAP_LOW) ? "Hz" : "kHz");
                memset(&freq, 0, sizeof(struct v4l2_frequency));
                freq.tuner = vin.tuner;
                if (ioctl(fd, VIDIOC_G_FREQUENCY, &freq) == 0) {
                    yCInfo(USBCAMERA,
                           "Current frequency: %.2f %s",
                           freq.frequency * 62.5,
                           (tun.capability & V4L2_TUNER_CAP_LOW) ? "Hz" : "kHz");
                }
            }
        }
        if (vin.type & V4L2_INPUT_TYPE_CAMERA) {
            yCInfo(USBCAMERA, "Camera");
        }
        yCInfo(USBCAMERA, "Supported standards: (%d) ", (int)vin.std);
        if (vin.std & V4L2_STD_PAL) {
            yCInfo(USBCAMERA, "PAL ");
        }
        if (vin.std & V4L2_STD_NTSC) {
            yCInfo(USBCAMERA, "NTSC ");
        }
        if (vin.std & V4L2_STD_SECAM) {
            yCInfo(USBCAMERA, "SECAM ");
        }
        yCInfo(USBCAMERA);
        vin.index++;
    }
}

void query_frame_sizes_v4l2(int fd)
{
    struct v4l2_frmsizeenum frms;
    struct v4l2_fmtdesc fmtd;

    memset(&frms, 0, sizeof(struct v4l2_frmsizeenum));
    memset(&fmtd, 0, sizeof(struct v4l2_fmtdesc));
    fmtd.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmtd.index = 0;


    yCInfo(USBCAMERA, "============================================");
    yCInfo(USBCAMERA, "Querying supported frame sizes");
    yCInfo(USBCAMERA);

    while (ioctl(fd, VIDIOC_ENUM_FMT, &fmtd) >= 0) {
        yCInfo(USBCAMERA, "Image format: %s", fmtd.description);
        frms.index = 0;
        frms.pixel_format = fmtd.pixelformat;
        while (ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frms) >= 0) {
            if (frms.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
                yCInfo(USBCAMERA,
                       "index %2d:  Width: %4d - Height: %d",
                       frms.index,
                       frms.discrete.width,
                       frms.discrete.height);
                frms.index++;
            } else {
                yCInfo(USBCAMERA,
                       "index %2d\tMin, max & step height: %d - %d - %d Min, max & step width: %d - %d - %d",
                       frms.index,
                       frms.stepwise.min_height,
                       frms.stepwise.max_height,
                       frms.stepwise.step_height,
                       frms.stepwise.min_width,
                       frms.stepwise.max_width,
                       frms.stepwise.step_width);
                break;
            }
        }
        fmtd.index++;
    }
}

void print_v4l2_control(struct v4l2_queryctrl* qc)
{
    yCInfo(USBCAMERA,
           "Control: id: 0x%x - name: %s - min: %d -max: %d - step: %d - type: %d(%s) - flags: %d (%s%s%s%s%s%s)",
           qc->id,
           (char*)&qc->name,
           qc->minimum,
           qc->maximum,
           qc->step,
           qc->type,
           (qc->type == V4L2_CTRL_TYPE_INTEGER ? "Integer" :
            qc->type == V4L2_CTRL_TYPE_BOOLEAN ? "Boolean" :
            qc->type == V4L2_CTRL_TYPE_MENU ? "Menu" :
            qc->type == V4L2_CTRL_TYPE_BUTTON ? "Button" :
            qc->type == V4L2_CTRL_TYPE_INTEGER64 ? "Integer64" :
            qc->type == V4L2_CTRL_TYPE_CTRL_CLASS ? "Class" :
            ""),
           qc->flags,
           qc->flags & V4L2_CTRL_FLAG_DISABLED ? "Disabled " : "",
           qc->flags & V4L2_CTRL_FLAG_GRABBED ? "Grabbed " : "",
           qc->flags & V4L2_CTRL_FLAG_READ_ONLY ? "ReadOnly " : "",
           qc->flags & V4L2_CTRL_FLAG_UPDATE ? "Update " : "",
           qc->flags & V4L2_CTRL_FLAG_INACTIVE ? "Inactive " : "",
           qc->flags & V4L2_CTRL_FLAG_SLIDER ? "slider " : "");
}

// void query_controls_v4l2(int fd)
// {
//     int i;
//     struct v4l2_queryctrl qctrl;
//     CLEAR(qctrl);
//     struct v4lconvert_data *d = v4lconvert_create(fd);
//
//     yCInfo(USBCAMERA, "============================================");
//     yCInfo(USBCAMERA, "Querying standard controls");
//     yCInfo(USBCAMERA);
//
//     // std ctrls
//     for( i = V4L2_CID_BASE; i< V4L2_CID_LASTP1; i++) {
//         qctrl.id = i;
//         //if((ioctl(fd, VIDIOC_QUERYCTRL, &qctrl) == 0))
//         if(v4lconvert_vidioc_queryctrl(d,&qctrl)==0) {
//             print_v4l2_control(&qctrl);
//         }
//     }
//
//     yCInfo(USBCAMERA, "============================================");
//     yCInfo(USBCAMERA, "Querying private controls");
//     yCInfo(USBCAMERA);
//
//     // priv ctrls
//     for (qctrl.id = V4L2_CID_PRIVATE_BASE;; qctrl.id++) {
//         if(v4lconvert_vidioc_queryctrl(d,&qctrl)==0) {
//             print_v4l2_control(&qctrl);
//         } else {
//             if (errno == EINVAL)
//                 break;
//             yCError(USBCAMERA, "we shouldnt be here...");
//         }
//     }
//
//     yCInfo(USBCAMERA, "============================================");
//     yCInfo(USBCAMERA, "Querying extended controls");
//     yCInfo(USBCAMERA);
//
//       //checking extended controls
//     qctrl.id = V4L2_CTRL_FLAG_NEXT_CTRL;
//     while (v4lconvert_vidioc_queryctrl(d,&qctrl)==0) {
//             print_v4l2_control(&qctrl);
//             qctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
//     }
//     v4lconvert_destroy(d);
// }


void list_cap_v4l2(int fd)
{
    struct v4l2_capability cap;

    yCInfo(USBCAMERA, "============================================");
    yCInfo(USBCAMERA, "Querying general capabilities");
    yCInfo(USBCAMERA);

    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) {
        yCError(USBCAMERA, "v4l2 not supported. Maybe a v4l1 device ...");
    } else {
        //print capabilities
        yCInfo(USBCAMERA, "Driver name: %s", cap.driver);
        yCInfo(USBCAMERA, "Device name: %s", cap.card);
        yCInfo(USBCAMERA, "bus_info: %s", cap.bus_info);
        yCInfo(USBCAMERA,
               "version: %u.%u.%u",
               (cap.version >> 16) & 0xFF,
               (cap.version >> 8) & 0xFF,
               cap.version & 0xFF);

        yCInfo(USBCAMERA, "%s capture capability",            (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)      ? "Has" : "Does NOT have");
        yCInfo(USBCAMERA, "%s output capability",             (cap.capabilities & V4L2_CAP_VIDEO_OUTPUT)       ? "Has" : "Does NOT have");
        yCInfo(USBCAMERA, "%s overlay capability",            (cap.capabilities & V4L2_CAP_VIDEO_OVERLAY)      ? "Has" : "Does NOT have");
        yCInfo(USBCAMERA, "%s VBI capture capability",        (cap.capabilities & V4L2_CAP_VBI_CAPTURE)        ? "Has" : "Does NOT have");
        yCInfo(USBCAMERA, "%s VBI output capability",         (cap.capabilities & V4L2_CAP_VBI_OUTPUT)         ? "Has" : "Does NOT have");
        yCInfo(USBCAMERA, "%s SLICED VBI capture capability", (cap.capabilities & V4L2_CAP_SLICED_VBI_CAPTURE) ? "Has" : "Does NOT have");
        yCInfo(USBCAMERA, "%s SLICED VBI output capability",  (cap.capabilities & V4L2_CAP_SLICED_VBI_OUTPUT)  ? "Has" : "Does NOT have");
        yCInfo(USBCAMERA, "%s RDS capability",                (cap.capabilities & V4L2_CAP_RDS_CAPTURE)        ? "Has" : "Does NOT have");
        yCInfo(USBCAMERA, "%s tuner capability",              (cap.capabilities & V4L2_CAP_TUNER)              ? "Has" : "Does NOT have");
        yCInfo(USBCAMERA, "%s audio capability",              (cap.capabilities & V4L2_CAP_AUDIO)              ? "Has" : "Does NOT have");
        yCInfo(USBCAMERA, "%s radio capability",              (cap.capabilities & V4L2_CAP_RADIO)              ? "Has" : "Does NOT have");
        yCInfo(USBCAMERA, "%s read/write capability",         (cap.capabilities & V4L2_CAP_READWRITE)          ? "Has" : "Does NOT have");
        yCInfo(USBCAMERA, "%s async IO capability",           (cap.capabilities & V4L2_CAP_ASYNCIO)            ? "Has" : "Does NOT have");
        yCInfo(USBCAMERA, "%s streaming capability",          (cap.capabilities & V4L2_CAP_STREAMING)          ? "Has" : "Does NOT have");
        yCInfo(USBCAMERA);

        if (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) {
            query_capture_intf_v4l2(fd);
        }
        // FIXME Enumerate other capabilites (output, overlay,...

        enum_image_fmt_v4l2(fd);
        query_current_image_fmt_v4l2(fd);
        query_frame_sizes_v4l2(fd);
        //                query_controls_v4l2(fd);
    }
}

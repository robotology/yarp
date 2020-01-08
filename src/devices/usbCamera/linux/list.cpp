/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include "list.h"

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <linux/videodev2.h>
#include <sys/ioctl.h>


void enum_image_fmt_v4l2(int fd)
{
    struct v4l2_fmtdesc fmtd;

    printf("============================================\n"
           "Querying image format\n\n");
    memset(&fmtd, 0, sizeof(struct v4l2_fmtdesc));
    fmtd.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmtd.index = 0;

    while (ioctl(fd, VIDIOC_ENUM_FMT, &fmtd) >= 0) {
        printf("%d - %s (compressed : %d) (%#x) \n",
               fmtd.index,
               fmtd.description,
               fmtd.flags,
               fmtd.pixelformat);
        fmtd.index++;
    }

    printf("\n");
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

    printf("============================================\n"
           "Querying current image format settings\n\n");
    if (-1 == ioctl(fd, VIDIOC_G_FMT, &fmt)) {
        perror("Failed to get image format.");
    } else {
        printf("Current width: %d\n", fmt.fmt.pix.width);
        printf("Current height: %d\n", fmt.fmt.pix.height);
        printf("Current bytes per line: %d\n", fmt.fmt.pix.bytesperline);
        printf("Current image size: %d\n", fmt.fmt.pix.sizeimage);
        printf("Current color space: %d\n", fmt.fmt.pix.colorspace);
        printf("Current pixel format: ");
        while (ioctl(fd, VIDIOC_ENUM_FMT, &fmtd) >= 0) {
            if (fmt.fmt.pix.pixelformat == fmtd.pixelformat) {
                printf("%s\n", fmtd.description);
                break;
            }
            fmtd.index++;
        }
    }
    printf("\n");
}

void query_capture_intf_v4l2(int fd)
{
    struct v4l2_input vin;
    struct v4l2_tuner tun;
    struct v4l2_frequency freq;

    memset(&vin, 0, sizeof(struct v4l2_input));
    vin.index = 0;

    printf("============================================\n"
           "Querying capture capabilities\n");
    while (ioctl(fd, VIDIOC_ENUMINPUT, &vin) >= 0) {
        printf("Input number: %d\n", vin.index);
        printf("Name: %s\n", vin.name);
        printf("Type: (%d) ", vin.type);
        if (vin.type & V4L2_INPUT_TYPE_TUNER) {
            printf("Tuner\n");
            printf("Tuner index: %d\n", vin.tuner);
            memset(&tun, 0, sizeof(struct v4l2_tuner));
            tun.index = vin.tuner;
            if (ioctl(fd, VIDIOC_G_TUNER, &tun) == 0) {
                printf("Name: %s\n", tun.name);
                if (tun.type == V4L2_TUNER_RADIO) {
                    printf("It is a RADIO tuner\n");
                }
                if (tun.type == V4L2_TUNER_ANALOG_TV) {
                    printf("It is a TV tuner\n");
                }
                if (tun.capability & V4L2_TUNER_CAP_LOW) {
                    printf("Frequencies in units of 62.5Hz\n");
                } else {
                    printf("Frequencies in units of 62.5kHz\n");
                }

                if (tun.capability & V4L2_TUNER_CAP_NORM) {
                    printf("Multi-standard tuner\n");
                }
                if (tun.capability & V4L2_TUNER_CAP_STEREO) {
                    printf("Stereo reception supported\n");
                }
                /* More flags here */
                printf("lowest tunable frequency: %.2f %s\n",
                       tun.rangelow * 62.5,
                       (tun.capability & V4L2_TUNER_CAP_LOW) ? "Hz" : "kHz");
                printf("highest tunable frequency: %.2f %s\n",
                       tun.rangehigh * 62.5,
                       (tun.capability & V4L2_TUNER_CAP_LOW) ? "Hz" : "kHz");
                memset(&freq, 0, sizeof(struct v4l2_frequency));
                freq.tuner = vin.tuner;
                if (ioctl(fd, VIDIOC_G_FREQUENCY, &freq) == 0) {
                    printf("Current frequency: %.2f %s\n",
                           freq.frequency * 62.5,
                           (tun.capability & V4L2_TUNER_CAP_LOW) ? "Hz" : "kHz");
                }
            }
        }
        if (vin.type & V4L2_INPUT_TYPE_CAMERA) {
            printf("Camera\n");
        }
        printf("Supported standards: (%d) ", (int)vin.std);
        if (vin.std & V4L2_STD_PAL) {
            printf("PAL ");
        }
        if (vin.std & V4L2_STD_NTSC) {
            printf("NTSC ");
        }
        if (vin.std & V4L2_STD_SECAM) {
            printf("SECAM ");
        }
        printf("\n");
        vin.index++;
    }
    printf("\n");
}

void query_frame_sizes_v4l2(int fd)
{
    struct v4l2_frmsizeenum frms;
    struct v4l2_fmtdesc fmtd;

    memset(&frms, 0, sizeof(struct v4l2_frmsizeenum));
    memset(&fmtd, 0, sizeof(struct v4l2_fmtdesc));
    fmtd.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmtd.index = 0;


    printf("============================================\n"
           "Querying supported frame sizes\n\n");
    while (ioctl(fd, VIDIOC_ENUM_FMT, &fmtd) >= 0) {
        printf("Image format: %s\n", fmtd.description);
        frms.index = 0;
        frms.pixel_format = fmtd.pixelformat;
        while (ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frms) >= 0) {
            printf("index %2d", frms.index);
            if (frms.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
                printf(":  Width: %4d - Height: %d\n",
                       frms.discrete.width,
                       frms.discrete.height);
                frms.index++;
            } else {
                printf("\tMin, max & step height: %d - %d - %d Min, max & step"
                       "width: %d - %d - %d",
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
    printf("Control: id: 0x%x - name: %s - min: %d -max: %d - step: %d - "
           "type: %d(%s) - flags: %d (%s%s%s%s%s%s)\n",
           qc->id,
           (char*)&qc->name,
           qc->minimum,
           qc->maximum,
           qc->step,
           qc->type,
           qc->type == V4L2_CTRL_TYPE_INTEGER ? "Integer" : qc->type == V4L2_CTRL_TYPE_BOOLEAN ? "Boolean" : qc->type == V4L2_CTRL_TYPE_MENU ? "Menu" : qc->type == V4L2_CTRL_TYPE_BUTTON ? "Button" : qc->type == V4L2_CTRL_TYPE_INTEGER64 ? "Integer64" : qc->type == V4L2_CTRL_TYPE_CTRL_CLASS ? "Class" : "",
           qc->flags,
           qc->flags & V4L2_CTRL_FLAG_DISABLED ? "Disabled " : "",
           qc->flags & V4L2_CTRL_FLAG_GRABBED ? "Grabbed " : "",
           qc->flags & V4L2_CTRL_FLAG_READ_ONLY ? "ReadOnly " : "",
           qc->flags & V4L2_CTRL_FLAG_UPDATE ? "Update " : "",
           qc->flags & V4L2_CTRL_FLAG_INACTIVE ? "Inactive " : "",
           qc->flags & V4L2_CTRL_FLAG_SLIDER ? "slider " : "");
}

//void query_controls_v4l2(int fd) {
//        int i;
//        struct v4l2_queryctrl qctrl;
//        CLEAR(qctrl);
//        struct v4lconvert_data *d = v4lconvert_create(fd);
//        printf("============================================\n"
//                        "Querying standard controls\n\n");
//        //std ctrls
//        for( i = V4L2_CID_BASE; i< V4L2_CID_LASTP1; i++) {
//                qctrl.id = i;
//                //if((ioctl(fd, VIDIOC_QUERYCTRL, &qctrl) == 0))
//                if(v4lconvert_vidioc_queryctrl(d,&qctrl)==0)
//                        print_v4l2_control(&qctrl);
//        }

//        printf("============================================\n"
//                        "Querying private controls\n\n");
//        //priv ctrls
//        for (qctrl.id = V4L2_CID_PRIVATE_BASE;; qctrl.id++) {
//                if(v4lconvert_vidioc_queryctrl(d,&qctrl)==0) {
//                        print_v4l2_control(&qctrl);
//                } else {
//                        if (errno == EINVAL)
//                                break;

//                        printf( "we shouldnt be here...\n");
//                }
//        }

//        printf("============================================\n"
//                        "Querying extended controls\n\n");
//        //checking extended controls
//        qctrl.id = V4L2_CTRL_FLAG_NEXT_CTRL;
//        while (v4lconvert_vidioc_queryctrl(d,&qctrl)==0) {
//                print_v4l2_control(&qctrl);
//                qctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
//        }
//        v4lconvert_destroy(d);
//}


void list_cap_v4l2(int fd)
{
    struct v4l2_capability cap;

    printf("============================================\n"
           "Querying general capabilities\n\n");
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) {
        printf("v4l2 not supported. Maybe a v4l1 device ...");
    } else {
        //print capabilities
        printf("Driver name: %s\n", cap.driver);
        printf("Device name: %s\n", cap.card);
        printf("bus_info: %s\n", cap.bus_info);
        printf("version: %u.%u.%u\n",
               (cap.version >> 16) & 0xFF,
               (cap.version >> 8) & 0xFF,
               cap.version & 0xFF);
        if (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) {
            printf("Has");
        } else {
            printf("Does NOT have");
        }
        printf(" capture capability\n");
        if (cap.capabilities & V4L2_CAP_VIDEO_OUTPUT) {
            printf("Has");
        } else {
            printf("Does NOT have");
        }
        printf(" output capability\n");
        if (cap.capabilities & V4L2_CAP_VIDEO_OVERLAY) {
            printf("Has");
        } else {
            printf("Does NOT have");
        }
        printf(" overlay capability\n");
        if (cap.capabilities & V4L2_CAP_VBI_CAPTURE) {
            printf("Has");
        } else {
            printf("Does NOT have");
        }
        printf(" VBI capture capability\n");
        if (cap.capabilities & V4L2_CAP_VBI_OUTPUT) {
            printf("Has");
        } else {
            printf("Does NOT have");
        }
        printf(" VBI output capability\n");
        if (cap.capabilities & V4L2_CAP_SLICED_VBI_CAPTURE) {
            printf("Has");
        } else {
            printf("Does NOT have");
        }
        printf(" SLICED VBI capture capability\n");
        if (cap.capabilities & V4L2_CAP_SLICED_VBI_OUTPUT) {
            printf("Has");
        } else {
            printf("Does NOT have");
        }
        printf(" SLICED VBI output capability\n");
        if (cap.capabilities & V4L2_CAP_RDS_CAPTURE) {
            printf("Has");
        } else {
            printf("Does NOT have");
        }
        printf(" RDS capability\n");
        if (cap.capabilities & V4L2_CAP_TUNER) {
            printf("Has");
        } else {
            printf("Does NOT have");
        }
        printf(" tuner capability\n");
        if (cap.capabilities & V4L2_CAP_AUDIO) {
            printf("Has");
        } else {
            printf("Does NOT have");
        }
        printf(" audio capability\n");
        if (cap.capabilities & V4L2_CAP_RADIO) {
            printf("Has");
        } else {
            printf("Does NOT have");
        }
        printf(" radio capability\n");
        if (cap.capabilities & V4L2_CAP_READWRITE) {
            printf("Has");
        } else {
            printf("Does NOT have");
        }
        printf(" read/write capability\n");
        if (cap.capabilities & V4L2_CAP_ASYNCIO) {
            printf("Has");
        } else {
            printf("Does NOT have");
        }
        printf(" asyncIO capability\n");
        if (cap.capabilities & V4L2_CAP_STREAMING) {
            printf("Has");
        } else {
            printf("Does NOT have");
        }
        printf(" streaming capability\n");
        printf("\n");

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

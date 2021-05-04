/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */


#ifndef YARP_DEVICE_USBCAMERA_LINUX_LIST_H
#define YARP_DEVICE_USBCAMERA_LINUX_LIST_H

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>

void enum_image_fmt_v4l2(int fd);

void query_current_image_fmt_v4l2(int fd);

void query_capture_intf_v4l2(int fd);

void query_frame_sizes_v4l2(int fd);

void print_v4l2_control(struct v4l2_queryctrl* qc);

void list_cap_v4l2(int fd);

// void query_controls_v4l2(int fd);

#endif // YARP_DEVICE_USBCAMERA_LINUX_LIST_H

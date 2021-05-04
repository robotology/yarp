/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
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

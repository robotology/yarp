/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Alberto Cardellino
 * email:   alberto.cardellino@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
 */


#ifndef _ICUB_V4L_LIST_HPP_
#define _ICUB_V4L_LIST_HPP_

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>

void enum_image_fmt_v4l2(int fd);

void query_current_image_fmt_v4l2(int fd);

void query_capture_intf_v4l2(int fd);

void query_frame_sizes_v4l2(int fd);

void print_v4l2_control(struct v4l2_queryctrl *qc);

void list_cap_v4l2(int fd);

// void query_controls_v4l2(int fd);

#endif  // _ICUB_V4L_LIST_HPP_

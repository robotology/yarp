#ifndef _V4L_LIST_HPP_
#define _V4L_LIST_HPP_

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

#endif  // _V4L_LIST_HPP_
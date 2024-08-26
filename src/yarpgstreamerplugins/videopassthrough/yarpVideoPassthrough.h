/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef GST_YARP_VIDEO_PASSTHROUGH_H
#define GST_YARP_VIDEO_PASSTHROUGH_H

#include <gst/base/gstpushsrc.h>
#include <gst/gst.h>
#include <gst/video/video.h>

#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/sig/Image.h>

YARP_LOG_COMPONENT(YVP_COMP, "yarp.gstreamerplugin.yarpvideopassthrough")

#endif // GST_YARP_VIDEO_PASSTHROUGH_H

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/conf/system.h>

#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
#define YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
#   include <yarp/os/PortReport.h>
YARP_WARNING_POP
#undef YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE
#endif


yarp::os::PortReport::~PortReport() = default;

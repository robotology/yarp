/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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

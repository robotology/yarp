/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/PortInfo.h>


yarp::os::PortInfo::PortInfo() :
        tag(PORTINFO_NULL),
        incoming(false),
        created(true),
        message("no information")
{
}

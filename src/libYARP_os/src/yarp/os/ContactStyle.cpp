/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/ContactStyle.h>


yarp::os::ContactStyle::ContactStyle() :
        admin(false),
        quiet(false),
        verboseOnSuccess(true),
        timeout(-1),
        carrier(""),
        expectReply(true),
        persistent(false),
        persistenceType(yarp::os::ContactStyle::OPENENDED)
{
}

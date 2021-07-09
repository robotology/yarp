/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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

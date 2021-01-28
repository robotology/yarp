/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/companion/impl/Companion.h>

#include <yarp/os/Network.h>

using yarp::companion::impl::Companion;
using yarp::os::NetworkBase;

int Companion::cmdWait(int argc, char *argv[])
{
    if (argc == 1) {
        return NetworkBase::waitPort(argv[0]);
    }
    if (argc == 2) {
        return NetworkBase::waitConnection(argv[0], argv[1]);
    }
    yCError(COMPANION, "Please specify a single port name, or a source and destination port name");
    return 1;
}

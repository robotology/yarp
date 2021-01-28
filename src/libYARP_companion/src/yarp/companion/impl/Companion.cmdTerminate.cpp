/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/companion/impl/Companion.h>

#include <yarp/os/Terminator.h>

using yarp::companion::impl::Companion;
using yarp::os::Terminator;


int Companion::cmdTerminate(int argc, char *argv[])
{
    if (argc == 1) {
        yCInfo(COMPANION, "Asking port %s to quit gracefully", argv[0]);
        Terminator::terminateByName(argv[0]);
        return 0;
    }

    yCError(COMPANION, "Wrong parameter format, please specify a port name as a single parameter to terminate");
    return 1;
}

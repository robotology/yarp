/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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

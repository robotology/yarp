/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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

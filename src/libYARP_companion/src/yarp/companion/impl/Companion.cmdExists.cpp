/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/companion/impl/Companion.h>

#include <yarp/os/Network.h>

using yarp::companion::impl::Companion;
using yarp::os::NetworkBase;

int Companion::cmdExists(int argc, char *argv[])
{
    if (argc == 1) {
        bool ok = NetworkBase::exists(argv[0], true);
        return ok?0:1;
    }
    if (argc == 2) {
        bool ok = NetworkBase::isConnected(argv[0], argv[1], "", false);
        return ok?0:1;
    }

    yCError(COMPANION, "Please specify a port name");
    return 1;
}

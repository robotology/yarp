/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/companion/impl/Companion.h>

#include <yarp/conf/version.h>

using yarp::companion::impl::Companion;


std::string Companion::version()
{
    return YARP_VERSION;
}

int Companion::cmdVersion(int argc, char *argv[])
{
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);
    yCInfo(COMPANION, "YARP version %s", version().c_str());
    return 0;
}

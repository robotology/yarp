/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "yarp/dev/ControlBoardInterfacesImpl.h"
#include <yarp/os/Log.h>

#include <cstdio>
using namespace yarp::dev;

bool StubImplEncodersRaw::NOT_YET_IMPLEMENTED(const char *func)
{
    if (func) {
        yError("%s: not yet implemented\n", func);
    } else {
        yError("Function not yet implemented\n");
    }

    return false;
}

/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "ControlBoardWrapperPreciselyTimed.h"

#include "ControlBoardLogComponent.h"

#include <yarp/os/Stamp.h>


yarp::os::Stamp ControlBoardWrapperPreciselyTimed::getLastInputStamp()
{
    timeMutex.lock();
    yarp::os::Stamp ret = time;
    timeMutex.unlock();
    return ret;
}

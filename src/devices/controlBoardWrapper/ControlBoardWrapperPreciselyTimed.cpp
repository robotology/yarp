/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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

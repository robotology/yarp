/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERPRECISELYTIMED_H
#define YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERPRECISELYTIMED_H

#include <yarp/dev/IPreciselyTimed.h>

#include "ControlBoardWrapperCommon.h"


class ControlBoardWrapperPreciselyTimed :
        virtual public ControlBoardWrapperCommon,
        public yarp::dev::IPreciselyTimed
{
public:
    yarp::os::Stamp getLastInputStamp() override;
};

#endif // YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERPRECISELYTIMED_H

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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

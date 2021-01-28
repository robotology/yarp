/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERREMOTEVARIABLES_H
#define YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERREMOTEVARIABLES_H

#include <yarp/dev/IRemoteVariables.h>

#include "ControlBoardWrapperCommon.h"


class ControlBoardWrapperRemoteVariables :
        virtual public ControlBoardWrapperCommon,
        public yarp::dev::IRemoteVariables
{
public:
    bool getRemoteVariable(std::string key, yarp::os::Bottle& val) override;
    bool setRemoteVariable(std::string key, const yarp::os::Bottle& val) override;
    bool getRemoteVariablesList(yarp::os::Bottle* listOfKeys) override;
};

#endif // YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERREMOTEVARIABLES_H

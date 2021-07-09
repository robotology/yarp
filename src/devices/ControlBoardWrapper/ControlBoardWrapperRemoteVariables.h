/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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

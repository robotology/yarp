/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ControlBoardWrapperRemoteVariables.h"

#include "ControlBoardLogComponent.h"

using yarp::os::Bottle;

bool ControlBoardWrapperRemoteVariables::getRemoteVariable(std::string key, yarp::os::Bottle& val)
{
    bool b = true;

    for (size_t i = 0; i < device.subdevices.size(); i++) {
        SubDevice* p = device.getSubdevice(i);

        if (!p) {
            return false;
        }
        if (!p->iVar) {
            return false;
        }
        yarp::os::Bottle tmpval;
        b &= p->iVar->getRemoteVariable(key, tmpval);
        if (b) {
            val.append(tmpval);
        }
    }

    return b;
}

bool ControlBoardWrapperRemoteVariables::setRemoteVariable(std::string key, const yarp::os::Bottle& val)
{
    size_t bottle_size = val.size();
    size_t device_size = device.subdevices.size();
    if (bottle_size != device_size) {
        yCError(CONTROLBOARD, "setRemoteVariable bottle_size != device_size failure");
        return false;
    }

    bool b = true;
    for (size_t i = 0; i < device_size; i++) {
        SubDevice* p = device.getSubdevice(i);
        if (!p) {
            yCError(CONTROLBOARD, "setRemoteVariable !p failure");
            return false;
        }
        if (!p->iVar) {
            yCError(CONTROLBOARD, "setRemoteVariable !p->iVar failure");
            return false;
        }
        Bottle* partial_val = val.get(i).asList();
        if (partial_val) {
            b &= p->iVar->setRemoteVariable(key, *partial_val);
        } else {
            yCError(CONTROLBOARD, "setRemoteVariable general failure");
            return false;
        }
    }

    return b;
}

bool ControlBoardWrapperRemoteVariables::getRemoteVariablesList(yarp::os::Bottle* listOfKeys)
{
    //int off = device.lut[0].offset;
    size_t subIndex = device.lut[0].deviceEntry;
    SubDevice* p = device.getSubdevice(subIndex);

    if (!p) {
        return false;
    }

    if (p->iVar) {
        return p->iVar->getRemoteVariablesList(listOfKeys);
    }
    return false;
}

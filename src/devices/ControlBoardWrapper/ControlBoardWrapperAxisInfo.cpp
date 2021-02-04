/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "ControlBoardWrapperAxisInfo.h"

#include "ControlBoardLogComponent.h"


bool ControlBoardWrapperAxisInfo::getAxisName(int j, std::string& name)
{
    size_t off;
    try {
        off = device.lut.at(j).offset;
    } catch (...) {
        yCError(CONTROLBOARD, "Joint number %d out of bound [0-%zu] for part %s", j, controlledJoints, partName.c_str());
        return false;
    }
    size_t subIndex = device.lut[j].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (p->info) {
        return p->info->getAxisName(static_cast<int>(off + p->base), name);
    }
    return false;
}

bool ControlBoardWrapperAxisInfo::getJointType(int j, yarp::dev::JointTypeEnum& type)
{
    int off = device.lut[j].offset;
    size_t subIndex = device.lut[j].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (p->info) {
        return p->info->getJointType(static_cast<int>(off + p->base), type);
    }
    return false;
}

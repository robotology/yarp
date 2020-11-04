/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "ControlBoardWrapperAxisInfo.h"

#include "ControlBoardWrapperLogComponent.h"


bool ControlBoardWrapperAxisInfo::getAxisName(int j, std::string& name)
{
    int off;
    try {
        off = device.lut.at(j).offset;
    } catch (...) {
        yCError(CONTROLBOARDWRAPPER, "Joint number %d out of bound [0-%d] for part %s", j, controlledJoints, partName.c_str());
        return false;
    }
    int subIndex = device.lut[j].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (p->info) {
        return p->info->getAxisName(off + p->base, name);
    }
    return false;
}

bool ControlBoardWrapperAxisInfo::getJointType(int j, yarp::dev::JointTypeEnum& type)
{
    int off = device.lut[j].offset;
    int subIndex = device.lut[j].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (p->info) {
        return p->info->getJointType(off + p->base, type);
    }
    return false;
}

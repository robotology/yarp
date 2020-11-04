/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "ControlBoardWrapperControlLimits.h"

#include "ControlBoardWrapperLogComponent.h"


bool ControlBoardWrapperControlLimits::setLimits(int j, double min, double max)
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

    if (p->lim) {
        return p->lim->setLimits(off + p->base, min, max);
    }
    return false;
}

bool ControlBoardWrapperControlLimits::getLimits(int j, double* min, double* max)
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
        *min = 0.0;
        *max = 0.0;
        return false;
    }

    if (p->lim) {
        return p->lim->getLimits(off + p->base, min, max);
    }
    *min = 0.0;
    *max = 0.0;
    return false;
}

bool ControlBoardWrapperControlLimits::setVelLimits(int j, double min, double max)
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

    if (!p->lim) {
        return false;
    }
    return p->lim->setVelLimits(off + p->base, min, max);
}

bool ControlBoardWrapperControlLimits::getVelLimits(int j, double* min, double* max)
{
    int off;
    try {
        off = device.lut.at(j).offset;
    } catch (...) {
        yCError(CONTROLBOARDWRAPPER, "Joint number %d out of bound [0-%d] for part %s", j, controlledJoints, partName.c_str());
        return false;
    }
    int subIndex = device.lut[j].deviceEntry;

    *min = 0.0;
    *max = 0.0;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (!p->lim) {
        return false;
    }
    return p->lim->getVelLimits(off + p->base, min, max);
}

/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "ControlBoardWrapperControlLimits.h"

#include "ControlBoardLogComponent.h"


bool ControlBoardWrapperControlLimits::setLimits(int j, double min, double max)
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

    if (p->lim) {
        return p->lim->setLimits(static_cast<int>(off + p->base), min, max);
    }
    return false;
}

bool ControlBoardWrapperControlLimits::getLimits(int j, double* min, double* max)
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
        *min = 0.0;
        *max = 0.0;
        return false;
    }

    if (p->lim) {
        return p->lim->getLimits(static_cast<int>(off + p->base), min, max);
    }
    *min = 0.0;
    *max = 0.0;
    return false;
}

bool ControlBoardWrapperControlLimits::setVelLimits(int j, double min, double max)
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

    if (!p->lim) {
        return false;
    }
    return p->lim->setVelLimits(static_cast<int>(off + p->base), min, max);
}

bool ControlBoardWrapperControlLimits::getVelLimits(int j, double* min, double* max)
{
    size_t off;
    try {
        off = device.lut.at(j).offset;
    } catch (...) {
        yCError(CONTROLBOARD, "Joint number %d out of bound [0-%zu] for part %s", j, controlledJoints, partName.c_str());
        return false;
    }
    size_t subIndex = device.lut[j].deviceEntry;

    *min = 0.0;
    *max = 0.0;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (!p->lim) {
        return false;
    }
    return p->lim->getVelLimits(static_cast<int>(off + p->base), min, max);
}

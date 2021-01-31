/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "ControlBoardWrapperPWMControl.h"

#include "ControlBoardWrapperLogComponent.h"


bool ControlBoardWrapperPWMControl::setRefDutyCycle(int j, double v)
{
    size_t off;
    try {
        off = device.lut.at(j).offset;
    } catch (...) {
        yCError(CONTROLBOARDWRAPPER, "Joint number %d out of bound [0-%zu] for part %s", j, controlledJoints, partName.c_str());
        return false;
    }
    size_t subIndex = device.lut[j].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (p->iPWM) {
        return p->iPWM->setRefDutyCycle(static_cast<int>(off + p->base), v);
    }
    return false;
}

bool ControlBoardWrapperPWMControl::setRefDutyCycles(const double* v)
{
    for (size_t l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        size_t subIndex = device.lut[l].deviceEntry;
        auto* p = device.getSubdevice(subIndex);

        if (!p || !p->iPWM || !p->iPWM->setRefDutyCycle(static_cast<int>(off + p->base), v[l])) {
            return false;
        }
    }

    return true;
}

bool ControlBoardWrapperPWMControl::getRefDutyCycle(int j, double* v)
{
    size_t off;
    try {
        off = device.lut.at(j).offset;
    } catch (...) {
        yCError(CONTROLBOARDWRAPPER, "Joint number %d out of bound [0-%zu] for part %s", j, controlledJoints, partName.c_str());
        return false;
    }
    size_t subIndex = device.lut[j].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (p->iPWM) {
        return p->iPWM->getRefDutyCycle(static_cast<int>(off + p->base), v);
    }
    return false;
}

bool ControlBoardWrapperPWMControl::getRefDutyCycles(double* v)
{
    for (size_t l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        size_t subIndex = device.lut[l].deviceEntry;
        auto* p = device.getSubdevice(subIndex);

        if (!p || !p->iPWM || !p->iPWM->getRefDutyCycle(static_cast<int>(off + p->base), &v[l])) {
            return false;
        }
    }

    return true;
}

bool ControlBoardWrapperPWMControl::getDutyCycle(int j, double* v)
{
    size_t off;
    try {
        off = device.lut.at(j).offset;
    } catch (...) {
        yCError(CONTROLBOARDWRAPPER, "Joint number %d out of bound [0-%zu] for part %s", j, controlledJoints, partName.c_str());
        return false;
    }
    size_t subIndex = device.lut[j].deviceEntry;

    SubDevice* p = device.getSubdevice(subIndex);
    if (!p) {
        return false;
    }

    if (p->iPWM) {
        return p->iPWM->getDutyCycle(static_cast<int>(off + p->base), v);
    }
    return false;
}

bool ControlBoardWrapperPWMControl::getDutyCycles(double* v)
{
    for (size_t l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        size_t subIndex = device.lut[l].deviceEntry;
        auto* p = device.getSubdevice(subIndex);

        if (!p || !p->iPWM || !p->iPWM->getDutyCycle(static_cast<int>(off + p->base), &v[l])) {
            return false;
        }
    }

    return true;
}

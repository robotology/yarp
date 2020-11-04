/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "ControlBoardWrapperImpedanceControl.h"

#include "ControlBoardWrapperLogComponent.h"


bool ControlBoardWrapperImpedanceControl::setImpedance(int j, double stiff, double damp)
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

    if (p->iImpedance) {
        return p->iImpedance->setImpedance(off + p->base, stiff, damp);
    }

    return false;
}


bool ControlBoardWrapperImpedanceControl::setImpedanceOffset(int j, double offset)
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

    if (p->iImpedance) {
        return p->iImpedance->setImpedanceOffset(off + p->base, offset);
    }

    return false;
}


bool ControlBoardWrapperImpedanceControl::getImpedance(int j, double* stiff, double* damp)
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

    if (p->iImpedance) {
        return p->iImpedance->getImpedance(off + p->base, stiff, damp);
    }

    return false;
}


bool ControlBoardWrapperImpedanceControl::getImpedanceOffset(int j, double* offset)
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

    if (p->iImpedance) {
        return p->iImpedance->getImpedanceOffset(off + p->base, offset);
    }

    return false;
}


bool ControlBoardWrapperImpedanceControl::getCurrentImpedanceLimit(int j, double* min_stiff, double* max_stiff, double* min_damp, double* max_damp)
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

    if (p->iImpedance) {
        return p->iImpedance->getCurrentImpedanceLimit(off + p->base, min_stiff, max_stiff, min_damp, max_damp);
    }

    return false;
}

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ControlBoardWrapperImpedanceControl.h"

#include "ControlBoardLogComponent.h"


bool ControlBoardWrapperImpedanceControl::setImpedance(int j, double stiff, double damp)
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

    if (p->iImpedance) {
        return p->iImpedance->setImpedance(static_cast<int>(off + p->base), stiff, damp);
    }

    return false;
}


bool ControlBoardWrapperImpedanceControl::setImpedanceOffset(int j, double offset)
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

    if (p->iImpedance) {
        return p->iImpedance->setImpedanceOffset(static_cast<int>(off + p->base), offset);
    }

    return false;
}


bool ControlBoardWrapperImpedanceControl::getImpedance(int j, double* stiff, double* damp)
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

    if (p->iImpedance) {
        return p->iImpedance->getImpedance(static_cast<int>(off + p->base), stiff, damp);
    }

    return false;
}


bool ControlBoardWrapperImpedanceControl::getImpedanceOffset(int j, double* offset)
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

    if (p->iImpedance) {
        return p->iImpedance->getImpedanceOffset(static_cast<int>(off + p->base), offset);
    }

    return false;
}


bool ControlBoardWrapperImpedanceControl::getCurrentImpedanceLimit(int j, double* min_stiff, double* max_stiff, double* min_damp, double* max_damp)
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

    if (p->iImpedance) {
        return p->iImpedance->getCurrentImpedanceLimit(static_cast<int>(off + p->base), min_stiff, max_stiff, min_damp, max_damp);
    }

    return false;
}

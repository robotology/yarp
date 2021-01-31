/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "ControlBoardWrapperCurrentControl.h"

#include "ControlBoardWrapperLogComponent.h"

#include <numeric> // std::iota
#include <vector>


bool ControlBoardWrapperCurrentControl::getCurrentRange(int j, double* min, double* max)
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

    if (p->iCurr) {
        return p->iCurr->getCurrentRange(static_cast<int>(off + p->base), min, max);
    }

    return false;
}

bool ControlBoardWrapperCurrentControl::getCurrentRanges(double* min, double* max)
{
    for (size_t l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        size_t subIndex = device.lut[l].deviceEntry;
        auto* p = device.getSubdevice(subIndex);

        if (!p || !p->iCurr || !p->iCurr->getCurrentRange(static_cast<int>(off + p->base), &min[l], &max[l])) {
            return false;
        }
    }

    return true;
}

bool ControlBoardWrapperCurrentControl::setRefCurrents(const double* t)
{
    int j_wrap = 0;

    for (size_t subDev_idx = 0; subDev_idx < device.subdevices.size(); subDev_idx++) {
        auto* p = device.getSubdevice(subDev_idx);

        if (p && p->iCurr) {
            std::vector<int> joints((p->top - p->base) + 1);
            std::iota(joints.begin(), joints.end(), p->base);

            if (!p->iCurr->setRefCurrents(joints.size(), joints.data(), &t[j_wrap])) {
                return false;
            }

            j_wrap += joints.size();
        } else {
            return false;
        }
    }

    return true;
}

bool ControlBoardWrapperCurrentControl::setRefCurrent(int j, double t)
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

    if (p->iCurr) {
        return p->iCurr->setRefCurrent(static_cast<int>(off + p->base), t);
    }
    return false;
}

bool ControlBoardWrapperCurrentControl::setRefCurrents(const int n_joint, const int* joints, const double* t)
{
    bool ret = true;

    rpcDataMutex.lock();
    //Reset subdev_jointsVectorLen vector
    memset(rpcData.subdev_jointsVectorLen, 0x00, sizeof(int) * rpcData.deviceNum);

    // Create a map of joints for each subDevice
    size_t subIndex = 0;
    for (int j = 0; j < n_joint; j++) {
        subIndex = device.lut[joints[j]].deviceEntry;
        rpcData.jointNumbers[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] =
            static_cast<int>(device.lut[joints[j]].offset + rpcData.subdevices_p[subIndex]->base);
        rpcData.values[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = t[j];
        rpcData.subdev_jointsVectorLen[subIndex]++;
    }

    for (subIndex = 0; subIndex < rpcData.deviceNum; subIndex++) {
        if (rpcData.subdevices_p[subIndex]->iCurr) {
            ret = ret && rpcData.subdevices_p[subIndex]->iCurr->setRefCurrents(rpcData.subdev_jointsVectorLen[subIndex], rpcData.jointNumbers[subIndex], rpcData.values[subIndex]);
        } else {
            ret = false;
        }
    }
    rpcDataMutex.unlock();
    return ret;
}

bool ControlBoardWrapperCurrentControl::getRefCurrents(double* t)
{
    for (size_t l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        size_t subIndex = device.lut[l].deviceEntry;
        auto* p = device.getSubdevice(subIndex);

        if (!p || !p->iCurr || !p->iCurr->getRefCurrent(static_cast<int>(off + p->base), &t[l])) {
            return false;
        }
    }

    return true;
}

bool ControlBoardWrapperCurrentControl::getRefCurrent(int j, double* t)
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

    if (p->iCurr) {
        return p->iCurr->getRefCurrent(static_cast<int>(off + p->base), t);
    }

    return false;
}

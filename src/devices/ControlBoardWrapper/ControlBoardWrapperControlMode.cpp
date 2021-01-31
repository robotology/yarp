/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "ControlBoardWrapperControlMode.h"

#include "ControlBoardWrapperLogComponent.h"

#include <numeric> // std::iota
#include <vector>


bool ControlBoardWrapperControlMode::getControlMode(int j, int* mode)
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

    if (p->iMode) {
        return p->iMode->getControlMode(static_cast<int>(off + p->base), mode);
    }
    return false;
}


bool ControlBoardWrapperControlMode::getControlModes(int* modes)
{
    int j_wrap = 0;

    for (size_t subDev_idx = 0; subDev_idx < device.subdevices.size(); subDev_idx++) {
        auto* p = device.getSubdevice(subDev_idx);

        if (p && p->iMode) {
            std::vector<int> joints((p->top - p->base) + 1);
            std::iota(joints.begin(), joints.end(), p->base);

            if (!p->iMode->getControlModes(joints.size(), joints.data(), &modes[j_wrap])) {
                return false;
            }

            j_wrap += joints.size();
        } else {
            return false;
        }
    }

    return true;
}


bool ControlBoardWrapperControlMode::getControlModes(const int n_joint, const int* joints, int* modes)
{
    bool ret = true;

    for (int l = 0; l < n_joint; l++) {
        int off = device.lut[joints[l]].offset;
        size_t subIndex = device.lut[joints[l]].deviceEntry;

        SubDevice* p = device.getSubdevice(subIndex);
        if (!p) {
            return false;
        }

        if (p->iMode) {
            ret = ret && p->iMode->getControlMode(static_cast<int>(off + p->base), &modes[l]);
        } else {
            ret = false;
        }
    }
    return ret;
}


bool ControlBoardWrapperControlMode::setControlMode(const int j, const int mode)
{
    bool ret = true;
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

    if (p->iMode) {
        ret = p->iMode->setControlMode(static_cast<int>(off + p->base), mode);
    }
    return ret;
}


bool ControlBoardWrapperControlMode::setControlModes(const int n_joints, const int* joints, int* modes)
{
    bool ret = true;

    rpcDataMutex.lock();
    //Reset subdev_jointsVectorLen vector
    memset(rpcData.subdev_jointsVectorLen, 0x00, sizeof(int) * rpcData.deviceNum);

    // Create a map of joints for each subDevice
    size_t subIndex = 0;
    for (int j = 0; j < n_joints; j++) {
        subIndex = device.lut[joints[j]].deviceEntry;
        rpcData.jointNumbers[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] =
            static_cast<int>(device.lut[joints[j]].offset + rpcData.subdevices_p[subIndex]->base);
        rpcData.modes[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = modes[j];
        rpcData.subdev_jointsVectorLen[subIndex]++;
    }

    for (subIndex = 0; subIndex < rpcData.deviceNum; subIndex++) {
        if (rpcData.subdevices_p[subIndex]->iMode) {
            ret = ret && rpcData.subdevices_p[subIndex]->iMode->setControlModes(rpcData.subdev_jointsVectorLen[subIndex], rpcData.jointNumbers[subIndex], rpcData.modes[subIndex]);
        } else {
            ret = false;
        }
    }
    rpcDataMutex.unlock();
    return ret;
}


bool ControlBoardWrapperControlMode::setControlModes(int* modes)
{
    int j_wrap = 0;

    for (size_t subDev_idx = 0; subDev_idx < device.subdevices.size(); subDev_idx++) {
        auto* p = device.getSubdevice(subDev_idx);

        if (p && p->iMode) {
            std::vector<int> joints((p->top - p->base) + 1);
            std::iota(joints.begin(), joints.end(), p->base);

            if (!p->iMode->setControlModes(joints.size(), joints.data(), &modes[j_wrap])) {
                return false;
            }

            j_wrap += joints.size();
        } else {
            return false;
        }
    }

    return true;
}

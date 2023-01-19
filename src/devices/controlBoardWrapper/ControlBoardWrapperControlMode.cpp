/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ControlBoardWrapperControlMode.h"

#include "ControlBoardLogComponent.h"


bool ControlBoardWrapperControlMode::getControlMode(int j, int* mode)
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

    if (p->iMode) {
        return p->iMode->getControlMode(static_cast<int>(off + p->base), mode);
    }
    return false;
}


bool ControlBoardWrapperControlMode::getControlModes(int* modes)
{
    int* all_mode = new int[device.maxNumOfJointsInDevices];
    bool ret = true;
    for (size_t d = 0; d < device.subdevices.size(); d++) {
        SubDevice* p = device.getSubdevice(d);
        if (!p) {
            ret = false;
            break;
        }

        if ((p->iMode) && (ret = p->iMode->getControlModes(all_mode))) {
            for (size_t juser = p->wbase, jdevice = p->base; juser <= p->wtop; juser++, jdevice++) {
                modes[juser] = all_mode[jdevice];
            }
        } else {
            printError("getControlModes", p->id, ret);
            ret = false;
            break;
        }
    }

    delete[] all_mode;
    return ret;
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
        yCError(CONTROLBOARD, "Joint number %d out of bound [0-%zu] for part %s", j, controlledJoints, partName.c_str());
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
    bool ret = true;
    int j_wrap = 0; // index of the wrapper joint

    int nDev = device.subdevices.size();
    for (int subDev_idx = 0; subDev_idx < nDev; subDev_idx++) {
        size_t subIndex = device.lut[j_wrap].deviceEntry;
        SubDevice* p = device.getSubdevice(subIndex);
        if (!p) {
            return false;
        }

        int wrapped_joints = static_cast<int>((p->top - p->base) + 1);
        int* joints = new int[wrapped_joints];

        if (p->iMode) {
            // versione comandi su subset di giunti
            for (int j_dev = 0; j_dev < wrapped_joints; j_dev++) {
                joints[j_dev] = static_cast<int>(p->base + j_dev); // for all joints is equivalent to add offset term
            }

            ret = ret && p->iMode->setControlModes(wrapped_joints, joints, &modes[j_wrap]);
            j_wrap += wrapped_joints;
        }

        if (joints != nullptr) {
            delete[] joints;
            joints = nullptr;
        }
    }

    return ret;
}

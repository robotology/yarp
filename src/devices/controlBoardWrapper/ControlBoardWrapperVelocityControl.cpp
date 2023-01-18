/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ControlBoardWrapperVelocityControl.h"

#include <yarp/os/LogStream.h>

#include "ControlBoardLogComponent.h"

bool ControlBoardWrapperVelocityControl::velocityMove(int j, double v)
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

    if (p->vel) {
        return p->vel->velocityMove(static_cast<int>(off + p->base), v);
    }
    return false;
}

bool ControlBoardWrapperVelocityControl::velocityMove(const double* v)
{
    bool ret = true;
    int j_wrap = 0; // index of the wrapper joint

    for (size_t subDev_idx = 0; subDev_idx < device.subdevices.size(); subDev_idx++) {
        SubDevice* p = device.getSubdevice(subDev_idx);

        if (!p) {
            return false;
        }

        int wrapped_joints = static_cast<int>((p->top - p->base) + 1);
        int* joints = new int[wrapped_joints];

        if (p->vel) {
            // verione comandi su subset di giunti
            for (int j_dev = 0; j_dev < wrapped_joints; j_dev++) {
                joints[j_dev] = static_cast<int>(p->base + j_dev);
            }

            ret = ret && p->vel->velocityMove(wrapped_joints, joints, &v[j_wrap]);
            j_wrap += wrapped_joints;
        } else {
            ret = false;
        }

        if (joints != nullptr) {
            delete[] joints;
            joints = nullptr;
        }
    }

    return ret;
}

bool ControlBoardWrapperVelocityControl::velocityMove(const int n_joints, const int* joints, const double* spds)
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
        rpcData.values[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = spds[j];
        rpcData.subdev_jointsVectorLen[subIndex]++;
    }

    for (subIndex = 0; subIndex < rpcData.deviceNum; subIndex++) {
        if (rpcData.subdevices_p[subIndex]->vel) {
            ret = ret && rpcData.subdevices_p[subIndex]->vel->velocityMove(rpcData.subdev_jointsVectorLen[subIndex], rpcData.jointNumbers[subIndex], rpcData.values[subIndex]);
        } else {
            ret = false;
        }
    }
    rpcDataMutex.unlock();
    return ret;
}

bool ControlBoardWrapperVelocityControl::getRefVelocity(const int j, double* vel)
{
    yCTrace(CONTROLBOARD);

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

    if (p->vel) {
        bool ret = p->vel->getRefVelocity(static_cast<int>(off + p->base), vel);
        return ret;
    }
    *vel = 0;
    return false;
}


bool ControlBoardWrapperVelocityControl::getRefVelocities(double* vels)
{
    auto* references = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for (size_t d = 0; d < device.subdevices.size(); d++) {
        SubDevice* p = device.getSubdevice(d);
        if (!p) {
            ret = false;
            break;
        }

        if ((p->vel) && (ret = p->vel->getRefVelocities(references))) {
            for (size_t juser = p->wbase, jdevice = p->base; juser <= p->wtop; juser++, jdevice++) {
                vels[juser] = references[jdevice];
            }
        } else {
            printError("getRefVelocities", p->id, ret);
            ret = false;
            break;
        }
    }

    delete[] references;
    return ret;
}

bool ControlBoardWrapperVelocityControl::getRefVelocities(const int n_joints, const int* joints, double* vels)
{
    yCTrace(CONTROLBOARD);

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
        rpcData.subdev_jointsVectorLen[subIndex]++;
    }

    for (subIndex = 0; subIndex < rpcData.deviceNum; subIndex++) {
        if (rpcData.subdevices_p[subIndex]->vel) {
            ret = ret && rpcData.subdevices_p[subIndex]->vel->getRefVelocities(rpcData.subdev_jointsVectorLen[subIndex], rpcData.jointNumbers[subIndex], rpcData.values[subIndex]);
        }
    }

    if (ret) {
        // ReMix values by user expectations
        for (size_t i = 0; i < rpcData.deviceNum; i++) {
            rpcData.subdev_jointsVectorLen[i] = 0; // reset tmp index
        }

        // fill the output vector
        for (int j = 0; j < n_joints; j++) {
            subIndex = device.lut[joints[j]].deviceEntry;
            vels[j] = rpcData.values[subIndex][rpcData.subdev_jointsVectorLen[subIndex]];
            rpcData.subdev_jointsVectorLen[subIndex]++;
        }
    } else {
        for (int j = 0; j < n_joints; j++) {
            vels[j] = 0;
        }
    }
    rpcDataMutex.unlock();
    return ret;
}

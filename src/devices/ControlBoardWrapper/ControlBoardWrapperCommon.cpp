/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "ControlBoardWrapperCommon.h"

#include "ControlBoardWrapperLogComponent.h"

#include <numeric> // std::iota
#include <vector>


bool ControlBoardWrapperCommon::getAxes(int* ax)
{
    *ax = static_cast<int>(controlledJoints);
    return true;
}


bool ControlBoardWrapperCommon::setRefAcceleration(int j, double acc)
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

    if (p->pos) {
        return p->pos->setRefAcceleration(static_cast<int>(off + p->base), acc);
    }
    return false;
}


bool ControlBoardWrapperCommon::setRefAccelerations(const double* accs)
{
    bool ret = true;
    int j_wrap = 0; // index of the joint from the wrapper side (useful if wrapper joins 2 subdevices)

    // for all subdevices
    for (size_t subDev_idx = 0; subDev_idx < device.subdevices.size(); subDev_idx++) {
        SubDevice* p = device.getSubdevice(subDev_idx);

        if (!p) {
            return false;
        }

        int wrapped_joints = static_cast<int>((p->top - p->base) + 1);
        int* joints = new int[wrapped_joints]; // to be defined once and for all?

        if (p->pos) {
            // verione comandi su subset di giunti
            for (int j_dev = 0; j_dev < wrapped_joints; j_dev++) {
                joints[j_dev] = static_cast<int>(p->base + j_dev);
            }

            ret = ret && p->pos->setRefAccelerations(wrapped_joints, joints, &accs[j_wrap]);
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


bool ControlBoardWrapperCommon::setRefAccelerations(const int n_joints, const int* joints, const double* accs)
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
        rpcData.values[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = accs[j];
        rpcData.subdev_jointsVectorLen[subIndex]++;
    }

    for (subIndex = 0; subIndex < rpcData.deviceNum; subIndex++) {
        if (rpcData.subdevices_p[subIndex]->pos) {
            ret = ret && rpcData.subdevices_p[subIndex]->pos->setRefAccelerations(rpcData.subdev_jointsVectorLen[subIndex], rpcData.jointNumbers[subIndex], rpcData.values[subIndex]);
        } else {
            ret = false;
        }
    }
    rpcDataMutex.unlock();
    return ret;
}


bool ControlBoardWrapperCommon::getRefAcceleration(int j, double* acc)
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

    if (p->pos) {
        return p->pos->getRefAcceleration(static_cast<int>(off + p->base), acc);
    }
    *acc = 0;
    return false;
}


bool ControlBoardWrapperCommon::getRefAccelerations(double* accs)
{
    int j_wrap = 0;

    for (size_t subDev_idx = 0; subDev_idx < device.subdevices.size(); subDev_idx++) {
        auto* p = device.getSubdevice(subDev_idx);

        if (p && p->pos) {
            std::vector<int> joints((p->top - p->base) + 1);
            std::iota(joints.begin(), joints.end(), p->base);

            if (!p->pos->getRefAccelerations(joints.size(), joints.data(), &accs[j_wrap])) {
                return false;
            }

            j_wrap += joints.size();
        } else {
            return false;
        }
    }

    return true;
}


bool ControlBoardWrapperCommon::getRefAccelerations(const int n_joints, const int* joints, double* accs)
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
        rpcData.subdev_jointsVectorLen[subIndex]++;
    }

    for (subIndex = 0; subIndex < rpcData.deviceNum; subIndex++) {
        if (rpcData.subdevices_p[subIndex]->pos) {
            ret = ret && rpcData.subdevices_p[subIndex]->pos->getRefAccelerations(rpcData.subdev_jointsVectorLen[subIndex], rpcData.jointNumbers[subIndex], rpcData.values[subIndex]);
        } else {
            ret = false;
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
            accs[j] = rpcData.values[subIndex][rpcData.subdev_jointsVectorLen[subIndex]];
            rpcData.subdev_jointsVectorLen[subIndex]++;
        }
    } else {
        for (int j = 0; j < n_joints; j++) {
            accs[j] = 0;
        }
    }
    rpcDataMutex.unlock();
    return ret;
}


bool ControlBoardWrapperCommon::stop(int j)
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

    if (p->pos) {
        return p->pos->stop(static_cast<int>(off + p->base));
    }
    return false;
}


bool ControlBoardWrapperCommon::stop()
{
    for (size_t subDev_idx = 0; subDev_idx < device.subdevices.size(); subDev_idx++) {
        auto* p = device.getSubdevice(subDev_idx);

        if (p && p->pos) {
            std::vector<int> joints((p->top - p->base) + 1);
            std::iota(joints.begin(), joints.end(), p->base);

            if (!p->pos->stop(joints.size(), joints.data())) {
                return false;
            }
        } else {
            return false;
        }
    }

    return true;
}


bool ControlBoardWrapperCommon::stop(const int n_joints, const int* joints)
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
        rpcData.subdev_jointsVectorLen[subIndex]++;
    }

    for (subIndex = 0; subIndex < rpcData.deviceNum; subIndex++) {
        if (rpcData.subdevices_p[subIndex]->pos) {
            ret = ret && rpcData.subdevices_p[subIndex]->pos->stop(rpcData.subdev_jointsVectorLen[subIndex], rpcData.jointNumbers[subIndex]);
        } else {
            ret = false;
        }
    }
    rpcDataMutex.unlock();
    return ret;
}


bool ControlBoardWrapperCommon::getNumberOfMotors(int* num)
{
    *num = static_cast<int>(controlledJoints);
    return true;
}


bool ControlBoardWrapperCommon::getCurrents(double* vals)
{
    for (size_t l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        size_t subIndex = device.lut[l].deviceEntry;
        auto* p = device.getSubdevice(subIndex);

        if (p) {
            int j = static_cast<int>(off + p->base);

            if (p->iCurr && p->iCurr->getCurrent(j, &vals[l])) {
                continue;
            } else if (p->amp && p->amp->getCurrent(j, &vals[l])) {
                continue;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }

    return true;
}


bool ControlBoardWrapperCommon::getCurrent(int j, double* val)
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
        return p->iCurr->getCurrent(static_cast<int>(off + p->base), val);
    }

    if (p->amp) {
        return p->amp->getCurrent(static_cast<int>(off + p->base), val);
    }
    *val = 0.0;
    return false;
}

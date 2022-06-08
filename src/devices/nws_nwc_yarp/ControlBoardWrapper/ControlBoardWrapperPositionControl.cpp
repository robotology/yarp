/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ControlBoardWrapperPositionControl.h"

#include "ControlBoardLogComponent.h"


bool ControlBoardWrapperPositionControl::positionMove(int j, double ref)
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

    if (p->pos) {
        return p->pos->positionMove(static_cast<int>(off + p->base), ref);
    }

    return false;
}


bool ControlBoardWrapperPositionControl::positionMove(const double* refs)
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

        if (p->pos) {
            // versione comandi su subset di giunti
            for (int j_dev = 0; j_dev < wrapped_joints; j_dev++) {
                joints[j_dev] = static_cast<int>(p->base + j_dev); // for all joints is equivalent to add offset term
            }

            ret = ret && p->pos->positionMove(wrapped_joints, joints, &refs[j_wrap]);
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


bool ControlBoardWrapperPositionControl::positionMove(const int n_joints, const int* joints, const double* refs)
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
        rpcData.values[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = refs[j];
        rpcData.subdev_jointsVectorLen[subIndex]++;
    }

    for (subIndex = 0; subIndex < rpcData.deviceNum; subIndex++) {
        if (rpcData.subdevices_p[subIndex]->pos) {
            ret = ret && rpcData.subdevices_p[subIndex]->pos->positionMove(rpcData.subdev_jointsVectorLen[subIndex], rpcData.jointNumbers[subIndex], rpcData.values[subIndex]);
        } else {
            ret = false;
        }
    }
    rpcDataMutex.unlock();
    return ret;
}


bool ControlBoardWrapperPositionControl::getTargetPosition(const int j, double* ref)
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

    if (p->pos) {
        bool ret = p->pos->getTargetPosition(static_cast<int>(off + p->base), ref);
        return ret;
    }
    *ref = 0;
    return false;
}


bool ControlBoardWrapperPositionControl::getTargetPositions(double* spds)
{
    auto* targets = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for (size_t d = 0; d < device.subdevices.size(); d++) {
        SubDevice* p = device.getSubdevice(d);
        if (!p) {
            ret = false;
            break;
        }

        if ((p->pos) && (ret = p->pos->getTargetPositions(targets))) {
            for (size_t juser = p->wbase, jdevice = p->base; juser <= p->wtop; juser++, jdevice++) {
                spds[juser] = targets[jdevice];
            }
        } else {
            printError("getTargetPositions", p->id, ret);
            ret = false;
            break;
        }
    }

    delete[] targets;
    return ret;
}


bool ControlBoardWrapperPositionControl::getTargetPositions(const int n_joints, const int* joints, double* targets)
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
            ret = ret && rpcData.subdevices_p[subIndex]->pos->getTargetPositions(rpcData.subdev_jointsVectorLen[subIndex], rpcData.jointNumbers[subIndex], rpcData.values[subIndex]);
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
            targets[j] = rpcData.values[subIndex][rpcData.subdev_jointsVectorLen[subIndex]];
            rpcData.subdev_jointsVectorLen[subIndex]++;
        }
    } else {
        for (int j = 0; j < n_joints; j++) {
            targets[j] = 0;
        }
    }
    rpcDataMutex.unlock();
    return ret;
}


bool ControlBoardWrapperPositionControl::relativeMove(int j, double delta)
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

    if (p->pos) {
        return p->pos->relativeMove(static_cast<int>(off + p->base), delta);
    }

    return false;
}


bool ControlBoardWrapperPositionControl::relativeMove(const double* deltas)
{
    bool ret = true;

    for (size_t l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        size_t subIndex = device.lut[l].deviceEntry;

        SubDevice* p = device.getSubdevice(subIndex);
        if (!p) {
            return false;
        }

        if (p->pos) {
            ret = ret && p->pos->relativeMove(static_cast<int>(off + p->base), deltas[l]);
        } else {
            ret = false;
        }
    }
    return ret;
}


bool ControlBoardWrapperPositionControl::relativeMove(const int n_joints, const int* joints, const double* deltas)
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
        rpcData.values[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = deltas[j];
        rpcData.subdev_jointsVectorLen[subIndex]++;
    }

    for (subIndex = 0; subIndex < rpcData.deviceNum; subIndex++) {
        if (rpcData.subdevices_p[subIndex]->pos) {
            ret = ret && rpcData.subdevices_p[subIndex]->pos->relativeMove(rpcData.subdev_jointsVectorLen[subIndex], rpcData.jointNumbers[subIndex], rpcData.values[subIndex]);
        } else {
            ret = false;
        }
    }
    rpcDataMutex.unlock();
    return ret;
}


bool ControlBoardWrapperPositionControl::checkMotionDone(int j, bool* flag)
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

    if (p->pos) {
        return p->pos->checkMotionDone(static_cast<int>(off + p->base), flag);
    }

    return false;
}


bool ControlBoardWrapperPositionControl::checkMotionDone(bool* flag)
{
    bool ret = true;

    rpcDataMutex.lock();
    //Reset subdev_jointsVectorLen vector
    memset(rpcData.subdev_jointsVectorLen, 0x00, sizeof(int) * rpcData.deviceNum);

    // Create a map of joints for each subDevice
    // In this case the "all joint version" of checkMotionDone(bool *flag) cannot be
    // called because the return value is an 'and' of all joints.
    // Therefore only the corret joints must be evaluated.

    size_t subIndex = 0;
    for (size_t j = 0; j < controlledJoints; j++) {
        subIndex = device.lut[j].deviceEntry;
        rpcData.jointNumbers[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] =
            static_cast<int>(device.lut[j].offset + rpcData.subdevices_p[subIndex]->base);
        rpcData.subdev_jointsVectorLen[subIndex]++;
    }

    bool tmp_subdeviceDone = true;
    bool tmp_deviceDone = true;

    // for each subdevice wrapped call checkmotiondone only on interested joints
    for (subIndex = 0; subIndex < rpcData.deviceNum; subIndex++) {
        if (rpcData.subdevices_p[subIndex]->pos) {
            ret = ret && rpcData.subdevices_p[subIndex]->pos->checkMotionDone(rpcData.subdev_jointsVectorLen[subIndex], rpcData.jointNumbers[subIndex], &tmp_subdeviceDone);
            tmp_deviceDone &= tmp_subdeviceDone;
        }
    }
    rpcDataMutex.unlock();

    // return a single value to the caller
    *flag = tmp_deviceDone;
    return ret;
}


bool ControlBoardWrapperPositionControl::checkMotionDone(const int n_joints, const int* joints, bool* flags)
{
    bool ret = true;
    bool tmp = true;
    bool XFlags = true;

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
            ret = ret && rpcData.subdevices_p[subIndex]->pos->checkMotionDone(rpcData.subdev_jointsVectorLen[subIndex], rpcData.jointNumbers[subIndex], &XFlags);
            tmp = tmp && XFlags;
        } else {
            ret = false;
        }
    }
    if (ret) {
        *flags = tmp;
    } else {
        *flags = false;
    }
    rpcDataMutex.unlock();
    return ret;
}


bool ControlBoardWrapperPositionControl::setRefSpeed(int j, double sp)
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

    if (p->pos) {
        return p->pos->setRefSpeed(static_cast<int>(off + p->base), sp);
    }
    return false;
}


bool ControlBoardWrapperPositionControl::setRefSpeeds(const double* spds)
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

        if (p->pos) {
            // verione comandi su subset di giunti
            for (int j_dev = 0; j_dev < wrapped_joints; j_dev++) {
                joints[j_dev] = static_cast<int>(p->base + j_dev);
            }

            ret = ret && p->pos->setRefSpeeds(wrapped_joints, joints, &spds[j_wrap]);
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


bool ControlBoardWrapperPositionControl::setRefSpeeds(const int n_joints, const int* joints, const double* spds)
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
        if (rpcData.subdevices_p[subIndex]->pos) {
            ret = ret && rpcData.subdevices_p[subIndex]->pos->setRefSpeeds(rpcData.subdev_jointsVectorLen[subIndex], rpcData.jointNumbers[subIndex], rpcData.values[subIndex]);
        } else {
            ret = false;
        }
    }
    rpcDataMutex.unlock();
    return ret;
}


bool ControlBoardWrapperPositionControl::getRefSpeed(int j, double* ref)
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

    if (p->pos) {
        return p->pos->getRefSpeed(static_cast<int>(off + p->base), ref);
    }
    *ref = 0;
    return false;
}


bool ControlBoardWrapperPositionControl::getRefSpeeds(double* spds)
{
    auto* references = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for (size_t d = 0; d < device.subdevices.size(); d++) {
        SubDevice* p = device.getSubdevice(d);
        if (!p) {
            ret = false;
            break;
        }

        if ((p->pos) && (ret = p->pos->getRefSpeeds(references))) {
            for (size_t juser = p->wbase, jdevice = p->base; juser <= p->wtop; juser++, jdevice++) {
                spds[juser] = references[jdevice];
            }
        } else {
            printError("getRefSpeeds", p->id, ret);
            ret = false;
            break;
        }
    }

    delete[] references;
    return ret;
}


bool ControlBoardWrapperPositionControl::getRefSpeeds(const int n_joints, const int* joints, double* spds)
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
            ret = ret && rpcData.subdevices_p[subIndex]->pos->getRefSpeeds(rpcData.subdev_jointsVectorLen[subIndex], rpcData.jointNumbers[subIndex], rpcData.values[subIndex]);
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
            spds[j] = rpcData.values[subIndex][rpcData.subdev_jointsVectorLen[subIndex]];
            rpcData.subdev_jointsVectorLen[subIndex]++;
        }
    } else {
        for (int j = 0; j < n_joints; j++) {
            spds[j] = 0;
        }
    }
    rpcDataMutex.unlock();
    return ret;
}

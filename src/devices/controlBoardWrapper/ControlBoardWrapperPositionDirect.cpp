/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ControlBoardWrapperPositionDirect.h"

#include "ControlBoardLogComponent.h"


bool ControlBoardWrapperPositionDirect::setPosition(int j, double ref)
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

    if (p->posDir) {
        return p->posDir->setPosition(static_cast<int>(off + p->base), ref);
    }

    return false;
}


bool ControlBoardWrapperPositionDirect::setPositions(const int n_joints, const int* joints, const double* dpos)
{
    bool ret = true;

    rpcDataMutex.lock();
    //Reset subdev_jointsVectorLen vector
    memset(rpcData.subdev_jointsVectorLen, 0x00, sizeof(int) * rpcData.deviceNum);

    // Create a map of joints for each subDevice
    size_t subIndex = 0;
    for (int j = 0; j < n_joints; j++) {
        subIndex = device.lut[joints[j]].deviceEntry;
        int offset = device.lut[joints[j]].offset;
        int base = rpcData.subdevices_p[subIndex]->base;
        rpcData.jointNumbers[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = offset + base;
        rpcData.values[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = dpos[j];
        rpcData.subdev_jointsVectorLen[subIndex]++;
    }

    for (subIndex = 0; subIndex < rpcData.deviceNum; subIndex++) {
        if (rpcData.subdevices_p[subIndex]->posDir) // Position Direct
        {
            ret = ret && rpcData.subdevices_p[subIndex]->posDir->setPositions(rpcData.subdev_jointsVectorLen[subIndex], rpcData.jointNumbers[subIndex], rpcData.values[subIndex]);
        } else {
            ret = false;
        }
    }
    rpcDataMutex.unlock();
    return ret;
}


bool ControlBoardWrapperPositionDirect::setPositions(const double* refs)
{
    bool ret = true;

    for (size_t l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        size_t subIndex = device.lut[l].deviceEntry;

        SubDevice* p = device.getSubdevice(subIndex);
        if (!p) {
            return false;
        }

        if (p->posDir) {
            ret = p->posDir->setPosition(static_cast<int>(off + p->base), refs[l]) && ret;
        } else {
            ret = false;
        }
    }
    return ret;
}


bool ControlBoardWrapperPositionDirect::getRefPosition(const int j, double* ref)
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

    if (p->posDir) {
        bool ret = p->posDir->getRefPosition(static_cast<int>(off + p->base), ref);
        return ret;
    }
    *ref = 0;
    return false;
}


bool ControlBoardWrapperPositionDirect::getRefPositions(double* spds)
{
    auto* references = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for (size_t d = 0; d < device.subdevices.size(); d++) {
        SubDevice* p = device.getSubdevice(d);
        if (!p) {
            ret = false;
            break;
        }

        if ((p->posDir) && (ret = p->posDir->getRefPositions(references))) {
            for (size_t juser = p->wbase, jdevice = p->base; juser <= p->wtop; juser++, jdevice++) {
                spds[juser] = references[jdevice];
            }
        } else {
            printError("getRefPositions", p->id, ret);
            ret = false;
            break;
        }
    }

    delete[] references;
    return ret;
}


bool ControlBoardWrapperPositionDirect::getRefPositions(const int n_joints, const int* joints, double* targets)
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
        if (rpcData.subdevices_p[subIndex]->posDir) {
            ret = ret && rpcData.subdevices_p[subIndex]->posDir->getRefPositions(rpcData.subdev_jointsVectorLen[subIndex], rpcData.jointNumbers[subIndex], rpcData.values[subIndex]);
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

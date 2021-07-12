/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ControlBoardWrapperCurrentControl.h"

#include "ControlBoardLogComponent.h"


bool ControlBoardWrapperCurrentControl::getCurrentRange(int j, double* min, double* max)
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

    if (p->iCurr) {
        return p->iCurr->getCurrentRange(static_cast<int>(off + p->base), min, max);
    }

    return false;
}

bool ControlBoardWrapperCurrentControl::getCurrentRanges(double* min, double* max)
{
    auto* c_min = new double[device.maxNumOfJointsInDevices];
    auto* c_max = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for (size_t d = 0; d < device.subdevices.size(); d++) {
        SubDevice* p = device.getSubdevice(d);
        if (!p) {
            ret = false;
            break;
        }

        if ((p->iCurr) && (ret = p->iCurr->getCurrentRanges(c_min, c_max))) {
            for (size_t juser = p->wbase, jdevice = p->base; juser <= p->wtop; juser++, jdevice++) {
                min[juser] = c_min[jdevice];
                max[juser] = c_max[jdevice];
            }
        } else {
            printError("getCurrentRanges", p->id, ret);
            ret = false;
            break;
        }
    }

    delete[] c_min;
    delete[] c_max;
    return ret;
}

bool ControlBoardWrapperCurrentControl::setRefCurrents(const double* t)
{
    bool ret = true;

    for (size_t l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        size_t subIndex = device.lut[l].deviceEntry;

        SubDevice* p = device.getSubdevice(subIndex);
        if (!p) {
            return false;
        }

        if (p->iCurr) {
            ret = ret && p->iCurr->setRefCurrent(static_cast<int>(off + p->base), t[l]);
        } else {
            ret = false;
        }
    }
    return ret;
}

bool ControlBoardWrapperCurrentControl::setRefCurrent(int j, double t)
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
    auto* references = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for (size_t d = 0; d < device.subdevices.size(); d++) {
        SubDevice* p = device.getSubdevice(d);
        if (!p) {
            ret = false;
            break;
        }

        if ((p->iCurr) && (ret = p->iCurr->getRefCurrents(references))) {
            for (size_t juser = p->wbase, jdevice = p->base; juser <= p->wtop; juser++, jdevice++) {
                t[juser] = references[jdevice];
            }
        } else {
            printError("getRefCurrents", p->id, ret);
            ret = false;
            break;
        }
    }

    delete[] references;
    return ret;
}

bool ControlBoardWrapperCurrentControl::getRefCurrent(int j, double* t)
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

    if (p->iCurr) {
        return p->iCurr->getRefCurrent(static_cast<int>(off + p->base), t);
    }

    return false;
}

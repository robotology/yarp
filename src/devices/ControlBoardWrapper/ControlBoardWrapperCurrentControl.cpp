/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "ControlBoardWrapperCurrentControl.h"

#include "ControlBoardWrapperLogComponent.h"


bool ControlBoardWrapperCurrentControl::getCurrentRange(int j, double* min, double* max)
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

    if (p->iCurr) {
        return p->iCurr->getCurrentRange(off + p->base, min, max);
    }

    return false;
}

bool ControlBoardWrapperCurrentControl::getCurrentRanges(double* min, double* max)
{
    auto* c_min = new double[device.maxNumOfJointsInDevices];
    auto* c_max = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for (unsigned int d = 0; d < device.subdevices.size(); d++) {
        SubDevice* p = device.getSubdevice(d);
        if (!p) {
            ret = false;
            break;
        }

        if ((p->iCurr) && (ret = p->iCurr->getCurrentRanges(c_min, c_max))) {
            for (int juser = p->wbase, jdevice = p->base; juser <= p->wtop; juser++, jdevice++) {
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

    for (int l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        int subIndex = device.lut[l].deviceEntry;

        SubDevice* p = device.getSubdevice(subIndex);
        if (!p) {
            return false;
        }

        if (p->iCurr) {
            ret = ret && p->iCurr->setRefCurrent(off + p->base, t[l]);
        } else {
            ret = false;
        }
    }
    return ret;
}

bool ControlBoardWrapperCurrentControl::setRefCurrent(int j, double t)
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

    if (p->iCurr) {
        return p->iCurr->setRefCurrent(off + p->base, t);
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
    int subIndex = 0;
    for (int j = 0; j < n_joint; j++) {
        subIndex = device.lut[joints[j]].deviceEntry;
        rpcData.jointNumbers[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = device.lut[joints[j]].offset + rpcData.subdevices_p[subIndex]->base;
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
    for (unsigned int d = 0; d < device.subdevices.size(); d++) {
        SubDevice* p = device.getSubdevice(d);
        if (!p) {
            ret = false;
            break;
        }

        if ((p->iCurr) && (ret = p->iCurr->getRefCurrents(references))) {
            for (int juser = p->wbase, jdevice = p->base; juser <= p->wtop; juser++, jdevice++) {
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

    if (p->iCurr) {
        return p->iCurr->getRefCurrent(off + p->base, t);
    }

    return false;
}

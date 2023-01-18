/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ControlBoardWrapperTorqueControl.h"

#include "ControlBoardLogComponent.h"


bool ControlBoardWrapperTorqueControl::getRefTorques(double* refs)
{
    auto* references = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for (size_t d = 0; d < device.subdevices.size(); d++) {
        SubDevice* p = device.getSubdevice(d);
        if (!p) {
            ret = false;
            break;
        }

        if ((p->iTorque) && (ret = p->iTorque->getRefTorques(references))) {
            for (size_t juser = p->wbase, jdevice = p->base; juser <= p->wtop; juser++, jdevice++) {
                refs[juser] = references[jdevice];
            }
        } else {
            printError("getRefTorques", p->id, ret);
            ret = false;
            break;
        }
    }

    delete[] references;
    return ret;
}

bool ControlBoardWrapperTorqueControl::getRefTorque(int j, double* t)
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

    if (p->iTorque) {
        return p->iTorque->getRefTorque(static_cast<int>(off + p->base), t);
    }
    return false;
}

bool ControlBoardWrapperTorqueControl::setRefTorques(const double* t)
{
    bool ret = true;

    for (size_t l = 0; l < controlledJoints; l++) {
        int off = device.lut[l].offset;
        size_t subIndex = device.lut[l].deviceEntry;

        SubDevice* p = device.getSubdevice(subIndex);
        if (!p) {
            return false;
        }

        if (p->iTorque) {
            ret = ret && p->iTorque->setRefTorque(static_cast<int>(off + p->base), t[l]);
        } else {
            ret = false;
        }
    }
    return ret;
}

bool ControlBoardWrapperTorqueControl::setRefTorque(int j, double t)
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

    if (p->iTorque) {
        return p->iTorque->setRefTorque(static_cast<int>(off + p->base), t);
    }
    return false;
}

bool ControlBoardWrapperTorqueControl::setRefTorques(const int n_joints, const int* joints, const double* t)
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
        rpcData.values[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = t[j];
        rpcData.subdev_jointsVectorLen[subIndex]++;
    }

    for (subIndex = 0; subIndex < rpcData.deviceNum; subIndex++) {
        if (rpcData.subdevices_p[subIndex]->iTorque) {
            ret = ret && rpcData.subdevices_p[subIndex]->iTorque->setRefTorques(rpcData.subdev_jointsVectorLen[subIndex], rpcData.jointNumbers[subIndex], rpcData.values[subIndex]);
        } else {
            ret = false;
        }
    }
    rpcDataMutex.unlock();
    return ret;
}

bool ControlBoardWrapperTorqueControl::getMotorTorqueParams(int j, yarp::dev::MotorTorqueParameters* params)
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

    if (p->iTorque) {
        return p->iTorque->getMotorTorqueParams(static_cast<int>(off + p->base), params);
    }
    return false;
}

bool ControlBoardWrapperTorqueControl::setMotorTorqueParams(int j, const yarp::dev::MotorTorqueParameters params)
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

    if (p->iTorque) {
        return p->iTorque->setMotorTorqueParams(static_cast<int>(off + p->base), params);
    }
    return false;
}

bool ControlBoardWrapperTorqueControl::getTorque(int j, double* t)
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

    if (p->iTorque) {
        return p->iTorque->getTorque(static_cast<int>(off + p->base), t);
    }

    return false;
}

bool ControlBoardWrapperTorqueControl::getTorques(double* t)
{
    auto* trqs = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for (size_t d = 0; d < device.subdevices.size(); d++) {
        SubDevice* p = device.getSubdevice(d);
        if (!p) {
            ret = false;
            break;
        }

        if ((p->iTorque) && (ret = p->iTorque->getTorques(trqs))) {
            for (size_t juser = p->wbase, jdevice = p->base; juser <= p->wtop; juser++, jdevice++) {
                t[juser] = trqs[jdevice];
            }
        } else {
            printError("getTorques", p->id, ret);
            ret = false;
            break;
        }
    }

    delete[] trqs;
    return ret;
}

bool ControlBoardWrapperTorqueControl::getTorqueRange(int j, double* min, double* max)
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

    if (p->iTorque) {
        return p->iTorque->getTorqueRange(static_cast<int>(off + p->base), min, max);
    }

    return false;
}

bool ControlBoardWrapperTorqueControl::getTorqueRanges(double* min, double* max)
{
    auto* t_min = new double[device.maxNumOfJointsInDevices];
    auto* t_max = new double[device.maxNumOfJointsInDevices];
    bool ret = true;
    for (size_t d = 0; d < device.subdevices.size(); d++) {
        SubDevice* p = device.getSubdevice(d);
        if (!p) {
            ret = false;
            break;
        }

        if ((p->iTorque) && (ret = p->iTorque->getTorqueRanges(t_min, t_max))) {
            for (size_t juser = p->wbase, jdevice = p->base; juser <= p->wtop; juser++, jdevice++) {
                min[juser] = t_min[jdevice];
                max[juser] = t_max[jdevice];
            }
        } else {
            printError("getTorqueRanges", p->id, ret);
            ret = false;
            break;
        }
    }

    delete[] t_min;
    delete[] t_max;
    return ret;
}

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ControlBoardWrapperInteractionMode.h"

#include "ControlBoardLogComponent.h"

using yarp::dev::VOCAB_IM_UNKNOWN;

bool ControlBoardWrapperInteractionMode::getInteractionMode(int j, yarp::dev::InteractionModeEnum* mode)
{
    size_t off;
    try {
        off = device.lut.at(j).offset;
    } catch (...) {
        yCError(CONTROLBOARD, "Joint number %d out of bound [0-%zu] for part %s", j, controlledJoints, partName.c_str());
        return false;
    }
    size_t subIndex = device.lut[j].deviceEntry;

    SubDevice* s = device.getSubdevice(subIndex);
    if (!s) {
        return false;
    }

    if (s->iInteract) {
        return s->iInteract->getInteractionMode(static_cast<int>(off + s->base), mode);
    }
    return false;
}

bool ControlBoardWrapperInteractionMode::getInteractionModes(int n_joints, int* joints, yarp::dev::InteractionModeEnum* modes)
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
        if (rpcData.subdevices_p[subIndex]->iInteract) {
            ret = ret && rpcData.subdevices_p[subIndex]->iInteract->getInteractionModes(
                    rpcData.subdev_jointsVectorLen[subIndex],
                    rpcData.jointNumbers[subIndex],
                    reinterpret_cast<yarp::dev::InteractionModeEnum*>(rpcData.modes[subIndex]));
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
            modes[j] = static_cast<yarp::dev::InteractionModeEnum>(rpcData.modes[subIndex][rpcData.subdev_jointsVectorLen[subIndex]]);
            rpcData.subdev_jointsVectorLen[subIndex]++;
        }
    } else {
        for (int j = 0; j < n_joints; j++) {
            modes[j] = VOCAB_IM_UNKNOWN;
        }
    }
    rpcDataMutex.unlock();
    return ret;
}

bool ControlBoardWrapperInteractionMode::getInteractionModes(yarp::dev::InteractionModeEnum* modes)
{

    auto* imodes = new yarp::dev::InteractionModeEnum[device.maxNumOfJointsInDevices];
    bool ret = true;
    for (size_t d = 0; d < device.subdevices.size(); d++) {
        SubDevice* p = device.getSubdevice(d);
        if (!p) {
            ret = false;
            break;
        }

        if ((p->iInteract) && (ret = p->iInteract->getInteractionModes(imodes))) {
            for (size_t juser = p->wbase, jdevice = p->base; juser <= p->wtop; juser++, jdevice++) {
                modes[juser] = imodes[jdevice];
            }
        } else {
            printError("getInteractionModes", p->id, ret);
            ret = false;
            break;
        }
    }

    delete[] imodes;
    return ret;
}

bool ControlBoardWrapperInteractionMode::setInteractionMode(int j, yarp::dev::InteractionModeEnum mode)
{
    size_t off;
    try {
        off = device.lut.at(j).offset;
    } catch (...) {
        yCError(CONTROLBOARD, "Joint number %d out of bound [0-%zu] for part %s", j, controlledJoints, partName.c_str());
        return false;
    }
    size_t subIndex = device.lut[j].deviceEntry;

    SubDevice* s = device.getSubdevice(subIndex);
    if (!s) {
        return false;
    }

    if (s->iInteract) {
        return s->iInteract->setInteractionMode(static_cast<int>(off + s->base), mode);
    }
    return false;
}

bool ControlBoardWrapperInteractionMode::setInteractionModes(int n_joints, int* joints, yarp::dev::InteractionModeEnum* modes)
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
        rpcData.modes[subIndex][rpcData.subdev_jointsVectorLen[subIndex]] = static_cast<int>(modes[j]);
        rpcData.subdev_jointsVectorLen[subIndex]++;
    }

    for (subIndex = 0; subIndex < rpcData.deviceNum; subIndex++) {
        if (rpcData.subdevices_p[subIndex]->iInteract) {
            ret = ret && rpcData.subdevices_p[subIndex]->iInteract->setInteractionModes(
                    rpcData.subdev_jointsVectorLen[subIndex],
                    rpcData.jointNumbers[subIndex],
                    reinterpret_cast<yarp::dev::InteractionModeEnum*>(rpcData.modes[subIndex]));
        } else {
            ret = false;
        }
    }
    rpcDataMutex.unlock();
    return ret;
}

bool ControlBoardWrapperInteractionMode::setInteractionModes(yarp::dev::InteractionModeEnum* modes)
{
    bool ret = true;

    for (size_t j = 0; j < controlledJoints; j++) {
        size_t off;
        try {
            off = device.lut.at(j).offset;
        } catch (...) {
            yCError(CONTROLBOARD, "Joint number %zu out of bound [0-%zu] for part %s", j, controlledJoints, partName.c_str());
            return false;
        }
        size_t subIndex = device.lut[j].deviceEntry;

        SubDevice* p = device.getSubdevice(subIndex);
        if (!p) {
            return false;
        }

        if (p->iInteract) {
            ret = ret && p->iInteract->setInteractionMode(static_cast<int>(off + p->base), modes[j]);
        } else {
            ret = false;
        }
    }
    return ret;
}

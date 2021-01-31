/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "ControlBoardWrapperInteractionMode.h"

#include "ControlBoardWrapperLogComponent.h"

#include <numeric> // std::iota
#include <vector>

using yarp::dev::VOCAB_IM_UNKNOWN;

bool ControlBoardWrapperInteractionMode::getInteractionMode(int j, yarp::dev::InteractionModeEnum* mode)
{
    size_t off;
    try {
        off = device.lut.at(j).offset;
    } catch (...) {
        yCError(CONTROLBOARDWRAPPER, "Joint number %d out of bound [0-%zu] for part %s", j, controlledJoints, partName.c_str());
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
    int j_wrap = 0;

    for (size_t subDev_idx = 0; subDev_idx < device.subdevices.size(); subDev_idx++) {
        auto* p = device.getSubdevice(subDev_idx);

        if (p && p->iInteract) {
            std::vector<int> joints((p->top - p->base) + 1);
            std::iota(joints.begin(), joints.end(), p->base);

            if (!p->iInteract->getInteractionModes(joints.size(), joints.data(), &modes[j_wrap])) {
                return false;
            }

            j_wrap += joints.size();
        } else {
            return false;
        }
    }

    return true;
}

bool ControlBoardWrapperInteractionMode::setInteractionMode(int j, yarp::dev::InteractionModeEnum mode)
{
    size_t off;
    try {
        off = device.lut.at(j).offset;
    } catch (...) {
        yCError(CONTROLBOARDWRAPPER, "Joint number %d out of bound [0-%zu] for part %s", j, controlledJoints, partName.c_str());
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
    int j_wrap = 0;

    for (size_t subDev_idx = 0; subDev_idx < device.subdevices.size(); subDev_idx++) {
        auto* p = device.getSubdevice(subDev_idx);

        if (p && p->iInteract) {
            std::vector<int> joints((p->top - p->base) + 1);
            std::iota(joints.begin(), joints.end(), p->base);

            if (!p->iInteract->setInteractionModes(joints.size(), joints.data(), &modes[j_wrap])) {
                return false;
            }

            j_wrap += joints.size();
        } else {
            return false;
        }
    }

    return true;
}

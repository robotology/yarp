/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "SubDevice.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

#include "ControlBoardWrapper.h"
#include "ControlBoardLogComponent.h"
#include "RPCMessagesParser.h"
#include "StreamingMessagesParser.h"
#include <iostream>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace std;


bool SubDevice::configure(size_t wb, size_t wt, size_t b, size_t t, size_t n, const std::string& key, const std::string& _parentName)
{
    parentName = _parentName;

    configuredF = false;

    wbase = wb;
    wtop = wt;
    base = b;
    top = t;
    axes = n;
    id = key;

    if (top < base) {
        yCError(CONTROLBOARD) << "Check configuration file top<base.";
        return false;
    }

    if ((top - base + 1) != axes) {
        yCError(CONTROLBOARD) << "Check configuration file, number of axes and top/base parameters do not match";
        return false;
    }

    if (axes <= 0) {
        yCError(CONTROLBOARD) << "Check number of axes";
        return false;
    }

    subDev_joint_encoders.resize(axes);
    jointEncodersTimes.resize(axes);
    subDev_motor_encoders.resize(axes);
    motorEncodersTimes.resize(axes);

    configuredF = true;
    return true;
}

void SubDevice::detach()
{
    subdevice = nullptr;

    pid = nullptr;
    pos = nullptr;
    posDir = nullptr;
    vel = nullptr;
    amp = nullptr;
    iJntEnc = nullptr;
    iMotEnc = nullptr;
    lim = nullptr;
    calib = nullptr;
    info = nullptr;
    iTorque = nullptr;
    iImpedance = nullptr;
    iMode = nullptr;
    iTimed = nullptr;
    iInteract = nullptr;
    iVar = nullptr;
    configuredF = false;
    attachedF = false;
}

bool SubDevice::attach(yarp::dev::PolyDriver* d, const std::string& k)
{
    if (id != k) {
        yCError(CONTROLBOARD, "Part <%s>: Wrong or unknown device %s. Cannot attach to it.", parentName.c_str(), k.c_str());
        return false;
    }

    //configure first
    if (!configuredF) {
        yCError(CONTROLBOARD, "Part <%s>: You need to call configure before you can attach any device", parentName.c_str());
        return false;
    }

    if (d == nullptr) {
        yCError(CONTROLBOARD, "Part <%s>: Invalid device (null pointer)", parentName.c_str());
        return false;
    }

    subdevice = d;

    if (subdevice->isValid()) {
        subdevice->view(pid);
        subdevice->view(pos);
        subdevice->view(posDir);
        subdevice->view(vel);
        subdevice->view(amp);
        subdevice->view(lim);
        subdevice->view(calib);
        subdevice->view(info);
        subdevice->view(iTimed);
        subdevice->view(iTorque);
        subdevice->view(iImpedance);
        subdevice->view(iMode);
        subdevice->view(iJntEnc);
        subdevice->view(iMotEnc);
        subdevice->view(iInteract);
        subdevice->view(imotor);
        subdevice->view(iVar);
        subdevice->view(iCurr);
        subdevice->view(iPWM);
    } else {
        yCError(CONTROLBOARD, "Part <%s>: Invalid device %s (isValid() returned false).", parentName.c_str(), k.c_str());
        return false;
    }

    if (!iMode) {
        yCDebug(CONTROLBOARD, "Part <%s>: iMode not valid interface.", parentName.c_str());
    }

    if (!iTorque) {
        yCDebug(CONTROLBOARD, "Part <%s>: iTorque not valid interface.", parentName.c_str());
    }

    if (!iCurr) {
        yCDebug(CONTROLBOARD, "Part <%s>: iCurr not valid interface.", parentName.c_str());
    }

    if (!iPWM) {
        yCDebug(CONTROLBOARD, "Part <%s>: iPWM not valid interface.", parentName.c_str());
    }

    if (!iImpedance) {
        yCDebug(CONTROLBOARD, "Part <%s>: iImpedance not valid interface.", parentName.c_str());
    }

    if (!iInteract) {
        yCDebug(CONTROLBOARD, "Part <%s>: iInteractionMode not valid interface.", parentName.c_str());
    }

    if (!iMotEnc) {
        yCDebug(CONTROLBOARD, "Part <%s>: iMotorEncoder not valid interface.", parentName.c_str());
    }

    if (!imotor) {
        yCDebug(CONTROLBOARD, "Part <%s>: iMotor not valid interface.", parentName.c_str());
    }

    if (!iVar) {
        yCDebug(CONTROLBOARD, "Part <%s>: iRemoteVariable not valid interface.", parentName.c_str());
    }

    if (!info) {
        yCDebug(CONTROLBOARD, "Part <%s>: iAxisInfo not valid interface.", parentName.c_str());
    }

    size_t deviceJoints = 0;

    // checking minimum set of intefaces required
    if (!pos) {
        yCError(CONTROLBOARD, "Part <%s>: IPositionControl interface was not found in subdevice. Quitting", parentName.c_str());
        return false;
    }

    if (!vel) {
        yCError(CONTROLBOARD, "Part <%s>: IVelocityControl interface was not found in subdevice. Quitting", parentName.c_str());
        return false;
    }

    if (!iJntEnc) {
        yCError(CONTROLBOARD, "Part <%s>: IEncoderTimed interface was not found in subdevice.", parentName.c_str());
        return false;
    }

    if (pos != nullptr) {
        int tmp_axes;
        if (!pos->getAxes(&tmp_axes)) {
            yCError(CONTROLBOARD) << "Failed to get axes number for subdevice " << k.c_str();
            return false;
        }
        if (tmp_axes <= 0) {
            yCError(CONTROLBOARD, "Part <%s>: attached device has an invalid number of joints (%d)", parentName.c_str(), tmp_axes);
            return false;
        }
        deviceJoints = static_cast<size_t>(tmp_axes);
    } else {
        int tmp_axes;
        if (!pos->getAxes(&tmp_axes)) {
            yCError(CONTROLBOARD, "Part <%s>: failed to get axes number for subdevice %s.", parentName.c_str(), k.c_str());
            return false;
        }
        if (tmp_axes <= 0) {
            yCError(CONTROLBOARD, "Part <%s>: attached device has an invalid number of joints (%d)", parentName.c_str(), tmp_axes);
            return false;
        }
        deviceJoints = static_cast<size_t>(tmp_axes);
    }

    if (deviceJoints < axes) {
        yCError(CONTROLBOARD, "Part <%s>: check device configuration, number of joints of attached device '%zu' less \
                than the one specified during configuration '%zu' for %s.",
                parentName.c_str(),
                deviceJoints,
                axes,
                k.c_str());
        return false;
    }

    int subdevAxes;
    if (!pos || !pos->getAxes(&subdevAxes)) {
        yCError(CONTROLBOARD) << "Device <" << parentName << "> attached to subdevice " << k.c_str() << " but it was not ready yet. \n"
                                     << "Please check the device has been correctly created and all required initialization actions has been performed.";
        return false;
    }

    totalAxis = deviceJoints;
    attachedF = true;
    return true;
}

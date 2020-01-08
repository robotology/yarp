/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "SubDevice.h"
#include "ControlBoardWrapper.h"
#include "StreamingMessagesParser.h"
#include "RPCMessagesParser.h"
#include <iostream>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace std;


SubDevice::SubDevice() :
    base(-1),
    top(-1),
    axes(0),
    configuredF(false),
    parent(nullptr),
    subdevice(nullptr),
    pid(nullptr),
    pos(nullptr),
    vel(nullptr),
    iJntEnc(nullptr),
    iMotEnc(nullptr),
    amp(nullptr),
    lim(nullptr),
    calib(nullptr),
    iTimed(nullptr),
    iTorque(nullptr),
    iImpedance(nullptr),
    iMode(nullptr),
    info(nullptr),
    posDir(nullptr),
    iInteract(nullptr),
    imotor(nullptr),
    iVar(nullptr),
    iPWM(nullptr),
    iCurr(nullptr),
    _subDevVerbose(false),
    attachedF(false)
{}

bool SubDevice::configure(int wb, int wt, int b, int t, int n, const std::string &key, ControlBoardWrapper *_parent)
{
    parent = _parent;
    configuredF=false;

    wbase = wb;
    wtop = wt;
    base=b;
    top=t;
    axes=n;
    id=key;

    if (top<base)
        {
            yError()<<"controlBoardWrapper: check configuration file top<base.";
            return false;
        }

    if ((top-base+1)!=axes)
        {
            yError()<<"controlBoardWrapper: check configuration file, number of axes and top/base parameters do not match";
            return false;
        }

    if (axes<=0)
        {
            yError()<<"controlBoardWrapper: check number of axes";
            return false;
        }

    subDev_joint_encoders.resize(axes);
    jointEncodersTimes.resize(axes);
    subDev_motor_encoders.resize(axes);
    motorEncodersTimes.resize(axes);

    configuredF=true;
    return true;
}

void SubDevice::detach()
{
    subdevice=nullptr;

    pid=nullptr;
    pos=nullptr;
    posDir=nullptr;
    vel=nullptr;
    amp = nullptr;
    iJntEnc=nullptr;
    iMotEnc=nullptr;
    lim=nullptr;
    calib=nullptr;
    info=nullptr;
    iTorque=nullptr;
    iImpedance=nullptr;
    iMode=nullptr;
    iTimed=nullptr;
    iInteract=nullptr;
    iVar = nullptr;
    configuredF=false;
    attachedF=false;
}

bool SubDevice::attach(yarp::dev::PolyDriver *d, const std::string &k)
{
    std::string parentName;
    if(parent)
    {
        parentName = parent->getId();
    }
    else
        parentName = "";

    if (id!=k)
    {
        yError("ControlBoardWrapper for part <%s>: Wrong or unknown device %s. Cannot attach to it.", parentName.c_str(), k.c_str());
        return false;
    }

    //configure first
    if (!configuredF)
        {
            yError("ControlBoardWrapper for part <%s>: You need to call configure before you can attach any device", parentName.c_str());
            return false;
        }

    if (d==nullptr)
        {
            yError("ControlBoardWrapper for part <%s>: Invalid device (null pointer)", parentName.c_str());
            return false;
        }

    subdevice=d;

    if (subdevice->isValid())
        {
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
        }
    else
        {
            yError("ControlBoardWrapper for part <%s>: Invalid device %s (isValid() returned false).", parentName.c_str(), k.c_str());
            return false;
        }

    if ( ((iMode==nullptr)) && (_subDevVerbose ))
        yWarning("ControlBoardWrapper for part <%s>:  Warning iMode not valid interface.", parentName.c_str());

    if ((iTorque==nullptr) && (_subDevVerbose))
        yWarning("ControlBoardWrapper for part <%s>:  Warning iTorque not valid interface.", parentName.c_str());

    if ((iCurr == nullptr) && (_subDevVerbose))
        yWarning("ControlBoardWrapper for part <%s>:  Warning iCurr not valid interface.", parentName.c_str());

    if ((iPWM == nullptr) && (_subDevVerbose))
        yWarning("ControlBoardWrapper for part <%s>:  Warning iPWM not valid interface.", parentName.c_str());

    if ((iImpedance==nullptr) && (_subDevVerbose))
        yWarning("ControlBoardWrapper for part <%s>:  Warning iImpedance not valid interface.", parentName.c_str());

    if ((iInteract==nullptr) && (_subDevVerbose))
        yWarning("ControlBoardWrapper for part <%s>:  Warning iInteractionMode not valid interface.", parentName.c_str());

    if ((iMotEnc==nullptr) && (_subDevVerbose))
        yWarning("ControlBoardWrapper for part <%s>:  Warning iMotorEncoder not valid interface.", parentName.c_str());

    if ((imotor==nullptr) && (_subDevVerbose))
        yWarning("ControlBoardWrapper for part <%s>:  Warning iMotor not valid interface.", parentName.c_str());

    if ((iVar == nullptr) && (_subDevVerbose))
        yWarning("ControlBoardWrapper for part <%s>:  Warning iRemoteVariable not valid interface.", parentName.c_str());

    if ((info == nullptr) && (_subDevVerbose))
        yWarning("ControlBoardWrapper for part <%s>:  Warning iAxisInfo not valid interface.", parentName.c_str());

    int deviceJoints=0;

    // checking minimum set of intefaces required
    if( ! (pos) )
    {
        yError("ControlBoarWrapper: neither IPositionControl nor IPositionControl2 interface was not found in subdevice. Quitting");
        return false;
    }

    if( ! (vel) )
    {
        yError("ControlBoarWrapper: neither IVelocityControl nor IVelocityControl2 interface was not found in subdevice. Quitting");
        return false;
    }

    if(!iJntEnc)
    {
        yError("ControlBoardWrapper for part <%s>: IEncoderTimed interface was not found in subdevice.", parentName.c_str());
        return false;
    }

    if (pos!=nullptr)
    {
        if (!pos->getAxes(&deviceJoints))
        {
            yError() << "ControlBoarWrapper: failed to get axes number for subdevice " << k.c_str();
            return false;
        }
        if(deviceJoints <= 0)
        {
            yError("ControlBoardWrapper for part <%s>: attached device has an invalid number of joints (%d)", parentName.c_str(), deviceJoints);
            return false;
        }
    }
    else
    {
        if (!pos->getAxes(&deviceJoints))
        {
            yError("ControlBoardWrapper for part <%s>: failed to get axes number for subdevice %s.", parentName.c_str(), k.c_str());
            return false;
        }
        if(deviceJoints <=0)
        {
            yError("ControlBoarWrapper for part <%s>: attached device has an invalid number of joints (%d)", parentName.c_str(), deviceJoints);
            return false;
        }
    }

    if (deviceJoints<axes)
    {
        yError("ControlBoarWrapper for part <%s>: check device configuration, number of joints of attached device '%d' less \
                than the one specified during configuration '%d' for %s.", parentName.c_str(), deviceJoints, axes, k.c_str());
        return false;
    }

    int subdevAxes;
    if(!pos || !pos->getAxes(&subdevAxes))
    {
        yError() << "ControlBoardWrapper for device <" << parentName << "> attached to subdevice " << k.c_str() << " but it was not ready yet. \n" \
                 << "Please check the device has been correctly created and all required initialization actions has been performed.";
                 return false;
    }

    totalAxis = deviceJoints;
    attachedF=true;
    return true;
}

/*
 * Copyright (C) 2009 RobotCub Consortium
 * Author: Lorenzo Natale
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include "ControlBoardWrapper.h"
#include "StreamingMessagesParser.h"
#include "RPCMessagesParser.h"
#include "SubDevice.h"
#include <iostream>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::dev::impl;
using namespace yarp::sig;
using namespace std;


SubDevice::SubDevice()
{
    pid = 0;
    pos = 0;
    pos2 = 0;
    posDir = 0;
    vel = 0;
    vel2 = 0;
    iJntEnc = 0;
    iMotEnc = 0;
    amp = 0;
    lim2 = 0;
    calib = 0;
    calib2 = 0;
    iTimed= 0;
    info = 0;
    iOpenLoop=0;
    iTorque=0;
    iImpedance=0;
    iMode=0;
    iMode2=0;
    iInteract=0;

    base=-1;
    top=-1;
    axes=0;

    subdevice=0;

    configuredF=false;
    attachedF=false;
    _subDevVerbose = false;
}

bool SubDevice::configure(int b, int t, int n, const std::string &key, yarp::dev::ControlBoardWrapper *_parent)
{
    parent = _parent;
    configuredF=false;

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
    subdevice=0;

    pid=0;
    pos=0;
    pos2=0;
    posDir=0;
    vel=0;
    vel2=0;
    amp = 0;
    iJntEnc=0;
    iMotEnc=0;
    lim2=0;
    calib=0;
    calib2=0;
    info=0;
    iTorque=0;
    iImpedance=0;
    iMode=0;
    iMode2=0;
    iTimed=0;
    iOpenLoop=0;
    iInteract=0;
    iVar = 0;
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
            yError()<<"controlBoardWrapper: Wrong device" << k.c_str();
            return false;
        }

    //configure first
    if (!configuredF)
        {
            yError()<<"controlBoardWrapper: You need to call configure before you can attach any device";
            return false;
        }

    if (d==0)
        {
            yError()<<"controlBoardWrapper: Invalid device (null pointer)";
            return false;
        }

    subdevice=d;

    if (subdevice->isValid())
        {
            subdevice->view(pid);
            subdevice->view(pos);
            subdevice->view(pos2);
            subdevice->view(posDir);
            subdevice->view(vel);
            subdevice->view(vel2);
            subdevice->view(amp);
            subdevice->view(lim2);
            subdevice->view(calib);
            subdevice->view(calib2);
            subdevice->view(info);
            subdevice->view(iTimed);
            subdevice->view(iTorque);
            subdevice->view(iImpedance);
            subdevice->view(iMode);
            subdevice->view(iMode2);
            subdevice->view(iOpenLoop);
            subdevice->view(iJntEnc);
            subdevice->view(iMotEnc);
            subdevice->view(iInteract);
            subdevice->view(imotor);
            subdevice->view(iVar);
        }
    else
        {
            yError()<<"controlBoardWrapper: Invalid device " << k << " (isValid() returned false)";
            return false;
        }

    if ( ((iMode==0) || (iMode2==0)) && (_subDevVerbose ))
        yWarning() << "controlBoardWrapper:  Warning iMode not valid interface";

    if ((iTorque==0) && (_subDevVerbose))
        yWarning() << "controlBoardWrapper:  Warning iTorque not valid interface";

    if ((iImpedance==0) && (_subDevVerbose))
        yWarning() << "controlBoardWrapper:  Warning iImpedance not valid interface";

    if ((iOpenLoop==0) && (_subDevVerbose))
        yWarning() << "controlBoardWrapper:  Warning iOpenLoop not valid interface";

    if ((iInteract==0) && (_subDevVerbose))
        yWarning() << "controlBoardWrapper:  Warning iInteractionMode not valid interface";

    if ((iMotEnc==0) && (_subDevVerbose))
        yWarning() << "controlBoardWrapper:  Warning iMotorEncoder not valid interface";

    if ((imotor==0) && (_subDevVerbose))
        yWarning() << "controlBoardWrapper:  Warning iMotor not valid interface";

    if ((iVar == 0) && (_subDevVerbose))
        yWarning() << "controlBoardWrapper:  Warning iVar not valid interface";

    if ((info == 0) && (_subDevVerbose))
        yWarning() << "controlBoardWrapper:  Warning info not valid interface";

    int deviceJoints=0;

    // checking minimum set of intefaces required
    if( ! (pos || pos2) ) // One of the 2 is enough, therefore if both are missing I raise an error
    {
        yError("ControlBoarWrapper: neither IPositionControl nor IPositionControl2 interface was not found in subdevice. Quitting");
        return false;
    }

    if( ! (vel || vel2) ) // One of the 2 is enough, therefore if both are missing I raise an error
    {
        yError("ControlBoarWrapper: neither IVelocityControl nor IVelocityControl2 interface was not found in subdevice. Quitting");
        return false;
    }
    else
    {
        // both have to be correct (and they should 'cause the vel2 is derived from 1. Use a workaround here, then investigate more!!
        if(vel2 && !vel)
            vel = vel2;
    }

    if(!iJntEnc)
    {
        yError("ControlBoarWrapper: IEncoderTimed interface was not found in subdevice");
        return false;
    }

    if (pos!=0)
    {
        if (!pos->getAxes(&deviceJoints))
        {
            yError() << "ControlBoarWrapper: failed to get axes number for subdevice " << k.c_str();
            return false;
        }
        if(deviceJoints <= 0)
        {
            yError("ControlBoarWrapper: attached device has an invalid number of joints (%d)", deviceJoints);
            return false;
        }
    }
    else
    {
        if (!pos2->getAxes(&deviceJoints))
        {
            yError() << "ControlBoarWrapper: failed to get axes number for subdevice " << k.c_str();
            return false;
        }
        if(deviceJoints <=0)
        {
            yError("ControlBoarWrapper: attached device has an invalid number of joints (%d)", deviceJoints);
            return false;
        }
    }

    if (deviceJoints<axes)
    {
        yError("ControlBoarWrapper: check device configuration, number of joints of attached device '%d' less than the one specified during configuration '%d' for %s.", deviceJoints, axes, k.c_str());
        return false;
    }

    int subdevAxes;
    if(!pos->getAxes(&subdevAxes))
    {

        yError() << "ControlBoardWrapper for device <" << parentName << "> attached to subdevice " << k.c_str() << " but it was not ready yet. \n" \
                 << "Please check the device has been correctly created and all required initialization actions has been performed.";
                 return false;
        attachedF=false;

    }
    attachedF=true;
    return true;
}

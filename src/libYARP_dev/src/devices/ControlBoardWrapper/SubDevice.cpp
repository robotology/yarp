/*
 * Copyright (C) 2009 RobotCub Consortium
 * Author: Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
    iTorque=0;
    iImpedance=0;
    iMode=0;
    iMode2=0;
    iInteract=0;
    iCurr = 0;
    iPWM = 0;

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
        yError("ControlBoardWrapper for part <%s>: Wrong or unknow device %s. Cannot attach to it.", parentName.c_str(), k.c_str());
        return false;
    }

    //configure first
    if (!configuredF)
        {
            yError("ControlBoardWrapper for part <%s>: You need to call configure before you can attach any device", parentName.c_str());
            return false;
        }

    if (d==0)
        {
            yError("ControlBoardWrapper for part <%s>: Invalid device (null pointer)", parentName.c_str());
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

    if ( ((iMode==0) || (iMode2==0)) && (_subDevVerbose ))
        yWarning("ControlBoardWrapper for part <%s>:  Warning iMode not valid interface.", parentName.c_str());

    if ((iTorque==0) && (_subDevVerbose))
        yWarning("ControlBoardWrapper for part <%s>:  Warning iTorque not valid interface.", parentName.c_str());

    if ((iCurr == 0) && (_subDevVerbose))
        yWarning("ControlBoardWrapper for part <%s>:  Warning iCurr not valid interface.", parentName.c_str());

    if ((iPWM == 0) && (_subDevVerbose))
        yWarning("ControlBoardWrapper for part <%s>:  Warning iPWM not valid interface.", parentName.c_str());

    if ((iImpedance==0) && (_subDevVerbose))
        yWarning("ControlBoardWrapper for part <%s>:  Warning iImpedance not valid interface.", parentName.c_str());

    if ((iInteract==0) && (_subDevVerbose))
        yWarning("ControlBoardWrapper for part <%s>:  Warning iInteractionMode not valid interface.", parentName.c_str());

    if ((iMotEnc==0) && (_subDevVerbose))
        yWarning("ControlBoardWrapper for part <%s>:  Warning iMotorEncoder not valid interface.", parentName.c_str());

    if ((imotor==0) && (_subDevVerbose))
        yWarning("ControlBoardWrapper for part <%s>:  Warning iMotor not valid interface.", parentName.c_str());

    if ((iVar == 0) && (_subDevVerbose))
        yWarning("ControlBoardWrapper for part <%s>:  Warning iRemoteVariable not valid interface.", parentName.c_str());

    if ((info == 0) && (_subDevVerbose))
        yWarning("ControlBoardWrapper for part <%s>:  Warning iAxisInfo not valid interface.", parentName.c_str());

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
        yError("ControlBoardWrapper for part <%s>: IEncoderTimed interface was not found in subdevice.", parentName.c_str());
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
            yError("ControlBoardWrapper for part <%s>: attached device has an invalid number of joints (%d)", parentName.c_str(), deviceJoints);
            return false;
        }
    }
    else
    {
        if (!pos2->getAxes(&deviceJoints))
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

    attachedF=true;
    return true;
}

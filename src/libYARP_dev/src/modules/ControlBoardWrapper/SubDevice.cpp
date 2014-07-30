// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

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
    enc = 0;
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

bool SubDevice::configure(int b, int t, int n, const std::string &key)
{
    configuredF=false;

    base=b;
    top=t;
    axes=n;
    id=key;

    if (top<base)
        {
            cerr<<"check configuration file top<base."<<endl;
            return false;
        }

    if ((top-base+1)!=axes)
        {
            cerr<<"check configuration file, number of axes and top/base parameters do not match"<<endl;
            return false;
        }

    if (axes<=0)
        {
            cerr<<"check number of axes"<<endl;
            return false;
        }

    subDev_encoders.resize(axes);
    encodersTimes.resize(axes);

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
    enc=0;
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
    configuredF=false;
    attachedF=false;
}

bool SubDevice::attach(yarp::dev::PolyDriver *d, const std::string &k)
{
    if (id!=k)
        {
            cerr<<"Wrong device sorry."<<endl;
            return false;
        }

    //configure first
    if (!configuredF)
        {
            cerr<<"You need to call configure before you can attach any device"<<endl;
            return false;
        }

    if (d==0)
        {
            cerr<<"Invalid device (null pointer)\n"<<endl;
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
            subdevice->view(enc);
            subdevice->view(iInteract);
        }
    else
        {
            cerr<<"Invalid device " << k << " (isValid() returned false)"<<endl;
            return false;
        }

    if ( ((iMode==0) || (iMode2==0)) && (_subDevVerbose ))
        std::cerr << "--> Warning iMode not valid interface\n";

    if ((iTorque==0) && (_subDevVerbose))
        std::cerr << "--> Warning iTorque not valid interface\n";

    if ((iImpedance==0) && (_subDevVerbose))
        std::cerr << "--> Warning iImpedance not valid interface\n";

    if ((iOpenLoop==0) && (_subDevVerbose))
        std::cerr << "--> Warning iOpenLoop not valid interface\n";

    if ((iInteract==0) && (_subDevVerbose))
        std::cerr << "--> Warning iInteractionMode not valid interface\n";

    int deviceJoints=0;

    // checking minimum set of intefaces required
    if( ! (pos || pos2) ) // One of the 2 is enough, therefore if both are missing I raise an error
    {
        printf("ControlBoarWrapper Error: neither IPositionControl nor IPositionControl2 interface was not found in subdevice. Quitting\n");
        return false;
    }

    if( ! (vel || vel2) ) // One of the 2 is enough, therefor if both are missing I raise an error
    {
        printf("ControlBoarWrapper Error: neither IVelocityControl nor IVelocityControl2 interface was not found in subdevice. Quitting\n");

        return false;
    }
    else
    {
        // both have to be correct (and they should 'cause the vel2 is derived from 1. Use a workaround here, then investigate more!!
        if(vel2 && !vel)
            vel = vel2;
    }

    if(!enc)
    {
        printf("ControlBoarWrapper Error: IEncoderTimed interface was not found in subdevice. Quitting\n");
        return false;
    }

    if (pos!=0)
    {
        if (!pos->getAxes(&deviceJoints))
        {
            std::cerr<< "Error: attached device has 0 axes\n";
            return false;
        }
    }
    else
    {
        if (!pos2->getAxes(&deviceJoints))
        {
            std::cerr<< "Error: attached device has 0 axes\n";
            return false;
        }
    }

    if (deviceJoints<axes)
    {
        std::cerr<<"check device configuration, number of joints of attached device less than the one specified during configuration.\n";
        return false;
    }
    attachedF=true;
    return true;
}

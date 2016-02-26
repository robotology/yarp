/*
 * Copyright (C) 2013 RobotCub Consortium
 * Author: Alberto Cardellino
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include "VirtualAnalogWrapper.h"
#include <iostream>
#include <yarp/os/LogStream.h>

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::dev::impl;

#define MAX_ENTRIES 255

// needed for the driver factory.
yarp::dev::DriverCreator *createVirtualAnalogWrapper() {
    return new DriverCreatorOf<yarp::dev::VirtualAnalogWrapper>("virtualAnalogServer",
        "virtualAnalogServer",
        "yarp::dev::VirtualAnalogWrapper");
}


AnalogSubDevice::AnalogSubDevice()
{
    detach();
}

AnalogSubDevice::~AnalogSubDevice()
{
    detach();
}

bool AnalogSubDevice::configure(int map0, int map1, const std::string &key)
{
    mIsConfigured=false;

    if (map1<map0)
    {
        yError()<<"VirtualAnalogWrapper: check configuration file top<base.";
        return false;
    }

    mMap0=map0;
    mMap1=map1;

    mKey=key;

    mTorques.resize(mMap1-mMap0+1);

    mIsConfigured=true;

    return true;
}

bool AnalogSubDevice::attach(yarp::dev::PolyDriver *device, const std::string &key)
{
    if (key!=mKey)
    {
        yError()<<"VirtualAnalogWrapper: wrong device" << key.c_str();
        return false;
    }

    //configure first
    if (!mIsConfigured)
    {
        yError()<<"VirtualAnalogWrapper: You need to call configure before you can attach any device";
        return false;
    }

    if (!device)
    {
        yError()<<"VirtualAnalogWrapper: Invalid device (null pointer)";
        return false;
    }

    mpDevice=device;

    if (mpDevice->isValid())
    {
        mpDevice->view(mpSensor);
    }
    else
    {
        yError()<<"VirtualAnalogWrapper: Invalid device " << key << " (isValid() returned false)";
        return false;
    }

    if (mpSensor)
    {
        mIsAttached=true;
        return true;
    }

    return false;
}

void AnalogSubDevice::detach()
{
    mMap0=mMap1=-1;

    mpDevice=NULL;
    mpSensor=NULL;

    mIsConfigured=false;
    mIsAttached=false;
}

bool VirtualAnalogWrapper::open(Searchable& config)
{
    yDebug() << config.toString().c_str();

    mIsVerbose = (config.check("verbose","if present, give detailed output"));

    if (mIsVerbose) yDebug() << "running with verbose output\n";

    //thus thread period is useful for output port... this input port has callback so maybe can skip it (?)
    //thread_period = prop.check("threadrate", 20, "thread rate in ms. for streaming encoder data").asInt();

    yDebug() << "Using VirtualAnalogServer\n";

    if (!config.check("networks", "list of networks merged by this wrapper"))
    {
        yError() << "VirtualAnalogWrapper: missing networks parameters";
        return false;
    }

    Bottle *networks=config.find("networks").asList();
    mNSubdevs=networks->size();
    mSubdevices.resize(mNSubdevs);

    mChan2Board.resize(MAX_ENTRIES);
    mChan2BAddr.resize(MAX_ENTRIES);
    for (int i=0; i< MAX_ENTRIES; i++)
    {
        mChan2Board[i]=-1;
        mChan2BAddr[i]=-1;
    }

    int totalJ=0;

    for (int k=0; k<networks->size(); ++k)
    {
        Bottle parameters=config.findGroup(networks->get(k).asString().c_str());

        if (parameters.size()!=5)    // mapping joints using the paradigm: part from - to / network from - to
        {
            yError() << "VirtualAnalogWrapper: check network parameters in part description"
                     << " I was expecting " << networks->get(k).asString().c_str() << " followed by four integers";
            return false;
        }

        int map0=parameters.get(1).asInt();
        int map1=parameters.get(2).asInt();
        int map2=parameters.get(3).asInt();
        int map3=parameters.get(4).asInt();
        if (map0 >= MAX_ENTRIES || map1 >= MAX_ENTRIES || map2>= MAX_ENTRIES || map3>= MAX_ENTRIES ||
            map0 <0             || map1 <0             || map2<0             || map3<0)
        {
            yError() << "VirtualAnalogWrapper: invalid map entries in networks section, failed initial check";
            return false;
        }

        for (int j=map0; j<=map1; ++j)
        {
            mChan2Board[j]=k;
            mChan2BAddr[j]=j-map0+map2;
        }

        if (!mSubdevices[k].configure(map2,map3,networks->get(k).asString().c_str()))
        {
            yError() << "VirtualAnalogWrapper: configure of subdevice ret false";
            return false;
        }

        totalJ+=map1-map0+1;
    }

    // Verify minimum set of parameters required
    if(!config.check("robotName") )   // ?? qui dentro, da dove lo pesco ??
    {
        yError() << "VirtualAnalogWrapper:  missing robotName, check your configuration file!";
        return false;
    }

    std::string root_name;
    std::string port_name = config.check("name",Value("controlboard"),"prefix for port names").asString().c_str();
    std::string robot_name = config.find("robotName").asString().c_str();

    root_name+="/";
    root_name+=robot_name;
    root_name+= "/joint_vsens" + port_name + ":i";

    if (!mPortInputTorques.open(root_name.c_str()))
    {
        yError() << "VirtualAnalogWrapper: can't open port " << root_name.c_str();
        return false;
    }

    return true;
}

bool VirtualAnalogWrapper::close()
{
    mPortInputTorques.interrupt();
    mPortInputTorques.close();
    Thread::stop();
    return true;
}

bool VirtualAnalogWrapper::attachAll(const PolyDriverList &polylist)
{
    mMutex.wait();

    for (int p=0; p<polylist.size(); ++p)
    {
        std::string key=polylist[p]->key.c_str();

        // find appropriate entry in list of subdevices and attach
        for (unsigned int k=0; k<mSubdevices.size(); ++k)
        {
            if (mSubdevices[k].getKey()==key)
            {
                if (!mSubdevices[k].attach(polylist[p]->poly,key))
                {
                    mMutex.post();
                    return false;
                }
            }
        }
    }

    //check if all devices are attached to the driver
    for (unsigned int k=0; k<mSubdevices.size(); ++k)
    {
        if (!mSubdevices[k].isAttached())
        {
            mMutex.post();
            return false;
        }
   }

    mMutex.post();

    Thread::start();

    return true;
}

bool VirtualAnalogWrapper::detachAll()
{
    mMutex.wait();

    for(int k=0; k<mNSubdevs; ++k)
    {
        mSubdevices[k].detach();
    }

    mMutex.post();

//     close();

    return true;
}

bool VirtualAnalogWrapper::perform_first_check(int elems)
{
    if (first_check) return true;

    for (int i=0; i<elems; i++)
    {
        if (mChan2Board[i]==-1 || mChan2BAddr[i]==-1)
        {
            yError() << "VirtualAnalogWrapper: invalid map entries in networks section, failed runtime check"
                     << " i: " << i << "mChan2Board[i] is " << mChan2Board[i] << " chan2add is " << mChan2BAddr[i];
            return false;
        }
    }

    yTrace() << "VirtualAnalogWrapper::perform_first_check() successfully completed";
    first_check = true;
    return true;
}

void VirtualAnalogWrapper::run()
{
    yarp::os::Bottle *pTorques;
    bool sendLastValueBeforeTimeout = false;
    while (!Thread::isStopping())
    {
        pTorques=mPortInputTorques.read(false);
        double timeNow=Time::now();

        if (pTorques)
        {
            sendLastValueBeforeTimeout = false;
            mMutex.wait();

            lastRecv=Time::now();
            switch (pTorques->get(0).asInt())
            {
                case 1: //arm torque message
                    if (perform_first_check(6)==false) break;
                    mSubdevices[mChan2Board[0]].setTorque(mChan2BAddr[0],pTorques->get(1).asDouble()); //shoulder 1 pitch      (0)
                    mSubdevices[mChan2Board[1]].setTorque(mChan2BAddr[1],pTorques->get(2).asDouble()); //shoulder 2 roll       (1)
                    mSubdevices[mChan2Board[2]].setTorque(mChan2BAddr[2],pTorques->get(3).asDouble()); //shoulder 3 yaw        (2)
                    mSubdevices[mChan2Board[3]].setTorque(mChan2BAddr[3],pTorques->get(4).asDouble()); //elbow                 (3)
                    mSubdevices[mChan2Board[4]].setTorque(mChan2BAddr[4],pTorques->get(5).asDouble()); //wrist pronosupination (4)
                    mSubdevices[mChan2Board[5]].setTorque(mChan2BAddr[5],0.0);
                break;

                case 2: //legs torque message
                    if (perform_first_check(6)==false) break;
                    mSubdevices[mChan2Board[0]].setTorque(mChan2BAddr[0],pTorques->get(1).asDouble()); //hip pitch
                    mSubdevices[mChan2Board[1]].setTorque(mChan2BAddr[1],pTorques->get(2).asDouble()); //hip roll
                    mSubdevices[mChan2Board[2]].setTorque(mChan2BAddr[2],pTorques->get(3).asDouble()); //hip yaw
                    mSubdevices[mChan2Board[3]].setTorque(mChan2BAddr[3],pTorques->get(4).asDouble()); //knee
                    mSubdevices[mChan2Board[4]].setTorque(mChan2BAddr[4],pTorques->get(5).asDouble()); //ankle pitch
                    mSubdevices[mChan2Board[5]].setTorque(mChan2BAddr[5],pTorques->get(6).asDouble()); //ankle roll
                break;

                case 3: //wrist torque message
                    if (perform_first_check(6)==false) break;
                    mSubdevices[mChan2Board[0]].setTorque(mChan2BAddr[0],pTorques->get(6).asDouble()); //wrist yaw   (6)
                    mSubdevices[mChan2Board[1]].setTorque(mChan2BAddr[1],pTorques->get(7).asDouble()); //wrist pitch (7)
                    mSubdevices[mChan2Board[2]].setTorque(mChan2BAddr[2],0.0);
                    mSubdevices[mChan2Board[3]].setTorque(mChan2BAddr[3],0.0);
                    mSubdevices[mChan2Board[4]].setTorque(mChan2BAddr[4],0.0);
                    mSubdevices[mChan2Board[5]].setTorque(mChan2BAddr[5],0.0);
                break;

                case 4: // torso
                    if (perform_first_check(3)==false) break;
                    mSubdevices[mChan2Board[0]].setTorque(mChan2BAddr[0],pTorques->get(1).asDouble()); //torso yaw (respect gravity)
                    mSubdevices[mChan2Board[1]].setTorque(mChan2BAddr[1],pTorques->get(2).asDouble()); //torso roll (lateral movement)
                    mSubdevices[mChan2Board[2]].setTorque(mChan2BAddr[2],pTorques->get(3).asDouble()); //torso pitch (front-back movement)
//                    mSubdevices[mChan2Board[3]].setTorque(mChan2BAddr[3],0.0);
//                    mSubdevices[mChan2Board[4]].setTorque(mChan2BAddr[4],0.0);
//                    mSubdevices[mChan2Board[5]].setTorque(mChan2BAddr[5],0.0);
                break;

                default:
                    yError() << "VirtualAnalogWrapper: got unexpected " << pTorques->get(0).asInt() << " message on virtualAnalogServer.";
            }

            for (int d=0; d<mNSubdevs; ++d)
            {
                mSubdevices[d].flushTorques();
            }

            mMutex.post();
        }
        else
        {
            // sending rate from wholeBody is 10ms, if nothing is got now, wait that much time
            yarp::os::Time::delay(0.001);
        }

        if ((lastRecv+0.050 < timeNow) && (!sendLastValueBeforeTimeout))
        {
            /* If 50ms have passed since the last received message, reset values.
             * Sending time will be 1ms due to the delay above (else case).
             */
            for (int d=0; d<mNSubdevs; ++d)
            {
                mSubdevices[d].resetTorque();
                mSubdevices[d].flushTorques();

//                Virtual Sensor status is not handled now because server DO NOT implement IVirtual AnalogSensor Interface.
//                status=IAnalogSensor::AS_TIMEOUT;
            }
            sendLastValueBeforeTimeout = true;
        }
    }
}

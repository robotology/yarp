/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "VirtualAnalogWrapper.h"
// #include <iostream>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;


namespace {
YARP_LOG_COMPONENT(VIRTUALANALOGSERVER, "yarp.device.virtualAnalogServer")
constexpr int MAX_ENTRIES = 255;
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
        yCError(VIRTUALANALOGSERVER) << "Check configuration file top<base.";
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
        yCError(VIRTUALANALOGSERVER) << "Wrong device" << key.c_str();
        return false;
    }

    //configure first
    if (!mIsConfigured)
    {
        yCError(VIRTUALANALOGSERVER) << "'configure' should be called before you can attach any device";
        return false;
    }

    if (!device)
    {
        yCError(VIRTUALANALOGSERVER) << "Invalid device (null pointer)";
        return false;
    }

    mpDevice=device;

    if (mpDevice->isValid())
    {
        mpDevice->view(mpSensor);
    }
    else
    {
        yCError(VIRTUALANALOGSERVER) << "Invalid device " << key << " (isValid() returned false)";
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

    mpDevice=nullptr;
    mpSensor=nullptr;

    mIsConfigured=false;
    mIsAttached=false;
}

bool VirtualAnalogWrapper::open(Searchable& config)
{
    yCDebug(VIRTUALANALOGSERVER) << config.toString().c_str();

    mIsVerbose = (config.check("verbose","if present, give detailed output"));

    if (mIsVerbose) {
        yCDebug(VIRTUALANALOGSERVER) << "Running with verbose output\n";
    }

    //thus thread period is useful for output port... this input port has callback so maybe can skip it (?)
    //thread_period = prop.check("threadrate", 20, "thread rate in ms. for streaming encoder data").asInt32();

    yCDebug(VIRTUALANALOGSERVER) << "Using VirtualAnalogServer\n";

    if (!config.check("networks", "list of networks merged by this wrapper"))
    {
        yCError(VIRTUALANALOGSERVER) << "Missing networks parameters";
        return false;
    }

    Bottle *networks=config.find("networks").asList();
    mNSubdevs=networks->size();
    mSubdevices.resize(mNSubdevs);

    mChan2Board.resize(MAX_ENTRIES);
    mChan2BAddr.resize(MAX_ENTRIES);
    for (int i = 0; i < MAX_ENTRIES; i++)
    {
        mChan2Board[i]=-1;
        mChan2BAddr[i]=-1;
    }

    int totalJ=0;

    for (size_t k=0; k<networks->size(); ++k)
    {
        auto parameters = config.findGroup(networks->get(k).asString());
        int map0, map1, map2, map3;

        if (parameters.size() == 2)
        {
            auto* bot = parameters.get(1).asList();
            Bottle tmpBot;
            if (bot == nullptr)
            {
                // try to read data as a string in the last resort
                tmpBot.fromString(parameters.get(1).asString());
                if (tmpBot.size() != 4)
                {
                    yCError(VIRTUALANALOGSERVER) << "Error: check network parameters in part description"
                                                 << "--> I was expecting" << networks->get(k).asString() << "followed by four integers between parentheses"
                                                 << "Got: " << parameters.toString();
                    return false;
                }

                bot = &tmpBot;
            }

            map0 = bot->get(0).asInt32();
            map1 = bot->get(1).asInt32();
            map2 = bot->get(2).asInt32();
            map3 = bot->get(3).asInt32();
        }
        else if (parameters.size() == 5)
        {
            yCError(VIRTUALANALOGSERVER) << "Parameter networks use deprecated syntax";
            map0 = parameters.get(1).asInt32();
            map1 = parameters.get(2).asInt32();
            map2 = parameters.get(3).asInt32();
            map3 = parameters.get(4).asInt32();
        }
        else
        {
            yCError(VIRTUALANALOGSERVER) << "Error: check network parameters in part description"
                                         << "--> I was expecting" << networks->get(k).asString() << "followed by four integers between parentheses"
                                         << "Got: " << parameters.toString();
            return false;
        }

        if (map0 >= MAX_ENTRIES || map1 >= MAX_ENTRIES || map2>= MAX_ENTRIES || map3>= MAX_ENTRIES ||
            map0 <0             || map1 <0             || map2<0             || map3<0)
        {
            yCError(VIRTUALANALOGSERVER) << "Invalid map entries in networks section, failed initial check";
            return false;
        }

        for (int j=map0; j<=map1; ++j)
        {
            mChan2Board[j]=k;
            mChan2BAddr[j]=j-map0+map2;
        }

        if (!mSubdevices[k].configure(map2,map3,networks->get(k).asString()))
        {
            yCError(VIRTUALANALOGSERVER) << "Configure of subdevice ret false";
            return false;
        }

        totalJ+=map1-map0+1;
    }

    // Verify minimum set of parameters required
    if(!config.check("robotName") )   // ?? qui dentro, da dove lo pesco ??
    {
        yCError(VIRTUALANALOGSERVER) << "Missing robotName, check your configuration file!";
        return false;
    }

    if (config.check("deviceId"))
    {
        yCError(VIRTUALANALOGSERVER) << "The parameter 'deviceId' has been deprecated, please use parameter 'name' instead. \n"
                 << "e.g. In the VFT wrapper configuration files of your robot, replace '<param name=""deviceId""> left_arm </param>' \n"
                 << "with '/icub/joint_vsens/left_arm:i' ";
        return false;
    }

    std::string port_name = config.check("name",Value("controlboard"),"Virtual analog wrapper port name, e.g. /icub/joint_vsens/left_arm:i").asString();
    std::string robot_name = config.find("robotName").asString();

    if (!mPortInputTorques.open(port_name))
    {
        yCError(VIRTUALANALOGSERVER) << "Can't open port " << port_name.c_str();
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
    mMutex.lock();

    for (int p=0; p<polylist.size(); ++p)
    {
        std::string key=polylist[p]->key;

        // find appropriate entry in list of subdevices and attach
        for (auto& mSubdevice : mSubdevices)
        {
            if (mSubdevice.getKey() == key)
            {
                if (!mSubdevice.attach(polylist[p]->poly,key))
                {
                    mMutex.unlock();
                    return false;
                }
            }
        }
    }

    //check if all devices are attached to the driver
    for (auto& mSubdevice : mSubdevices)
    {
        if (!mSubdevice.isAttached())
        {
            mMutex.unlock();
            return false;
        }
   }

    mMutex.unlock();

    Thread::start();

    return true;
}

bool VirtualAnalogWrapper::detachAll()
{
    mMutex.lock();

    for(int k=0; k<mNSubdevs; ++k)
    {
        mSubdevices[k].detach();
    }

    mMutex.unlock();

//     close();

    return true;
}

bool VirtualAnalogWrapper::perform_first_check(int elems)
{
    if (first_check) {
        return true;
    }

    for (int i=0; i<elems; i++)
    {
        if (mChan2Board[i]==-1 || mChan2BAddr[i]==-1)
        {
            yCError(VIRTUALANALOGSERVER) << "Invalid map entries in networks section, failed runtime check"
                     << " i: " << i << "mChan2Board[i] is " << mChan2Board[i] << " chan2add is " << mChan2BAddr[i];
            return false;
        }
    }

    yCTrace(VIRTUALANALOGSERVER) << "perform_first_check() successfully completed";
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
            mMutex.lock();

            lastRecv=Time::now();
            switch (pTorques->get(0).asInt32())
            {
                case 1: //arm torque message
                    if (perform_first_check(6) == false) {
                        break;
                    }
                    mSubdevices[mChan2Board[0]].setTorque(mChan2BAddr[0],pTorques->get(1).asFloat64()); //shoulder 1 pitch      (0)
                    mSubdevices[mChan2Board[1]].setTorque(mChan2BAddr[1],pTorques->get(2).asFloat64()); //shoulder 2 roll       (1)
                    mSubdevices[mChan2Board[2]].setTorque(mChan2BAddr[2],pTorques->get(3).asFloat64()); //shoulder 3 yaw        (2)
                    mSubdevices[mChan2Board[3]].setTorque(mChan2BAddr[3],pTorques->get(4).asFloat64()); //elbow                 (3)
                    mSubdevices[mChan2Board[4]].setTorque(mChan2BAddr[4],pTorques->get(5).asFloat64()); //wrist pronosupination (4)
                    mSubdevices[mChan2Board[5]].setTorque(mChan2BAddr[5],0.0);
                break;

                case 2: //legs torque message
                    if (perform_first_check(6) == false) {
                        break;
                    }
                    mSubdevices[mChan2Board[0]].setTorque(mChan2BAddr[0],pTorques->get(1).asFloat64()); //hip pitch
                    mSubdevices[mChan2Board[1]].setTorque(mChan2BAddr[1],pTorques->get(2).asFloat64()); //hip roll
                    mSubdevices[mChan2Board[2]].setTorque(mChan2BAddr[2],pTorques->get(3).asFloat64()); //hip yaw
                    mSubdevices[mChan2Board[3]].setTorque(mChan2BAddr[3],pTorques->get(4).asFloat64()); //knee
                    mSubdevices[mChan2Board[4]].setTorque(mChan2BAddr[4],pTorques->get(5).asFloat64()); //ankle pitch
                    mSubdevices[mChan2Board[5]].setTorque(mChan2BAddr[5],pTorques->get(6).asFloat64()); //ankle roll
                break;

                case 3: //wrist torque message
                    if (perform_first_check(6) == false) {
                        break;
                    }
                    mSubdevices[mChan2Board[0]].setTorque(mChan2BAddr[0],pTorques->get(6).asFloat64()); //wrist yaw   (6)
                    mSubdevices[mChan2Board[1]].setTorque(mChan2BAddr[1],pTorques->get(7).asFloat64()); //wrist pitch (7)
                    mSubdevices[mChan2Board[2]].setTorque(mChan2BAddr[2],0.0);
                    mSubdevices[mChan2Board[3]].setTorque(mChan2BAddr[3],0.0);
                    mSubdevices[mChan2Board[4]].setTorque(mChan2BAddr[4],0.0);
                    mSubdevices[mChan2Board[5]].setTorque(mChan2BAddr[5],0.0);
                break;

                case 4: // torso
                    if (perform_first_check(3) == false) {
                        break;
                    }
                    mSubdevices[mChan2Board[0]].setTorque(mChan2BAddr[0],pTorques->get(1).asFloat64()); //torso yaw (respect gravity)
                    mSubdevices[mChan2Board[1]].setTorque(mChan2BAddr[1],pTorques->get(2).asFloat64()); //torso roll (lateral movement)
                    mSubdevices[mChan2Board[2]].setTorque(mChan2BAddr[2],pTorques->get(3).asFloat64()); //torso pitch (front-back movement)
//                    mSubdevices[mChan2Board[3]].setTorque(mChan2BAddr[3],0.0);
//                    mSubdevices[mChan2Board[4]].setTorque(mChan2BAddr[4],0.0);
//                    mSubdevices[mChan2Board[5]].setTorque(mChan2BAddr[5],0.0);
                break;

                default:
                    yCError(VIRTUALANALOGSERVER) << "Got unexpected " << pTorques->get(0).asInt32() << " message on virtualAnalogServer.";
            }

            for (int d=0; d<mNSubdevs; ++d)
            {
                mSubdevices[d].flushTorques();
            }

            mMutex.unlock();
        }
        else
        {
            // sending rate from wholeBody is 10ms, if nothing is got now, wait that much time
            yarp::os::Time::delay(0.001);
        }

        if(first_check)
        {
            if ((lastRecv+0.080 < timeNow) && (!sendLastValueBeforeTimeout))
            {
               /* If 80ms have passed since the last received message, reset values to zero (just once).
                * Sending time will be 1ms due to the delay above (else case).
                */
                for (int d=0; d<mNSubdevs; ++d)
                {
                    mSubdevices[d].resetTorque();
                    mSubdevices[d].flushTorques();

                }
    //          Virtual Sensor status is not handled now because server DO NOT implement IVirtual AnalogSensor Interface.
    //          status=IAnalogSensor::AS_TIMEOUT;
                yCError(VIRTUALANALOGSERVER) << "Timeout!! No new value received for more than " << timeNow - lastRecv  << " secs.";
                sendLastValueBeforeTimeout = true;
            }
        }
    }
}

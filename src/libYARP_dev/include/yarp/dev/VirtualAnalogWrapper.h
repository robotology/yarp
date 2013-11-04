// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef __VIRTUALANALOGSERVER__
#define __VIRTUALANALOGSERVER__

/*
* Copyright (C) 2013 RobotCub Consortium
* Author: Alberto Cardellino
* CopyPolicy: Released under the terms of the GNU GPL v2.0.
*
*/

// VirtualAnalogWrapper
// A server that opens an input port getting externally measured analog values
// and it is able to attach to a one or more virtual analog sensor through
// IVirtualAnalogSensor interface.

#include <yarp/os/PortablePair.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/RateThread.h>
#include <yarp/os/Vocab.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Semaphore.h>
#include <yarp/dev/Wrapper.h>

#include <yarp/dev/IVirtualAnalogSensor.h>

#include <string>
#include <vector>

#include <stdarg.h>

#ifdef MSVC
    #pragma warning(disable:4355)
#endif

/* Using custom namespace to avoid name conflict while linking since helper
 * class for the AnalogWrapper could been used also for other devices in other
 * part of the code.
 * Better would be use internal linkage, through unnamed namespace therefore
 * enforcing internal linkage, but compiler correctly complains issuing a
 * warning message, because th eclass is used as a member of another class
 * outside the anonymous namespace.
 * This is not a problem actually, but seeing a lot of warning messages is annoying.
 */

namespace virtualAnalogWrapper_yarp_internal_namespace
{
    class AnalogSubDevice;
}

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace virtualAnalogWrapper_yarp_internal_namespace;

#ifndef DOXYGEN_SHOULD_SKIP_THIS

class VirtualAnalogWrapper;

namespace virtualAnalogWrapper_yarp_internal_namespace {
class AnalogSubDevice
{
public:
    AnalogSubDevice();
   ~AnalogSubDevice();

    bool attach(PolyDriver *driver, const std::string &key);
    void detach();

    bool configure(int map0, int map1, const std::string &key);
   
    bool isAttached(){ return mIsAttached; }

    void setTorque(int joint,double torque)
    {
        if (joint<mMap0 || mMap1<joint) return;

        mTorques[joint-mMap0]=torque;
    }

    void resetTorque()
    {
        mTorques.zero();
    }

    void flushTorques()
    {
        if (mpSensor) mpSensor->updateMeasure(mTorques);
    }

    const std::string& getKey(){ return mKey; }

protected:
    std::string mKey;
    
    int mMap0,mMap1; 

    yarp::sig::Vector mTorques;

    bool mIsConfigured;
    bool mIsAttached;
    double lastRecvMsg;
    PolyDriver            *mpDevice;
    IVirtualAnalogSensor  *mpSensor;
};

} // closing namespace virtualAnalogWrapper_yarp_internal_namespace

///////////////////////////////////////////////////
// TODO add IVirtualAnalogSensor interface to have Channels number and status??
class VirtualAnalogWrapper : public DeviceDriver, public Thread, public IMultipleWrapper
{
public:
    VirtualAnalogWrapper() : mMutex(1)
    {
        lastRecv = 0;
        mIsVerbose=false;
        mNSubdevs=0;
        first_check = false;
    }

    ~VirtualAnalogWrapper()
    {
        close();
    }

    // DeviceDriver //////////////////////////////////////////////////////////
    virtual bool open(Searchable& config);
    virtual bool close();
    //////////////////////////////////////////////////////////////////////////

    // Thread ////////////////////////////////////////////////////////////////
    virtual void run();    
    //////////////////////////////////////////////////////////////////////////

    // IMultipleWrapper //////////////////////////////////////////////////////
    virtual bool attachAll(const yarp::dev::PolyDriverList &p);
    virtual bool detachAll();
    //////////////////////////////////////////////////////////////////////////

    // Utility
    bool perform_first_check(int elems);

protected:

    yarp::os::Semaphore mMutex;

    bool mIsVerbose;

    int mNSubdevs;

    std::vector<int> mChan2Board;
    std::vector<int> mChan2BAddr;
    double lastRecv;
    bool first_check;

    std::vector<AnalogSubDevice> mSubdevices;
    yarp::os::BufferedPort<yarp::os::Bottle> mPortInputTorques;
};

#endif // DOXYGEN_SHOULD_SKIP_THIS

#endif

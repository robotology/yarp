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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
                        TODO make more general
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* Using yarp::dev::impl namespace for all helper class inside yarp::dev to reduce
 * name conflicts
 */

namespace yarp{
    namespace dev{
        class VirtualAnalogWrapper;
        namespace impl {
            class AnalogSubDevice;
        }
    }
}

///////////////////////////////////////////////////
// TODO add IVirtualAnalogSensor interface to have Channels number and status??

/**
 *  @ingroup dev_impl_wrapper
 *
 * An analog wrapper for virtual device
 * A virtual device is a software emulated device, for example force-torque computed from a real sensor
 * and then relocated to another part of the robot or some kind of estimated meassurement needed by the
 * robot.
 *
 * This virtual wrapper will open a port and accept the incoming estimated measurement and send them to
 * the real robot using the attached device.
 */
class yarp::dev::VirtualAnalogWrapper : public yarp::dev::DeviceDriver, public yarp::os::Thread, public yarp::dev::IMultipleWrapper
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
    virtual bool open(yarp::os::Searchable& config);
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

#ifndef DOXYGEN_SHOULD_SKIP_THIS
protected:
    yarp::os::Semaphore mMutex;

    bool mIsVerbose;

    int mNSubdevs;

    std::vector<int> mChan2Board;
    std::vector<int> mChan2BAddr;
    double lastRecv;
    bool first_check;

    std::vector<yarp::dev::impl::AnalogSubDevice> mSubdevices;
    yarp::os::BufferedPort<yarp::os::Bottle> mPortInputTorques;
#endif // DOXYGEN_SHOULD_SKIP_THIS
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS

class yarp::dev::impl::AnalogSubDevice
{
public:
    AnalogSubDevice();
   ~AnalogSubDevice();

    bool attach(yarp::dev::PolyDriver *driver, const std::string &key);
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
    yarp::dev::PolyDriver            *mpDevice;
    yarp::dev::IVirtualAnalogSensor  *mpSensor;
};

#endif // DOXYGEN_SHOULD_SKIP_THIS

#endif

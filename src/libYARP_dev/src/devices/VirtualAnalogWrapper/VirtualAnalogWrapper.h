/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_VIRTUALANALOGWRAPPER_VIRTUALANALOGWRAPPER_H
#define YARP_DEV_VIRTUALANALOGWRAPPER_VIRTUALANALOGWRAPPER_H


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
#include <cstdarg>

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
 * and then relocated to another part of the robot or some kind of estimated measurement needed by the
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
    virtual bool open(yarp::os::Searchable& config) override;
    virtual bool close() override;
    //////////////////////////////////////////////////////////////////////////

    // Thread ////////////////////////////////////////////////////////////////
    virtual void run() override;
    //////////////////////////////////////////////////////////////////////////

    // IMultipleWrapper //////////////////////////////////////////////////////
    virtual bool attachAll(const yarp::dev::PolyDriverList &p) override;
    virtual bool detachAll() override;
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
        if (mpSensor) mpSensor->updateVirtualAnalogSensorMeasure(mTorques);
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

#endif // YARP_DEV_VIRTUALANALOGWRAPPER_VIRTUALANALOGWRAPPER_H

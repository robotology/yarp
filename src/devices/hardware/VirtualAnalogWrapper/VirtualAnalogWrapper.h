/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Vocab.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/IMultipleWrapper.h>

#include <yarp/dev/IVirtualAnalogSensor.h>

#include <mutex>
#include <string>
#include <vector>
#include <cstdarg>

#ifdef MSVC
    #pragma warning(disable:4355)
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
                        TODO make more general
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


///////////////////////////////////////////////////
// TODO add IVirtualAnalogSensor interface to have Channels number and status??


class AnalogSubDevice;

/**
 *  @ingroup dev_impl_wrapper
 *
 * \brief `virtualAnalogServer`: An analog wrapper for virtual device
 * A virtual device is a software emulated device, for example force-torque computed from a real sensor
 * and then relocated to another part of the robot or some kind of estimated measurement needed by the
 * robot.
 *
 * This virtual wrapper will open a port and accept the incoming estimated measurement and send them to
 * the real robot using the attached device.
 */
class VirtualAnalogWrapper :
        public yarp::dev::DeviceDriver,
        public yarp::os::Thread,
        public yarp::dev::IMultipleWrapper
{
public:
    VirtualAnalogWrapper() = default;
    VirtualAnalogWrapper(const VirtualAnalogWrapper&) = delete;
    VirtualAnalogWrapper(VirtualAnalogWrapper&&) = delete;
    VirtualAnalogWrapper& operator=(const VirtualAnalogWrapper&) = delete;
    VirtualAnalogWrapper& operator=(VirtualAnalogWrapper&&) = delete;

    ~VirtualAnalogWrapper() override
    {
        close();
    }

    // DeviceDriver //////////////////////////////////////////////////////////
    bool open(yarp::os::Searchable& config) override;
    bool close() override;
    //////////////////////////////////////////////////////////////////////////

    // Thread ////////////////////////////////////////////////////////////////
    void run() override;
    //////////////////////////////////////////////////////////////////////////

    // IMultipleWrapper //////////////////////////////////////////////////////
    bool attachAll(const yarp::dev::PolyDriverList &p) override;
    bool detachAll() override;
    //////////////////////////////////////////////////////////////////////////

    // Utility
    bool perform_first_check(int elems);

protected:
    std::mutex mMutex;

    bool mIsVerbose{false};

    int mNSubdevs{0};

    std::vector<int> mChan2Board;
    std::vector<int> mChan2BAddr;
    double lastRecv{0.0};
    bool first_check{false};

    std::vector<AnalogSubDevice> mSubdevices;
    yarp::os::BufferedPort<yarp::os::Bottle> mPortInputTorques;
};


class AnalogSubDevice
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
        if (joint < mMap0 || mMap1 < joint) {
            return;
        }

        mTorques[joint-mMap0]=torque;
    }

    void resetTorque()
    {
        mTorques.zero();
    }

    void flushTorques()
    {
        if (mpSensor) {
            mpSensor->updateVirtualAnalogSensorMeasure(mTorques);
        }
    }

    const std::string& getKey(){ return mKey; }

protected:
    std::string mKey;

    int mMap0,mMap1;

    yarp::sig::Vector mTorques;

    bool mIsConfigured;
    bool mIsAttached;
    double lastRecvMsg{0.0};
    yarp::dev::PolyDriver            *mpDevice;
    yarp::dev::IVirtualAnalogSensor  *mpSensor;
};


#endif // YARP_DEV_VIRTUALANALOGWRAPPER_VIRTUALANALOGWRAPPER_H

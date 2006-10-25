// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Giorgio Metta and Lorenzo Natale
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef __INERTIALCUBE2__
#define __INERTIALCUBE2__

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/GenericSensorInterfaces.h>

namespace yarp{
    namespace dev{
        class InertiaCube2;
    }
}

struct InertiaCube2Parameters
{
    short comPort;
};

/**
 *
 * @ingroup dev_impl
 *
 * InterSense inertia cube inertial sensor.
 */
class yarp::dev::InertiaCube2 : public IGenericSensor, public DeviceDriver
{
public:
    InertiaCube2();
    ~InertiaCube2();
    
    // IGenericSensor interface.
    virtual bool read(yarp::sig::Vector &out);
    virtual bool getChannels(int *nc);
    virtual  bool open(yarp::os::Searchable &config);
    virtual bool calibrate(int ch, double v);
    virtual bool close();

    // Open the device
    bool open(const InertiaCube2Parameters &par);

private:
    bool start();
    bool stop();

    void *system_resources;
    int nchannels;
};

#endif
//

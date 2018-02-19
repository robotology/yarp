/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <string>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/os/RateThread.h>
#include <yarp/dev/GenericSensorInterfaces.h>
#include <yarp/os/Stamp.h>
#include <yarp/dev/PreciselyTimed.h>
#include <yarp/math/Math.h>

namespace yarp{
    namespace dev{
        class fakeIMU;
    }
}

#define DEFAULT_PERIOD 10   //ms

class yarp::dev::fakeIMU :  public DeviceDriver,
                            public IGenericSensor,
                            public yarp::os::RateThread,
                            public yarp::dev::IPreciselyTimed
{
public:
    fakeIMU();
    ~fakeIMU();

    // Device Driver interface
    virtual bool open(yarp::os::Searchable &config) override;
    virtual bool close() override;

    // IGenericSensor interface.
    virtual bool read(yarp::sig::Vector &out) override;
    virtual bool getChannels(int *nc) override;
    virtual bool calibrate(int ch, double v) override;

    // IPreciselyTimed interface
    virtual yarp::os::Stamp getLastInputStamp() override;

    yarp::sig::Vector rpy, gravity;
    yarp::sig::Matrix dcm;
    yarp::sig::Vector accels;

private:

    bool threadInit() override;
    void run() override;
    unsigned int nchannels;
    double dummy_value;
    yarp::os::Stamp lastStamp;
};

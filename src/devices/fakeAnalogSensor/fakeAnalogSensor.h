/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEVICE_FAKE_ANALOGSENSOR
#define YARP_DEVICE_FAKE_ANALOGSENSOR

#include <yarp/os/RateThread.h>
#include <yarp/os/Semaphore.h>

#include <yarp/dev/all.h>
#include <yarp/dev/IAnalogSensor.h>

namespace yarp{
    namespace dev{
        class FakeAnalogSensor;
    }
}

/**
*
*
* Fake analog sensor device for testing purpose and reference for new analog devices
*
* Parameters accepted in the config argument of the open method:
* | Parameter name | Type   | Units | Default Value | Required | Description | Notes |
* |:--------------:|:------:|:-----:|:-------------:|:--------:|:-----------:|:-----:|
* |
*/

class yarp::dev::FakeAnalogSensor : public yarp::dev::DeviceDriver,
                                    public yarp::os::RateThread,
                                    public yarp::dev::IAnalogSensor
{
private:

    yarp::os::Semaphore     mutex;

    yarp::os::ConstString   name;    // device name
    unsigned int            channelsNum;
    short                   status;
    double                  timeStamp;
    yarp::sig::Vector       data;

public:
    FakeAnalogSensor(int period = 20);

    ~FakeAnalogSensor();

    virtual bool open(yarp::os::Searchable& config) override;
    virtual bool close() override;

    //IAnalogSensor interface
    virtual int getChannels() override;
    virtual int getState(int ch) override;
    virtual int read(yarp::sig::Vector &out) override;

    virtual int calibrateSensor() override;
    virtual int calibrateSensor(const yarp::sig::Vector& v) override;

    virtual int calibrateChannel(int ch) override;
    virtual int calibrateChannel(int ch, double v) override;

    // RateThread interface
    virtual void run() override;
    virtual bool threadInit() override;
    virtual void threadRelease() override;
};


#endif  // YARP_DEVICE_FAKE_ANALOGSENSOR

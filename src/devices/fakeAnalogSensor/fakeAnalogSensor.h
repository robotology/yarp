/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEVICE_FAKE_ANALOGSENSOR
#define YARP_DEVICE_FAKE_ANALOGSENSOR

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Mutex.h>

#include <yarp/dev/all.h>
#include <yarp/dev/IAnalogSensor.h>

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

class FakeAnalogSensor :
        public yarp::dev::DeviceDriver,
        public yarp::os::PeriodicThread,
        public yarp::dev::IAnalogSensor
{
private:

    yarp::os::Mutex         mutex;

    std::string   name;    // device name
    unsigned int            channelsNum;
    short                   status;
    double                  timeStamp;
    yarp::sig::Vector       data;

public:
    FakeAnalogSensor(double period = 0.02);

    ~FakeAnalogSensor();

    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    //IAnalogSensor interface
    int getChannels() override;
    int getState(int ch) override;
    int read(yarp::sig::Vector &out) override;

    int calibrateSensor() override;
    int calibrateSensor(const yarp::sig::Vector& v) override;

    int calibrateChannel(int ch) override;
    int calibrateChannel(int ch, double v) override;

    // RateThread interface
    void run() override;
    bool threadInit() override;
    void threadRelease() override;
};


#endif  // YARP_DEVICE_FAKE_ANALOGSENSOR

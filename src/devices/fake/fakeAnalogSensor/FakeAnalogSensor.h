/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEVICE_FAKE_ANALOGSENSOR
#define YARP_DEVICE_FAKE_ANALOGSENSOR

#include <yarp/os/PeriodicThread.h>

#include <yarp/dev/all.h>
#include <yarp/dev/IAnalogSensor.h>

#include <mutex>
#include "FakeAnalogSensor_ParamsParser.h"

/**
*
* @ingroup dev_impl_fake dev_impl_media dev_impl_analog_sensors
*
* \brief `fakeAnalogSensor`: Fake analog sensor device for testing purpose and reference for new analog devices
*
* Parameters required by this device are shown in class: FakeAnalogSensor_ParamsParser
*/

class FakeAnalogSensor :
        public yarp::dev::DeviceDriver,
        public yarp::os::PeriodicThread,
        public yarp::dev::IAnalogSensor,
        public FakeAnalogSensor_ParamsParser
{
private:

    std::mutex         mutex;

    std::string   name;    // device name
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

// Copyright (C) 2016 iCub Facility, Istituto Italiano di Tecnologia
// Authors: Alberto Cardellino
// email:   alberto.cardellino@iit.it
// CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

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

    virtual bool open(yarp::os::Searchable& config);
    virtual bool close();

    //IAnalogSensor interface
    virtual int getChannels();
    virtual int getState(int ch);
    virtual int read(yarp::sig::Vector &out);

    virtual int calibrateSensor();
    virtual int calibrateSensor(const yarp::sig::Vector& v);

    virtual int calibrateChannel(int ch);
    virtual int calibrateChannel(int ch, double v);

    // RateThread interface
    virtual void run();
    virtual bool threadInit();
    virtual void threadRelease();
};


#endif  // YARP_DEVICE_FAKE_ANALOGSENSOR

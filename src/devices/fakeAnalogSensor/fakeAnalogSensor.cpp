// Copyright (C) 2016 iCub Facility, Istituto Italiano di Tecnologia
// Authors: Alberto Cardellino
// email:   alberto.cardellino@iit.it
// CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


#include <yarp/os/Time.h>
#include <yarp/os/LogStream.h>

#include <fakeAnalogSensor.h>

using namespace std;
using namespace yarp::dev;

FakeAnalogSensor::FakeAnalogSensor(int period) : RateThread(period),
                                                 mutex(1),
                                                 status(IAnalogSensor::AS_OK)
{
    yTrace();
    timeStamp = yarp::os::Time::now();
};

FakeAnalogSensor::~FakeAnalogSensor()
{
    yTrace();
}


bool FakeAnalogSensor::open(yarp::os::Searchable& config)
{
    yTrace();
    bool correct=true;

    //debug
    fprintf(stderr, "%s\n", config.toString().c_str());

    // Check parameters first
//     if(!config.check("channels"))
//     {
//         correct = false;
//         yError() << "Parameter 'channels' missing";
//     }

    if(!config.check("period"))
    {
        correct = false;
        yError() << "Parameter 'period' missing";
    }

    if (!correct)
    {
        yError() << "Insufficient parameters to FakeAnalogSensor\n";
        return false;
    }

    int period=config.find("period").asInt();
    setRate(period);

    //create the data vector:
    this->channelsNum = 1;
    data.resize(channelsNum);
    data.zero();

    RateThread::start();
    return true;
}

bool FakeAnalogSensor::close()
{
    yTrace();
    //stop the thread
    RateThread::stop();

    return true;
}

int FakeAnalogSensor::read(yarp::sig::Vector &out)
{
    mutex.wait();
    out[0] = yarp::os::Time::now();
    mutex.post();

    return status;
}

int FakeAnalogSensor::getState(int ch)
{
    yTrace();
    // Always ok for now
    return status;
}

int FakeAnalogSensor::getChannels()
{
    yTrace();
    return channelsNum;
}

int yarp::dev::FakeAnalogSensor::calibrateSensor()
{
    yTrace();
    //NOT YET IMPLEMENTED
    return 0;
}

int FakeAnalogSensor::calibrateChannel(int ch, double v)
{
    yTrace();
    //NOT YET IMPLEMENTED
    return 0;
}

int FakeAnalogSensor::calibrateSensor(const yarp::sig::Vector& v)
{
    yTrace();
    //NOT YET IMPLEMENTED
    return 0;
}

int FakeAnalogSensor::calibrateChannel(int ch)
{
    yTrace();
    //NOT YET IMPLEMENTED
    return 0;
}

bool FakeAnalogSensor::threadInit()
{
    yTrace();
    return true;
}

void FakeAnalogSensor::run()
{
    mutex.wait();

    // Do fake stuff
    double timeNow = yarp::os::Time::now();

    //if 100ms have passed since the last received message
    if (timeNow > timeStamp+10)
        status = IAnalogSensor::AS_TIMEOUT;
    else
        status = IAnalogSensor::AS_OK;

    timeStamp = timeNow;
    mutex.post();
}

void FakeAnalogSensor::threadRelease()
{
    yTrace();
}


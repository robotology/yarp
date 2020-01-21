/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "fakeAnalogSensor.h"

#include <yarp/os/Time.h>
#include <yarp/os/LogStream.h>

using namespace std;
using namespace yarp::dev;

FakeAnalogSensor::FakeAnalogSensor(double period) : PeriodicThread(period),
        mutex(),
        channelsNum(0),
        status(IAnalogSensor::AS_OK)
{
    yTrace();
    timeStamp = yarp::os::Time::now();
}

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

    double period=config.find("period").asInt32() / 1000.0;
    setPeriod(period);

    //create the data vector:
    this->channelsNum = 1;
    data.resize(channelsNum);
    data.zero();

    return PeriodicThread::start();
}

bool FakeAnalogSensor::close()
{
    yTrace();
    //stop the thread
    PeriodicThread::stop();

    return true;
}

int FakeAnalogSensor::read(yarp::sig::Vector &out)
{
    mutex.lock();
    out[0] = yarp::os::Time::now();
    mutex.unlock();

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

int FakeAnalogSensor::calibrateSensor()
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
    mutex.lock();

    // Do fake stuff
    double timeNow = yarp::os::Time::now();

    //if 100ms have passed since the last received message
    if (timeNow > timeStamp+10)
        status = IAnalogSensor::AS_TIMEOUT;
    else
        status = IAnalogSensor::AS_OK;

    timeStamp = timeNow;
    mutex.unlock();
}

void FakeAnalogSensor::threadRelease()
{
    yTrace();
}

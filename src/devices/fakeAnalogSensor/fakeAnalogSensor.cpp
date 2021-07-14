/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fakeAnalogSensor.h"

#include <yarp/os/Time.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

using namespace std;
using namespace yarp::dev;

namespace {
YARP_LOG_COMPONENT(FAKEANALOGSENSOR, "yarp.device.fakeAnalogSensor")
}

FakeAnalogSensor::FakeAnalogSensor(double period) : PeriodicThread(period),
        mutex(),
        channelsNum(0),
        status(IAnalogSensor::AS_OK)
{
    yCTrace(FAKEANALOGSENSOR);
    timeStamp = yarp::os::Time::now();
}

FakeAnalogSensor::~FakeAnalogSensor()
{
    yCTrace(FAKEANALOGSENSOR);
}


bool FakeAnalogSensor::open(yarp::os::Searchable& config)
{
    yCTrace(FAKEANALOGSENSOR);
    bool correct=true;

    //debug
    fprintf(stderr, "%s\n", config.toString().c_str());

    // Check parameters first
//     if(!config.check("channels"))
//     {
//         correct = false;
//         yCError(FAKEANALOGSENSOR) << "Parameter 'channels' missing";
//     }

    if(!config.check("period"))
    {
        correct = false;
        yCError(FAKEANALOGSENSOR) << "Parameter 'period' missing";
    }

    if (!correct)
    {
        yCError(FAKEANALOGSENSOR) << "Insufficient parameters to FakeAnalogSensor\n";
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
    yCTrace(FAKEANALOGSENSOR);
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
    yCTrace(FAKEANALOGSENSOR);
    // Always ok for now
    return status;
}

int FakeAnalogSensor::getChannels()
{
    yCTrace(FAKEANALOGSENSOR);
    return channelsNum;
}

int FakeAnalogSensor::calibrateSensor()
{
    yCTrace(FAKEANALOGSENSOR);
    //NOT YET IMPLEMENTED
    return 0;
}

int FakeAnalogSensor::calibrateChannel(int ch, double v)
{
    yCTrace(FAKEANALOGSENSOR);
    //NOT YET IMPLEMENTED
    return 0;
}

int FakeAnalogSensor::calibrateSensor(const yarp::sig::Vector& v)
{
    yCTrace(FAKEANALOGSENSOR);
    //NOT YET IMPLEMENTED
    return 0;
}

int FakeAnalogSensor::calibrateChannel(int ch)
{
    yCTrace(FAKEANALOGSENSOR);
    //NOT YET IMPLEMENTED
    return 0;
}

bool FakeAnalogSensor::threadInit()
{
    yCTrace(FAKEANALOGSENSOR);
    return true;
}

void FakeAnalogSensor::run()
{
    mutex.lock();

    // Do fake stuff
    double timeNow = yarp::os::Time::now();

    //if 100ms have passed since the last received message
    if (timeNow > timeStamp + 10) {
        status = IAnalogSensor::AS_TIMEOUT;
    } else {
        status = IAnalogSensor::AS_OK;
    }

    timeStamp = timeNow;
    mutex.unlock();
}

void FakeAnalogSensor::threadRelease()
{
    yCTrace(FAKEANALOGSENSOR);
}

/*
* Copyright (C) 2015 iCub Facility, Istituto Italiano di Tecnologia
* Authors: Alberto Cardellino
* email:   alberto.cardellino@iit.it
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#include <string>
#include <yarp/os/Thread.h>
#include <yarp/os/Time.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/LogStream.h>

#include <fakeIMU.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

/**
 * This device implements a fake analog sensor
 * emulating an IMU
 * @author Alberto Cardellino
 */
fakeIMU::fakeIMU() : RateThread(DEFAULT_PERIOD)
{
    nchannels = 12;
    dummy_value = 0;
}

fakeIMU::~fakeIMU()
{
    close();
}

bool fakeIMU::open(yarp::os::Searchable &config)
{
    int period;
    if( config.check("period"))
    {
        period = config.find("period").asInt();
        setRate(period);
    }
    else
        yInfo() << "Using default period of " << DEFAULT_PERIOD << " ms";

    start();
    return true;
}

bool fakeIMU::close()
{
    fakeIMU::stop();
    return true;
}

bool fakeIMU::read(Vector &out)
{
    if(out.size() != nchannels)
        out.resize(nchannels);


    for(unsigned int i=0; i<nchannels; i++)
        out[i] = dummy_value;
    return true;
}

bool fakeIMU::getChannels(int *nc)
{
    *nc=nchannels;
    return true;
}

bool fakeIMU::calibrate(int ch, double v)
{
    yWarning("Not implemented yet\n");
    return false;
}

bool fakeIMU::threadInit()
{
    lastStamp.update();
    return true;
}

void fakeIMU::run()
{
    dummy_value++;
    lastStamp.update();
}

yarp::os::Stamp fakeIMU::getLastInputStamp()
{
    return lastStamp;
}


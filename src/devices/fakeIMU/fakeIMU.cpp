/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
using namespace yarp::math;

/**
 * This device implements a fake analog sensor
 * emulating an IMU
 * @author Alberto Cardellino
 */
fakeIMU::fakeIMU() : RateThread(DEFAULT_PERIOD)
{
    nchannels = 12;
    dummy_value = 0;
    rpy.resize(3);
    dcm.resize(4,4);
    gravity.resize(4);
    accels.resize(4);
    rpy.zero();
    dcm.zero();
    accels.zero();

    gravity[0] = 0.0;
    gravity[1] = 0;
    gravity[2] = -9.81;
    gravity[3] = 0;

    m_sensorName = "sensorName";
    m_frameName  = "frameName";
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

    constantValue = config.check("constantValue");

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

    out.zero();

    // Euler angle
    for(unsigned int i=0; i<3; i++)
    {
        out[i] = dummy_value;
    }

    // accelerations
    for(unsigned int i=0; i<3; i++)
    {
        out[3+i] = accels[i];
    }

    // gyro
    for(unsigned int i=0; i<3; i++)
    {
        out[6+i] = dummy_value;
    }

    // magnetometer
    for(unsigned int i=0; i<3; i++)
    {
        out[9+i] = dummy_value;
    }

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
    static double count=10;

    rpy[0] = 0;
    rpy[1] = count * 3.14/180;
    rpy[2] = 0;

    dcm = rpy2dcm(rpy);
    accels = gravity * dcm;

    lastStamp.update();

    dummy_value = count;
    if (!constantValue) {
        count++;
    }

    if(count >= 360)
        count = 0;
}

yarp::os::Stamp fakeIMU::getLastInputStamp()
{
    return lastStamp;
}

yarp::dev::MAS_status fakeIMU::genericGetStatus(size_t sens_index) const
{
    if (sens_index!=0) {
        return yarp::dev::MAS_status::MAS_ERROR;
    }

    return yarp::dev::MAS_status::MAS_OK;
}

bool fakeIMU::genericGetSensorName(size_t sens_index, yarp::os::ConstString &name) const
{
    if (sens_index!=0) {
        return false;
    }

    name = m_sensorName;
    return true;
}

bool fakeIMU::genericGetFrameName(size_t sens_index, yarp::os::ConstString &frameName) const
{
    if (sens_index!=0) {
        return false;
    }

    frameName = m_frameName;
    return true;
}

size_t fakeIMU::getNrOfThreeAxisGyroscopes() const
{
    return 1;
}

yarp::dev::MAS_status fakeIMU::getThreeAxisGyroscopeStatus(size_t sens_index) const
{
    return genericGetStatus(sens_index);
}

bool fakeIMU::getThreeAxisGyroscopeName(size_t sens_index, yarp::os::ConstString &name) const
{
    return genericGetSensorName(sens_index, name);
}

bool fakeIMU::getThreeAxisGyroscopeFrameName(size_t sens_index, yarp::os::ConstString &frameName) const
{
    return genericGetFrameName(sens_index, frameName);
}

bool fakeIMU::getThreeAxisGyroscopeMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{
    if (sens_index!=0) {
        return false;
    }

    out.resize(3);
    out[0] = dummy_value;
    out[1] = dummy_value;
    out[2] = dummy_value;

    // Workaround for https://github.com/robotology/yarp/issues/1610
    yarp::os::Stamp copyStamp(lastStamp);
    timestamp = copyStamp.getTime();

    return true;
}

size_t fakeIMU::getNrOfThreeAxisLinearAccelerometers() const
{
    return 1;
}

yarp::dev::MAS_status fakeIMU::getThreeAxisLinearAccelerometerStatus(size_t sens_index) const
{
    return genericGetStatus(sens_index);
}

bool fakeIMU::getThreeAxisLinearAccelerometerName(size_t sens_index, yarp::os::ConstString &name) const
{
    return genericGetSensorName(sens_index, name);
}

bool fakeIMU::getThreeAxisLinearAccelerometerFrameName(size_t sens_index, yarp::os::ConstString &frameName) const
{
    return genericGetFrameName(sens_index, frameName);
}

bool fakeIMU::getThreeAxisLinearAccelerometerMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{
    if (sens_index!=0) {
        return false;
    }

    out.resize(3);
    out[0] = accels[0];
    out[1] = accels[1];
    out[2] = accels[2];

    // Workaround for https://github.com/robotology/yarp/issues/1610
    yarp::os::Stamp copyStamp(lastStamp);
    timestamp = copyStamp.getTime();

    return true;
}

size_t fakeIMU::getNrOfThreeAxisMagnetometers() const
{
    return 1;
}

yarp::dev::MAS_status fakeIMU::getThreeAxisMagnetometerStatus(size_t sens_index) const
{
    return genericGetStatus(sens_index);
}

bool fakeIMU::getThreeAxisMagnetometerName(size_t sens_index, yarp::os::ConstString &name) const
{
    return genericGetSensorName(sens_index, name);
}

bool fakeIMU::getThreeAxisMagnetometerFrameName(size_t sens_index, yarp::os::ConstString &frameName) const
{
    return genericGetFrameName(sens_index, frameName);
}

bool fakeIMU::getThreeAxisMagnetometerMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{
    if (sens_index!=0) {
        return false;
    }

    out.resize(3);
    out[0] = dummy_value;
    out[1] = dummy_value;
    out[2] = dummy_value;

    // Workaround for https://github.com/robotology/yarp/issues/1610
    yarp::os::Stamp copyStamp(lastStamp);
    timestamp = copyStamp.getTime();

    return true;
}

size_t fakeIMU::getNrOfOrientationSensors() const
{
    return 1;
}

yarp::dev::MAS_status fakeIMU::getOrientationSensorStatus(size_t sens_index) const
{
    return genericGetStatus(sens_index);
}

bool fakeIMU::getOrientationSensorName(size_t sens_index, yarp::os::ConstString &name) const
{
    return genericGetSensorName(sens_index, name);
}

bool fakeIMU::getOrientationSensorFrameName(size_t sens_index, yarp::os::ConstString &frameName) const
{
    return genericGetFrameName(sens_index, frameName);
}

bool fakeIMU::getOrientationSensorMeasureAsRollPitchYaw(size_t sens_index, yarp::sig::Vector& rpy_out, double& timestamp) const
{
    if (sens_index!=0) {
        return false;
    }

    rpy_out.resize(3);
    rpy_out[0] = dummy_value;
    rpy_out[1] = dummy_value;
    rpy_out[2] = dummy_value;

    // Workaround for https://github.com/robotology/yarp/issues/1610
    yarp::os::Stamp copyStamp(lastStamp);
    timestamp = copyStamp.getTime();

    return true;
}

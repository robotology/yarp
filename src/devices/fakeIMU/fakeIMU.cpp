/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fakeIMU.h"

#include <yarp/os/Thread.h>
#include <yarp/os/Time.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

#include <string>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::math;

namespace {
YARP_LOG_COMPONENT(FAKEIMU, "yarp.device.fakeIMU")
constexpr double DEFAULT_PERIOD = 0.01; // seconds
constexpr int DEFAULT_NCHANNELS = 12;
constexpr double DEFAULT_DUMMY_VALUE = 0.0;
constexpr const char* DEFAULT_SENSOR_NAME = "sensorName";
constexpr const char* DEFAULT_FRAME_NAME = "frameName";

constexpr double EARTH_GRAVITY = -9.81;
}

/**
 * This device implements a fake analog sensor
 * emulating an IMU
 */
fakeIMU::fakeIMU() :
        PeriodicThread(DEFAULT_PERIOD),
        rpy({0.0, 0.0, 0.0}),
        gravity({0.0, 0.0, EARTH_GRAVITY, 0.0}),
        dcm(4, 4),
        accels({0.0, 0.0, 0.0, 0.0}),
        nchannels(DEFAULT_NCHANNELS),
        dummy_value(DEFAULT_DUMMY_VALUE),
        m_sensorName(DEFAULT_SENSOR_NAME),
        m_frameName(DEFAULT_FRAME_NAME)
{
    dcm.zero();
}

fakeIMU::~fakeIMU()
{
    close();
}

bool fakeIMU::open(yarp::os::Searchable &config)
{
    double period;
    if( config.check("period")) {
        period = config.find("period").asInt32() / 1000.0;
        setPeriod(period);
    } else  {
        yCInfo(FAKEIMU) << "Using default period of " << DEFAULT_PERIOD << " s";
    }

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
    if (out.size() != nchannels) {
        out.resize(nchannels);
    }

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
    yCWarning(FAKEIMU, "Not implemented yet");
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

    if (count >= 360) {
        count = 0;
    }
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

bool fakeIMU::genericGetSensorName(size_t sens_index, std::string &name) const
{
    if (sens_index!=0) {
        return false;
    }

    name = m_sensorName;
    return true;
}

bool fakeIMU::genericGetFrameName(size_t sens_index, std::string &frameName) const
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

bool fakeIMU::getThreeAxisGyroscopeName(size_t sens_index, std::string &name) const
{
    return genericGetSensorName(sens_index, name);
}

bool fakeIMU::getThreeAxisGyroscopeFrameName(size_t sens_index, std::string &frameName) const
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

bool fakeIMU::getThreeAxisLinearAccelerometerName(size_t sens_index, std::string &name) const
{
    return genericGetSensorName(sens_index, name);
}

bool fakeIMU::getThreeAxisLinearAccelerometerFrameName(size_t sens_index, std::string &frameName) const
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

bool fakeIMU::getThreeAxisMagnetometerName(size_t sens_index, std::string &name) const
{
    return genericGetSensorName(sens_index, name);
}

bool fakeIMU::getThreeAxisMagnetometerFrameName(size_t sens_index, std::string &frameName) const
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

bool fakeIMU::getOrientationSensorName(size_t sens_index, std::string &name) const
{
    return genericGetSensorName(sens_index, name);
}

bool fakeIMU::getOrientationSensorFrameName(size_t sens_index, std::string &frameName) const
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

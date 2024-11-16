/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FakeIMU.h"

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

constexpr double EARTH_GRAVITY = -9.81;
}

/**
 * This device implements a fake analog sensor
 * emulating an IMU
 */
FakeIMU::FakeIMU() :
        PeriodicThread(DEFAULT_PERIOD),
        rpy({0.0, 0.0, 0.0}),
        gravity({0.0, 0.0, EARTH_GRAVITY, 0.0}),
        dcm(4, 4),
        accels({0.0, 0.0, 0.0, 0.0}),
        nchannels(DEFAULT_NCHANNELS),
        dummy_value(DEFAULT_DUMMY_VALUE)
{
    dcm.zero();
}

FakeIMU::~FakeIMU()
{
    close();
}

bool FakeIMU::open(yarp::os::Searchable &config)
{
    if (!this->parseParams(config)) {return false;}

    double dperiod = m_period / 1000.0;
    setPeriod(dperiod);

    start();
    return true;
}

bool FakeIMU::close()
{
    FakeIMU::stop();
    return true;
}

bool FakeIMU::threadInit()
{
    lastStamp.update();
    return true;
}

void FakeIMU::run()
{
    static double count=10;

    rpy[0] = 0;
    rpy[1] = count * 3.14/180;
    rpy[2] = 0;

    dcm = rpy2dcm(rpy);
    accels = gravity * dcm;

    lastStamp.update();

    dummy_value = count;
    if (!m_constantValue) {
        count++;
    }

    if (count >= 360) {
        count = 0;
    }
}

yarp::dev::MAS_status FakeIMU::genericGetStatus(size_t sens_index) const
{
    if (sens_index!=0) {
        return yarp::dev::MAS_status::MAS_ERROR;
    }

    return yarp::dev::MAS_status::MAS_OK;
}

bool FakeIMU::genericGetSensorName(size_t sens_index, std::string &name) const
{
    if (sens_index!=0) {
        return false;
    }

    name = m_sensorName;
    return true;
}

bool FakeIMU::genericGetFrameName(size_t sens_index, std::string &frameName) const
{
    if (sens_index!=0) {
        return false;
    }

    frameName = m_frameName;
    return true;
}

size_t FakeIMU::getNrOfThreeAxisGyroscopes() const
{
    return 1;
}

yarp::dev::MAS_status FakeIMU::getThreeAxisGyroscopeStatus(size_t sens_index) const
{
    return genericGetStatus(sens_index);
}

bool FakeIMU::getThreeAxisGyroscopeName(size_t sens_index, std::string &name) const
{
    return genericGetSensorName(sens_index, name);
}

bool FakeIMU::getThreeAxisGyroscopeFrameName(size_t sens_index, std::string &frameName) const
{
    return genericGetFrameName(sens_index, frameName);
}

bool FakeIMU::getThreeAxisGyroscopeMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
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

size_t FakeIMU::getNrOfThreeAxisLinearAccelerometers() const
{
    return 1;
}

yarp::dev::MAS_status FakeIMU::getThreeAxisLinearAccelerometerStatus(size_t sens_index) const
{
    return genericGetStatus(sens_index);
}

bool FakeIMU::getThreeAxisLinearAccelerometerName(size_t sens_index, std::string &name) const
{
    return genericGetSensorName(sens_index, name);
}

bool FakeIMU::getThreeAxisLinearAccelerometerFrameName(size_t sens_index, std::string &frameName) const
{
    return genericGetFrameName(sens_index, frameName);
}

bool FakeIMU::getThreeAxisLinearAccelerometerMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
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

size_t FakeIMU::getNrOfThreeAxisMagnetometers() const
{
    return 1;
}

yarp::dev::MAS_status FakeIMU::getThreeAxisMagnetometerStatus(size_t sens_index) const
{
    return genericGetStatus(sens_index);
}

bool FakeIMU::getThreeAxisMagnetometerName(size_t sens_index, std::string &name) const
{
    return genericGetSensorName(sens_index, name);
}

bool FakeIMU::getThreeAxisMagnetometerFrameName(size_t sens_index, std::string &frameName) const
{
    return genericGetFrameName(sens_index, frameName);
}

bool FakeIMU::getThreeAxisMagnetometerMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
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

size_t FakeIMU::getNrOfOrientationSensors() const
{
    return 1;
}

yarp::dev::MAS_status FakeIMU::getOrientationSensorStatus(size_t sens_index) const
{
    return genericGetStatus(sens_index);
}

bool FakeIMU::getOrientationSensorName(size_t sens_index, std::string &name) const
{
    return genericGetSensorName(sens_index, name);
}

bool FakeIMU::getOrientationSensorFrameName(size_t sens_index, std::string &frameName) const
{
    return genericGetFrameName(sens_index, frameName);
}

bool FakeIMU::getOrientationSensorMeasureAsRollPitchYaw(size_t sens_index, yarp::sig::Vector& rpy_out, double& timestamp) const
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

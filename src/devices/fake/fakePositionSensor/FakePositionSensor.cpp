/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FakePositionSensor.h"

#include <yarp/os/Time.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

using namespace yarp::dev;

namespace {
YARP_LOG_COMPONENT(FAKE_POSITION_SENSOR, "yarp.device.fakePositionSensor")
}

FakePositionSensor::FakePositionSensor(double period) : PeriodicThread(period),
        m_mutex(),
        m_channelsNum(1)
{
    yCTrace(FAKE_POSITION_SENSOR);
}

FakePositionSensor::~FakePositionSensor()
{
    yCTrace(FAKE_POSITION_SENSOR);
}

bool FakePositionSensor::open(yarp::os::Searchable& config)
{
    if (!this->parseParams(config)) {return false;}

    setPeriod(m_period);

    //create the data vector:
    this->m_channelsNum = 1;
    m_orientation_sensors.resize(m_channelsNum);
    m_position_sensors.resize(m_channelsNum);

    return PeriodicThread::start();
}

bool FakePositionSensor::close()
{
    yCTrace(FAKE_POSITION_SENSOR);
    //stop the thread
    PeriodicThread::stop();

    return true;
}

bool FakePositionSensor::threadInit()
{
    yCTrace(FAKE_POSITION_SENSOR);
    return true;
}

void FakePositionSensor::run()
{
    m_mutex.lock();

    // Do fake stuff
    double timeNow = yarp::os::Time::now();

    for (size_t i = 0; i < m_position_sensors.size(); i++)
    {
        m_position_sensors[i].m_timestamp = timeNow;
        m_position_sensors[i].m_status = yarp::dev::MAS_status::MAS_OK;
        for (auto it= m_position_sensors[i].m_data.begin(); it != m_position_sensors[i].m_data.end(); it++)
        {
            *it = *it + 0.001;
        }
    }
    for (size_t i = 0; i < m_orientation_sensors.size(); i++)
    {
        m_orientation_sensors[i].m_timestamp = timeNow;
        m_orientation_sensors[i].m_status = yarp::dev::MAS_status::MAS_OK;
        for (auto it = m_orientation_sensors[i].m_data.begin(); it != m_orientation_sensors[i].m_data.end(); it++)
        {
            *it = *it - 0.001;
        }
    }

    m_mutex.unlock();
}

void FakePositionSensor::threadRelease()
{
    yCTrace(FAKE_POSITION_SENSOR);
}


size_t FakePositionSensor::getNrOfPositionSensors() const
{
    std::lock_guard<std::mutex> myLockGuard(m_mutex);
    return m_position_sensors.size();
}

yarp::dev::MAS_status FakePositionSensor::getPositionSensorStatus(size_t sens_index) const
{
    std::lock_guard<std::mutex> myLockGuard (m_mutex);
    if (sens_index >= m_position_sensors.size()) return yarp::dev::MAS_status::MAS_UNKNOWN;
    return m_position_sensors[sens_index].m_status;
}

bool FakePositionSensor::getPositionSensorName(size_t sens_index, std::string& name) const
{
    std::lock_guard<std::mutex> myLockGuard(m_mutex);
    if (sens_index >= m_position_sensors.size()) return false;
    name = m_position_sensors[sens_index].m_name;
    return true;
}

bool FakePositionSensor::getPositionSensorFrameName(size_t sens_index, std::string& frameName) const
{
    std::lock_guard<std::mutex> myLockGuard(m_mutex);
    if (sens_index >= m_position_sensors.size()) return false;
    frameName = m_position_sensors[sens_index].m_framename;
    return true;
}

bool FakePositionSensor::getPositionSensorMeasure(size_t sens_index, yarp::sig::Vector& xyz, double& timestamp) const
{
    std::lock_guard<std::mutex> myLockGuard(m_mutex);
    if (sens_index >= m_position_sensors.size()) return false;
    timestamp = m_position_sensors[sens_index].m_timestamp;
    xyz = m_position_sensors[sens_index].m_data;
    return true;
}

size_t FakePositionSensor::getNrOfOrientationSensors() const
{
    std::lock_guard<std::mutex> myLockGuard(m_mutex);
    return m_orientation_sensors.size();
}

yarp::dev::MAS_status FakePositionSensor::getOrientationSensorStatus(size_t sens_index) const
{
    std::lock_guard<std::mutex> myLockGuard(m_mutex);
    if (sens_index >= m_orientation_sensors.size()) return yarp::dev::MAS_status::MAS_UNKNOWN;
    return m_orientation_sensors[sens_index].m_status;
}

bool FakePositionSensor::getOrientationSensorName(size_t sens_index, std::string& name) const
{
    std::lock_guard<std::mutex> myLockGuard(m_mutex);
    if (sens_index >= m_orientation_sensors.size()) return false;
    name = m_orientation_sensors[sens_index].m_name;
    return true;
}

bool FakePositionSensor::getOrientationSensorFrameName(size_t sens_index, std::string& frameName) const
{
    std::lock_guard<std::mutex> myLockGuard(m_mutex);
    if (sens_index >= m_orientation_sensors.size()) return false;
    frameName = m_orientation_sensors[sens_index].m_framename;
    return true;
}

bool FakePositionSensor::getOrientationSensorMeasureAsRollPitchYaw(size_t sens_index, yarp::sig::Vector& xyz, double& timestamp) const
{
    std::lock_guard<std::mutex> myLockGuard(m_mutex);
    if (sens_index >= m_orientation_sensors.size()) return false;
    timestamp = m_orientation_sensors[sens_index].m_timestamp;
    xyz = m_orientation_sensors[sens_index].m_data;
    return true;
}

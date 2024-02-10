/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEVICE_FAKE_POSITIONSENSOR
#define YARP_DEVICE_FAKE_POSITIONSENSOR

#include <yarp/os/PeriodicThread.h>

#include <yarp/dev/all.h>
#include <yarp/dev/MultipleAnalogSensorsInterfaces.h>

#include <mutex>

#include "FakePositionSensor_ParamsParser.h"

/**
*
* @ingroup dev_impl_fake dev_impl_media dev_impl_analog_sensors
*
* \brief `fakePositionSensor`: Fake position sensor device for testing purpose and reference for new similar devices
*
* Parameters accepted in the config argument of the open method:
* | Parameter name | Type   | Units   | Default Value | Required | Description | Notes |
* |:--------------:|:------:|:-------:|:-------------:|:--------:|:-----------:|:-----:|
* | sensor_period  | double | seconds |   0.01        | No       | Period over which the measurement is updated.  |       |
*/

class FakePositionSensor :
        public yarp::dev::DeviceDriver,
        public yarp::os::PeriodicThread,
        public yarp::dev::IPositionSensors,
        public yarp::dev::IOrientationSensors,
        public FakePositionSensor_ParamsParser
{
private:

    mutable std::mutex      m_mutex;
    unsigned int            m_channelsNum;

    struct myfakesensor
    {
        std::string             m_name = "mySensor";
        std::string             m_framename = "myFrame";
        yarp::dev::MAS_status   m_status = yarp::dev::MAS_status::MAS_OK;
        double                  m_timestamp = 0.0;
        yarp::sig::Vector       m_data;

        myfakesensor()
        {
            m_data.resize(3);
            m_data[0] = 1.0;
            m_data[1] = 2.0;
            m_data[2] = 3.0;
        }
    };

    std::vector<myfakesensor>     m_position_sensors;
    std::vector<myfakesensor>     m_orientation_sensors;

public:
    FakePositionSensor(double period = 0.05);

    ~FakePositionSensor();

    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    //Interfaces
    size_t getNrOfPositionSensors() const override;
    yarp::dev::MAS_status getPositionSensorStatus(size_t sens_index) const override;
    bool getPositionSensorName(size_t sens_index, std::string& name) const override;
    bool getPositionSensorFrameName(size_t sens_index, std::string& frameName) const override;
    bool getPositionSensorMeasure(size_t sens_index, yarp::sig::Vector& xyz, double& timestamp) const override;

    size_t getNrOfOrientationSensors() const override;
    yarp::dev::MAS_status getOrientationSensorStatus(size_t sens_index) const override;
    bool getOrientationSensorName(size_t sens_index, std::string& name) const override;
    bool getOrientationSensorFrameName(size_t sens_index, std::string& frameName) const override;
    bool getOrientationSensorMeasureAsRollPitchYaw(size_t sens_index, yarp::sig::Vector& xyz, double& timestamp) const override;

    // RateThread interface
    void run() override;
    bool threadInit() override;
    void threadRelease() override;
};


#endif  // YARP_DEVICE_FAKE_POSITIONSENSOR

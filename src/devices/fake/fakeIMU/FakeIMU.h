/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/MultipleAnalogSensorsInterfaces.h>
#include <yarp/os/Stamp.h>
#include <yarp/math/Math.h>
#include "FakeIMU_ParamsParser.h"

/**
* @ingroup dev_impl_fake
* \brief `fakeIMU` : fake device implementing the device interface typically implemented by an Inertial Measurement Unit
*
* Parameters required by this device are shown in class: FakeIMU_ParamsParser
*
*/
class FakeIMU :
        public yarp::dev::DeviceDriver,
        public yarp::os::PeriodicThread,
        public yarp::dev::IThreeAxisGyroscopes,
        public yarp::dev::IThreeAxisLinearAccelerometers,
        public yarp::dev::IThreeAxisMagnetometers,
        public yarp::dev::IOrientationSensors,
        public FakeIMU_ParamsParser
{
public:
    FakeIMU();
    FakeIMU(const FakeIMU&) = delete;
    FakeIMU(FakeIMU&&) = delete;
    FakeIMU& operator=(const FakeIMU&) = delete;
    FakeIMU& operator=(FakeIMU&&) = delete;

    ~FakeIMU() override;

    // Device Driver interface
    bool open(yarp::os::Searchable &config) override;
    bool close() override;

    /* IThreeAxisGyroscopes methods */
    yarp::dev::ReturnValue getNrOfThreeAxisGyroscopes(size_t& num) const override;
    yarp::dev::MAS_status getThreeAxisGyroscopeStatus(size_t sens_index) const override;
    yarp::dev::ReturnValue getThreeAxisGyroscopeName(size_t sens_index, std::string &name) const override;
    yarp::dev::ReturnValue getThreeAxisGyroscopeFrameName(size_t sens_index, std::string &frameName) const override;
    yarp::dev::ReturnValue getThreeAxisGyroscopeMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;

    /* IThreeAxisLinearAccelerometers methods */
    yarp::dev::ReturnValue getNrOfThreeAxisLinearAccelerometers(size_t& num) const override;
    yarp::dev::MAS_status getThreeAxisLinearAccelerometerStatus(size_t sens_index) const override;
    yarp::dev::ReturnValue getThreeAxisLinearAccelerometerName(size_t sens_index, std::string &name) const override;
    yarp::dev::ReturnValue getThreeAxisLinearAccelerometerFrameName(size_t sens_index, std::string &frameName) const override;
    yarp::dev::ReturnValue getThreeAxisLinearAccelerometerMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;

    /* IThreeAxisMagnetometers methods */
    yarp::dev::ReturnValue getNrOfThreeAxisMagnetometers(size_t& num) const override;
    yarp::dev::MAS_status getThreeAxisMagnetometerStatus(size_t sens_index) const override;
    yarp::dev::ReturnValue getThreeAxisMagnetometerName(size_t sens_index, std::string &name) const override;
    yarp::dev::ReturnValue getThreeAxisMagnetometerFrameName(size_t sens_index, std::string &frameName) const override;
    yarp::dev::ReturnValue getThreeAxisMagnetometerMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;

    /* IOrientationSensors methods */
    yarp::dev::ReturnValue getNrOfOrientationSensors(size_t& num) const override;
    yarp::dev::MAS_status getOrientationSensorStatus(size_t sens_index) const override;
    yarp::dev::ReturnValue getOrientationSensorName(size_t sens_index, std::string &name) const override;
    yarp::dev::ReturnValue getOrientationSensorFrameName(size_t sens_index, std::string &frameName) const override;
    yarp::dev::ReturnValue getOrientationSensorMeasureAsRollPitchYaw(size_t sens_index, yarp::sig::Vector& rpy, double& timestamp) const override;

private:
    yarp::dev::MAS_status genericGetStatus(size_t sens_index) const;
    yarp::dev::ReturnValue genericGetSensorName(size_t sens_index, std::string &name) const;
    yarp::dev::ReturnValue genericGetFrameName(size_t sens_index, std::string &frameName) const;

    bool threadInit() override;
    void run() override;

    yarp::sig::Vector rpy;
    yarp::sig::Vector gravity;
    yarp::sig::Matrix dcm;
    yarp::sig::Vector accels;

    unsigned int nchannels;
    double dummy_value;
    yarp::os::Stamp lastStamp;
};

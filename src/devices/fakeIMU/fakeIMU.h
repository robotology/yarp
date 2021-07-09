/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/IGenericSensor.h>
#include <yarp/dev/MultipleAnalogSensorsInterfaces.h>
#include <yarp/os/Stamp.h>
#include <yarp/dev/IPreciselyTimed.h>
#include <yarp/math/Math.h>

/**
* @ingroup dev_impl_fake
* \brief `fakeIMU` : fake device implementing the device interface typically implemented by an Inertial Measurement Unit
*
* | YARP device name |
* |:-----------------:|
* | `fakeIMU` |
*
*
* The parameters accepted by this device are:
* | Parameter name | SubParameter   | Type    | Units          | Default Value | Required                    | Description                                                       | Notes |
* |:--------------:|:--------------:|:-------:|:--------------:|:-------------:|:--------------------------: |:-----------------------------------------------------------------:|:-----:|
* | period         |       -        | int     | millisecond    |   10          | No          | Period over which the measurement is updated.  |       |
* | constantValue  |       -        |  -      | -              |   -           | No          | If the parameter is present, the fake sensor values never changes (useful for testing server/client coherence).  |       |
*
*/
class fakeIMU :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IGenericSensor,
        public yarp::os::PeriodicThread,
        public yarp::dev::IPreciselyTimed,
        public yarp::dev::IThreeAxisGyroscopes,
        public yarp::dev::IThreeAxisLinearAccelerometers,
        public yarp::dev::IThreeAxisMagnetometers,
        public yarp::dev::IOrientationSensors
{
public:
    fakeIMU();
    fakeIMU(const fakeIMU&) = delete;
    fakeIMU(fakeIMU&&) = delete;
    fakeIMU& operator=(const fakeIMU&) = delete;
    fakeIMU& operator=(fakeIMU&&) = delete;

    ~fakeIMU() override;

    // Device Driver interface
    bool open(yarp::os::Searchable &config) override;
    bool close() override;

    // IGenericSensor interface.
    bool read(yarp::sig::Vector &out) override;
    bool getChannels(int *nc) override;
    bool calibrate(int ch, double v) override;

    // IPreciselyTimed interface
    yarp::os::Stamp getLastInputStamp() override;

    /* IThreeAxisGyroscopes methods */
    size_t getNrOfThreeAxisGyroscopes() const override;
    yarp::dev::MAS_status getThreeAxisGyroscopeStatus(size_t sens_index) const override;
    bool getThreeAxisGyroscopeName(size_t sens_index, std::string &name) const override;
    bool getThreeAxisGyroscopeFrameName(size_t sens_index, std::string &frameName) const override;
    bool getThreeAxisGyroscopeMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;

    /* IThreeAxisLinearAccelerometers methods */
    size_t getNrOfThreeAxisLinearAccelerometers() const override;
    yarp::dev::MAS_status getThreeAxisLinearAccelerometerStatus(size_t sens_index) const override;
    bool getThreeAxisLinearAccelerometerName(size_t sens_index, std::string &name) const override;
    bool getThreeAxisLinearAccelerometerFrameName(size_t sens_index, std::string &frameName) const override;
    bool getThreeAxisLinearAccelerometerMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;

    /* IThreeAxisMagnetometers methods */
    size_t getNrOfThreeAxisMagnetometers() const override;
    yarp::dev::MAS_status getThreeAxisMagnetometerStatus(size_t sens_index) const override;
    bool getThreeAxisMagnetometerName(size_t sens_index, std::string &name) const override;
    bool getThreeAxisMagnetometerFrameName(size_t sens_index, std::string &frameName) const override;
    bool getThreeAxisMagnetometerMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;

    /* IOrientationSensors methods */
    size_t getNrOfOrientationSensors() const override;
    yarp::dev::MAS_status getOrientationSensorStatus(size_t sens_index) const override;
    bool getOrientationSensorName(size_t sens_index, std::string &name) const override;
    bool getOrientationSensorFrameName(size_t sens_index, std::string &frameName) const override;
    bool getOrientationSensorMeasureAsRollPitchYaw(size_t sens_index, yarp::sig::Vector& rpy, double& timestamp) const override;

private:
    yarp::dev::MAS_status genericGetStatus(size_t sens_index) const;
    bool genericGetSensorName(size_t sens_index, std::string &name) const;
    bool genericGetFrameName(size_t sens_index, std::string &frameName) const;

    bool threadInit() override;
    void run() override;

    yarp::sig::Vector rpy;
    yarp::sig::Vector gravity;
    yarp::sig::Matrix dcm;
    yarp::sig::Vector accels;

    unsigned int nchannels;
    double dummy_value;
    yarp::os::Stamp lastStamp;
    std::string m_sensorName;
    std::string m_frameName;
    bool constantValue;
};

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_MULTIPLEANALOGSENSORSCLIENT_MULTIPLEANALOGSENSORSCLIENT_H
#define YARP_DEV_MULTIPLEANALOGSENSORSCLIENT_MULTIPLEANALOGSENSORSCLIENT_H

#include <yarp/dev/MultipleAnalogSensorsInterfaces.h>

#include "MultipleAnalogSensorsMetadata.h"
#include "SensorStreamingData.h"

#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>
#include <yarp/dev/DeviceDriver.h>

#include <mutex>


class SensorStreamingDataInputPort :
        public yarp::os::BufferedPort<SensorStreamingData>
{
public:
    SensorStreamingData receivedData;
    mutable yarp::dev::MAS_status status{yarp::dev::MAS_WAITING_FOR_FIRST_READ};
    mutable std::mutex dataMutex;
    double timeoutInSeconds{0.01};
    double lastTimeStampReadInSeconds{0.0};

    using yarp::os::BufferedPort<SensorStreamingData>::onRead;
    void onRead(SensorStreamingData &v) override;
    void updateTimeoutStatus() const;
};

/**
* @ingroup dev_impl_network_clients
*
* \brief `multipleanalogsensorsclient`: The client side of a device exposing MultipleAnalogSensors interfaces.
*
* | YARP device name |
* |:-----------------:|
* | `multipleanalogsensorsclient` |
*
* The parameters accepted by this device are:
* | Parameter name     | SubParameter   | Type    | Units          | Default Value | Required     | Description                                                                            | Notes |
* |:------------------:|:--------------:|:-------:|:--------------:|:-------------:|:------------:|:--------------------------------------------------------------------------------------:|:-----:|
* | remote             |       -        | string  | -              |   -           | Yes          | Prefix of the ports to which to connect, opened by MultipleAnalogSensorsServer device. |       |
* | local              |       -        | string  | -              |   -           | Yes          | Port prefix of the ports opened by this device.                                        |       |
* | timeout            |       -        | double  | seconds        | 0.01          | No           | Timeout after which the device reports an error if no measurement was received.        |       |
* | externalConnection |       -        | bool    | -              | false         | No           | If set to true, the connection to the rpc port of the MAS server is skipped and it is possible to connect to the data source externally after being opened | Use case: e.g yarpdataplayer source. Note that with this configuration some information like sensor name, frame name and sensor number will be not available.|
*
*/
class MultipleAnalogSensorsClient :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IThreeAxisGyroscopes,
        public yarp::dev::IThreeAxisLinearAccelerometers,
        public yarp::dev::IThreeAxisMagnetometers,
        public yarp::dev::IPositionSensors,
        public yarp::dev::IOrientationSensors,
        public yarp::dev::ITemperatureSensors,
        public yarp::dev::ISixAxisForceTorqueSensors,
        public yarp::dev::IContactLoadCellArrays,
        public yarp::dev::IEncoderArrays,
        public yarp::dev::ISkinPatches
{
    SensorStreamingDataInputPort m_streamingPort;
    yarp::os::Port m_rpcPort;
    std::string m_localRPCPortName;
    std::string m_localStreamingPortName;
    std::string m_remoteRPCPortName;
    std::string m_remoteStreamingPortName;
    bool m_RPCConnectionActive{false};
    bool m_StreamingConnectionActive{false};
    bool m_externalConnection{false};

    MultipleAnalogSensorsMetadata m_RPCInterface;
    SensorRPCData m_sensorsMetadata;

    size_t genericGetNrOfSensors(const std::vector<SensorMetadata>& metadataVector,
                                 const SensorMeasurements& measurementsVector) const;
    yarp::dev::MAS_status genericGetStatus() const;
    bool genericGetName(const std::vector<SensorMetadata>& metadataVector, const std::string& tag,
                          size_t sens_index, std::string &name) const;
    bool genericGetFrameName(const std::vector<SensorMetadata>& metadataVector, const std::string& tag,
                            size_t sens_index, std::string &frameName) const;
    bool genericGetMeasure(const std::vector<SensorMetadata>& metadataVector, const std::string& tag,
                             const SensorMeasurements& measurementsVector,
                             size_t sens_index, yarp::sig::Vector& out, double& timestamp) const;
    size_t genericGetSize(const std::vector<SensorMetadata>& metadataVector,
                          const std::string& tag, const SensorMeasurements& measurementsVector, size_t sens_index) const;


public:
    /* DevideDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

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

    /* IPositionSensors methods */
    size_t getNrOfPositionSensors() const override;
    yarp::dev::MAS_status getPositionSensorStatus(size_t sens_index) const override;
    bool getPositionSensorName(size_t sens_index, std::string& name) const override;
    bool getPositionSensorFrameName(size_t sens_index, std::string& frameName) const override;
    bool getPositionSensorMeasure(size_t sens_index, yarp::sig::Vector& xyz, double& timestamp) const override;

    /* IOrientationSensors methods */
    size_t getNrOfOrientationSensors() const override;
    yarp::dev::MAS_status getOrientationSensorStatus(size_t sens_index) const override;
    bool getOrientationSensorName(size_t sens_index, std::string &name) const override;
    bool getOrientationSensorFrameName(size_t sens_index, std::string &frameName) const override;
    bool getOrientationSensorMeasureAsRollPitchYaw(size_t sens_index, yarp::sig::Vector& rpy, double& timestamp) const override;

    /* ITemperatureSensors methods */
    size_t getNrOfTemperatureSensors() const override;
    yarp::dev::MAS_status getTemperatureSensorStatus(size_t sens_index) const override;
    bool getTemperatureSensorName(size_t sens_index, std::string &name) const override;
    bool getTemperatureSensorFrameName(size_t sens_index, std::string &frameName) const override;
    bool getTemperatureSensorMeasure(size_t sens_index, double& out, double& timestamp) const override;
    bool getTemperatureSensorMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;

    /* ISixAxisForceTorqueSensors */
    size_t getNrOfSixAxisForceTorqueSensors() const override;
    yarp::dev::MAS_status getSixAxisForceTorqueSensorStatus(size_t sens_index) const override;
    bool getSixAxisForceTorqueSensorName(size_t sens_index, std::string &name) const override;
    bool getSixAxisForceTorqueSensorFrameName(size_t sens_index, std::string &frame) const override;
    bool getSixAxisForceTorqueSensorMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;

    /* IContactLoadCellArrays */
    size_t getNrOfContactLoadCellArrays() const override;
    yarp::dev::MAS_status getContactLoadCellArrayStatus(size_t sens_index) const override;
    bool getContactLoadCellArrayName(size_t sens_index, std::string &name) const override;
    bool getContactLoadCellArrayMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;
    size_t getContactLoadCellArraySize(size_t sens_index) const override;

    /* IEncoderArrays */
    size_t getNrOfEncoderArrays() const override;
    yarp::dev::MAS_status getEncoderArrayStatus(size_t sens_index) const override;
    bool getEncoderArrayName(size_t sens_index, std::string &name) const override;
    bool getEncoderArrayMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;
    size_t getEncoderArraySize(size_t sens_index) const override;

    /* ISkinPatches */
    size_t getNrOfSkinPatches() const override;
    yarp::dev::MAS_status getSkinPatchStatus(size_t sens_index) const override;
    bool getSkinPatchName(size_t sens_index, std::string &name) const override;
    bool getSkinPatchMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;
    size_t getSkinPatchSize(size_t sens_index) const override;
};

#endif

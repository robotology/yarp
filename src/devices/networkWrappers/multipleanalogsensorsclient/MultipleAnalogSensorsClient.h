/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_MULTIPLEANALOGSENSORSCLIENT_MULTIPLEANALOGSENSORSCLIENT_H
#define YARP_DEV_MULTIPLEANALOGSENSORSCLIENT_MULTIPLEANALOGSENSORSCLIENT_H

#include <yarp/dev/MultipleAnalogSensorsInterfaces.h>

#include "MultipleAnalogSensorsMetadata.h"
#include <yarp/dev/SensorStreamingData.h>

#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ReturnValue.h>

#include "MultipleAnalogSensorsClient_ParamsParser.h"

#include <mutex>


class SensorStreamingDataInputPort :
        public yarp::os::BufferedPort< yarp::dev::SensorStreamingData>
{
public:
    yarp::dev::SensorStreamingData receivedData;
    mutable yarp::dev::MAS_status status{yarp::dev::MAS_WAITING_FOR_FIRST_READ};
    mutable std::mutex dataMutex;
    double timeoutInSeconds{0.01};
    double lastTimeStampReadInSeconds{0.0};

    using yarp::os::BufferedPort< yarp::dev::SensorStreamingData>::onRead;
    void onRead(yarp::dev::SensorStreamingData &v) override;
    void updateTimeoutStatus() const;
};

/**
* @ingroup dev_impl_network_clients
*
* \brief `multipleanalogsensorsclient`: The client side of a device exposing MultipleAnalogSensors interfaces.
*
* Parameters required by this device are shown in class: MultipleAnalogSensorsClient_ParamsParser
*
*/
class MultipleAnalogSensorsClient :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IThreeAxisGyroscopes,
        public yarp::dev::IThreeAxisLinearAccelerometers,
        public yarp::dev::IThreeAxisAngularAccelerometers,
        public yarp::dev::IThreeAxisMagnetometers,
        public yarp::dev::IPositionSensors,
        public yarp::dev::ILinearVelocitySensors,
        public yarp::dev::IOrientationSensors,
        public yarp::dev::ITemperatureSensors,
        public yarp::dev::ISixAxisForceTorqueSensors,
        public yarp::dev::IContactLoadCellArrays,
        public yarp::dev::IEncoderArrays,
        public yarp::dev::ISkinPatches,
        public MultipleAnalogSensorsClient_ParamsParser
{
    SensorStreamingDataInputPort m_streamingPort;
    yarp::os::Port m_rpcPort;

    MultipleAnalogSensorsMetadata m_RPCInterface;
    SensorRPCData m_sensorsMetadata;

    size_t genericGetNrOfSensors(const std::vector<SensorMetadata>& metadataVector,
                                 const  yarp::dev::SensorMeasurements& measurementsVector) const;
    yarp::dev::MAS_status genericGetStatus() const;
    yarp::dev::ReturnValue genericGetName(const std::vector<SensorMetadata>& metadataVector, const std::string& tag,
                          size_t sens_index, std::string &name) const;
    yarp::dev::ReturnValue genericGetFrameName(const std::vector<SensorMetadata>& metadataVector, const std::string& tag,
                            size_t sens_index, std::string &frameName) const;
    yarp::dev::ReturnValue genericGetMeasure(const std::vector<SensorMetadata>& metadataVector, const std::string& tag,
                             const  yarp::dev::SensorMeasurements& measurementsVector,
                             size_t sens_index, yarp::sig::Vector& out, double& timestamp) const;
    size_t genericGetSize(const std::vector<SensorMetadata>& metadataVector,
                          const std::string& tag, const  yarp::dev::SensorMeasurements& measurementsVector, size_t sens_index) const;


public:
    /* DevideDriver methods */
    bool open(yarp::os::Searchable& config) override;
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

    /* IThreeAxisAngularAccelerometers methods */
    yarp::dev::ReturnValue getNrOfThreeAxisAngularAccelerometers(size_t& num) const override;
    yarp::dev::MAS_status getThreeAxisAngularAccelerometerStatus(size_t sens_index) const override;
    yarp::dev::ReturnValue getThreeAxisAngularAccelerometerName(size_t sens_index, std::string &name) const override;
    yarp::dev::ReturnValue getThreeAxisAngularAccelerometerFrameName(size_t sens_index, std::string &frameName) const override;
    yarp::dev::ReturnValue getThreeAxisAngularAccelerometerMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;

    /* IThreeAxisMagnetometers methods */
    yarp::dev::ReturnValue getNrOfThreeAxisMagnetometers(size_t& num) const override;
    yarp::dev::MAS_status getThreeAxisMagnetometerStatus(size_t sens_index) const override;
    yarp::dev::ReturnValue getThreeAxisMagnetometerName(size_t sens_index, std::string &name) const override;
    yarp::dev::ReturnValue getThreeAxisMagnetometerFrameName(size_t sens_index, std::string &frameName) const override;
    yarp::dev::ReturnValue getThreeAxisMagnetometerMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;

    /* IPositionSensors methods */
    yarp::dev::ReturnValue getNrOfPositionSensors(size_t& num) const override;
    yarp::dev::MAS_status getPositionSensorStatus(size_t sens_index) const override;
    yarp::dev::ReturnValue getPositionSensorName(size_t sens_index, std::string& name) const override;
    yarp::dev::ReturnValue getPositionSensorFrameName(size_t sens_index, std::string& frameName) const override;
    yarp::dev::ReturnValue getPositionSensorMeasure(size_t sens_index, yarp::sig::Vector& xyz, double& timestamp) const override;

    /* ILinearVelocitySensors methods */
    yarp::dev::ReturnValue getNrOfLinearVelocitySensors(size_t& num) const override;
    yarp::dev::MAS_status getLinearVelocitySensorStatus(size_t sens_index) const override;
    yarp::dev::ReturnValue getLinearVelocitySensorName(size_t sens_index, std::string& name) const override;
    yarp::dev::ReturnValue getLinearVelocitySensorFrameName(size_t sens_index, std::string& frameName) const override;
    yarp::dev::ReturnValue getLinearVelocitySensorMeasure(size_t sens_index, yarp::sig::Vector& xyz, double& timestamp) const override;

    /* IOrientationSensors methods */
    yarp::dev::ReturnValue getNrOfOrientationSensors(size_t& num) const override;
    yarp::dev::MAS_status getOrientationSensorStatus(size_t sens_index) const override;
    yarp::dev::ReturnValue getOrientationSensorName(size_t sens_index, std::string &name) const override;
    yarp::dev::ReturnValue getOrientationSensorFrameName(size_t sens_index, std::string &frameName) const override;
    yarp::dev::ReturnValue getOrientationSensorMeasureAsRollPitchYaw(size_t sens_index, yarp::sig::Vector& rpy, double& timestamp) const override;

    /* ITemperatureSensors methods */
    yarp::dev::ReturnValue getNrOfTemperatureSensors(size_t& num) const override;
    yarp::dev::MAS_status getTemperatureSensorStatus(size_t sens_index) const override;
    yarp::dev::ReturnValue getTemperatureSensorName(size_t sens_index, std::string &name) const override;
    yarp::dev::ReturnValue getTemperatureSensorFrameName(size_t sens_index, std::string &frameName) const override;
    yarp::dev::ReturnValue getTemperatureSensorMeasure(size_t sens_index, double& out, double& timestamp) const override;
    yarp::dev::ReturnValue getTemperatureSensorMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;

    /* ISixAxisForceTorqueSensors */
    yarp::dev::ReturnValue getNrOfSixAxisForceTorqueSensors(size_t& num) const override;
    yarp::dev::MAS_status getSixAxisForceTorqueSensorStatus(size_t sens_index) const override;
    yarp::dev::ReturnValue getSixAxisForceTorqueSensorName(size_t sens_index, std::string &name) const override;
    yarp::dev::ReturnValue getSixAxisForceTorqueSensorFrameName(size_t sens_index, std::string &frame) const override;
    yarp::dev::ReturnValue getSixAxisForceTorqueSensorMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;

    /* IContactLoadCellArrays */
    yarp::dev::ReturnValue getNrOfContactLoadCellArrays(size_t& num) const override;
    yarp::dev::MAS_status getContactLoadCellArrayStatus(size_t sens_index) const override;
    yarp::dev::ReturnValue getContactLoadCellArrayName(size_t sens_index, std::string &name) const override;
    yarp::dev::ReturnValue getContactLoadCellArrayMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;
    size_t getContactLoadCellArraySize(size_t sens_index) const override;

    /* IEncoderArrays */
    yarp::dev::ReturnValue getNrOfEncoderArrays(size_t& num) const override;
    yarp::dev::MAS_status getEncoderArrayStatus(size_t sens_index) const override;
    yarp::dev::ReturnValue getEncoderArrayName(size_t sens_index, std::string &name) const override;
    yarp::dev::ReturnValue getEncoderArrayMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;
    size_t getEncoderArraySize(size_t sens_index) const override;

    /* ISkinPatches */
    yarp::dev::ReturnValue getNrOfSkinPatches(size_t& num) const override;
    yarp::dev::MAS_status getSkinPatchStatus(size_t sens_index) const override;
    yarp::dev::ReturnValue getSkinPatchName(size_t sens_index, std::string &name) const override;
    yarp::dev::ReturnValue getSkinPatchMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;
    size_t getSkinPatchSize(size_t sens_index) const override;
};

#endif

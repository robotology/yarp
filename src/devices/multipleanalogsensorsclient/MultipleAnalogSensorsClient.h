/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_MULTIPLEANALOGSENSORSCLIENT_MULTIPLEANALOGSENSORSCLIENT_H
#define YARP_DEV_MULTIPLEANALOGSENSORSCLIENT_MULTIPLEANALOGSENSORSCLIENT_H

#include <yarp/dev/MultipleAnalogSensorsInterfaces.h>

#include "MultipleAnalogSensorsMetadata.h"
#include "SensorStreamingData.h"

#include <yarp/os/Network.h>

#include <mutex>

namespace yarp {
    namespace dev {
        class SensorStreamingDataInputPort;
        class MultipleAnalogSensorsClient;
    }
}

class yarp::dev::SensorStreamingDataInputPort: public yarp::os::BufferedPort<SensorStreamingData>
{
public:
    SensorStreamingData receivedData;
    mutable yarp::dev::MAS_status status{yarp::dev::MAS_WAITING_FOR_FIRST_READ};
    mutable std::mutex dataMutex;
    double timeoutInSeconds{0.01};
    double lastTimeStampReadInSeconds{0.0};

    using yarp::os::BufferedPort<SensorStreamingData>::onRead;
    virtual void onRead(SensorStreamingData &v) override;
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
* | Parameter name | SubParameter   | Type    | Units          | Default Value | Required                    | Description                                                       | Notes |
* |:--------------:|:--------------:|:-------:|:--------------:|:-------------:|:--------------------------: |:-----------------------------------------------------------------:|:-----:|
* | remote         |       -        | string  | -              |   -           | Yes          | Prefix of the ports to which to connect, opened by MultipleAnalogSensorsServer device. | |
* | local          |       -        | string  | -              |   -           | Yes          | Port prefix of the ports openened by this device.  |                                     |
* | timeout        |       -        | double  | seconds        | 0.01          | No           | Timeout after which the device reports an error if no measurement was received. |        |
*
*/
class yarp::dev::MultipleAnalogSensorsClient: public DeviceDriver,
                                              public IThreeAxisGyroscopes,
                                              public IThreeAxisLinearAccelerometers,
                                              public IThreeAxisMagnetometers,
                                              public IOrientationSensors,
                                              public ITemperatureSensors,
                                              public ISixAxisForceTorqueSensors, 
                                              public IContactLoadCellArrays,
                                              public IEncoderArrays,
                                              public ISkinPatches
{
    SensorStreamingDataInputPort m_streamingPort;
    yarp::os::Port m_rpcPort;
    std::string m_localRPCPortName;
    std::string m_localStreamingPortName;
    std::string m_remoteRPCPortName;
    std::string m_remoteStreamingPortName;
    bool m_RPCConnectionActive{false};
    bool m_StreamingConnectionActive{false};

    MultipleAnalogSensorsMetadata m_RPCInterface;
    SensorRPCData m_sensorsMetadata;

    size_t genericGetNrOfSensors(const std::vector<SensorMetadata>& metadataVector) const;
    MAS_status genericGetStatus() const;
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
    virtual size_t getNrOfThreeAxisGyroscopes() const override;
    virtual yarp::dev::MAS_status getThreeAxisGyroscopeStatus(size_t sens_index) const override;
    virtual bool getThreeAxisGyroscopeName(size_t sens_index, std::string &name) const override;
    virtual bool getThreeAxisGyroscopeFrameName(size_t sens_index, std::string &frameName) const override;
    virtual bool getThreeAxisGyroscopeMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;

    /* IThreeAxisLinearAccelerometers methods */
    virtual size_t getNrOfThreeAxisLinearAccelerometers() const override;
    virtual yarp::dev::MAS_status getThreeAxisLinearAccelerometerStatus(size_t sens_index) const override;
    virtual bool getThreeAxisLinearAccelerometerName(size_t sens_index, std::string &name) const override;
    virtual bool getThreeAxisLinearAccelerometerFrameName(size_t sens_index, std::string &frameName) const override;
    virtual bool getThreeAxisLinearAccelerometerMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;

    /* IThreeAxisMagnetometers methods */
    virtual size_t getNrOfThreeAxisMagnetometers() const override;
    virtual yarp::dev::MAS_status getThreeAxisMagnetometerStatus(size_t sens_index) const override;
    virtual bool getThreeAxisMagnetometerName(size_t sens_index, std::string &name) const override;
    virtual bool getThreeAxisMagnetometerFrameName(size_t sens_index, std::string &frameName) const override;
    virtual bool getThreeAxisMagnetometerMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;

    /* IOrientationSensors methods */
    virtual size_t getNrOfOrientationSensors() const override;
    virtual yarp::dev::MAS_status getOrientationSensorStatus(size_t sens_index) const override;
    virtual bool getOrientationSensorName(size_t sens_index, std::string &name) const override;
    virtual bool getOrientationSensorFrameName(size_t sens_index, std::string &frameName) const override;
    virtual bool getOrientationSensorMeasureAsRollPitchYaw(size_t sens_index, yarp::sig::Vector& rpy, double& timestamp) const override;

    /* ITemperatureSensors methods */
    virtual size_t getNrOfTemperatureSensors() const override;
    virtual yarp::dev::MAS_status getTemperatureSensorStatus(size_t sens_index) const override;
    virtual bool getTemperatureSensorName(size_t sens_index, std::string &name) const override;
    virtual bool getTemperatureSensorFrameName(size_t sens_index, std::string &frameName) const override;
    virtual bool getTemperatureSensorMeasure(size_t sens_index, double& out, double& timestamp) const override;
    virtual bool getTemperatureSensorMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;

    /* ISixAxisForceTorqueSensors */
    virtual size_t getNrOfSixAxisForceTorqueSensors() const override;
    virtual yarp::dev::MAS_status getSixAxisForceTorqueSensorStatus(size_t sens_index) const override;
    virtual bool getSixAxisForceTorqueSensorName(size_t sens_index, std::string &name) const override;
    virtual bool getSixAxisForceTorqueSensorFrameName(size_t sens_index, std::string &frame) const override;
    virtual bool getSixAxisForceTorqueSensorMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;

    /* IContactLoadCellArrays */
    virtual size_t getNrOfContactLoadCellArrays() const override;
    virtual yarp::dev::MAS_status getContactLoadCellArrayStatus(size_t sens_index) const override;
    virtual bool getContactLoadCellArrayName(size_t sens_index, std::string &name) const override;
    virtual bool getContactLoadCellArrayMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;
    virtual size_t getContactLoadCellArraySize(size_t sens_index) const override;

    /* IEncoderArrays */
    virtual size_t getNrOfEncoderArrays() const override;
    virtual yarp::dev::MAS_status getEncoderArrayStatus(size_t sens_index) const override;
    virtual bool getEncoderArrayName(size_t sens_index, std::string &name) const override;
    virtual bool getEncoderArrayMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;
    virtual size_t getEncoderArraySize(size_t sens_index) const override;

    /* ISkinPatches */
    virtual size_t getNrOfSkinPatches() const override;
    virtual yarp::dev::MAS_status getSkinPatchStatus(size_t sens_index) const override;
    virtual bool getSkinPatchName(size_t sens_index, std::string &name) const override;
    virtual bool getSkinPatchMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;
    virtual size_t getSkinPatchSize(size_t sens_index) const override;
};

#endif 

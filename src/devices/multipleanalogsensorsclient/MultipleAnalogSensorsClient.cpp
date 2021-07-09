/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "MultipleAnalogSensorsClient.h"

#include "SensorStreamingData.h"
#include "MultipleAnalogSensorsMetadata.h"

#include <yarp/os/LogComponent.h>

namespace {
YARP_LOG_COMPONENT(MULTIPLEANALOGSENSORSCLIENT, "yarp.device.multipleanalogsensorsclient")
}

void SensorStreamingDataInputPort::onRead(SensorStreamingData& v)
{
    std::lock_guard<std::mutex> guard(dataMutex);
    receivedData = v;
    lastTimeStampReadInSeconds = yarp::os::Time::now();
    status = yarp::dev::MAS_OK;
}

void SensorStreamingDataInputPort::updateTimeoutStatus() const
{
    if (status == yarp::dev::MAS_OK)
    {
        double now = yarp::os::Time::now();
        if ((now-lastTimeStampReadInSeconds) > this->timeoutInSeconds)
        {
            yCError(MULTIPLEANALOGSENSORSCLIENT,
                    "No data received in the last %lf seconds, timeout enabled.",
                    (now-lastTimeStampReadInSeconds));
            status = yarp::dev::MAS_TIMEOUT;
        }
    }
}

bool MultipleAnalogSensorsClient::open(yarp::os::Searchable& config)
{
    m_externalConnection = config.check("externalConnection",yarp::os::Value(false)).asBool();
    if (!config.check("remote"))
    {
        yCError(MULTIPLEANALOGSENSORSCLIENT, "Missing name parameter, exiting.");
        return false;
    }

    if (!config.check("local"))
    {
        yCError(MULTIPLEANALOGSENSORSCLIENT, "Missing local parameter, exiting.");
        return false;
    }

    if (config.check("timeout") && !(config.find("timeout").isFloat64()))
    {
        yCError(MULTIPLEANALOGSENSORSCLIENT, "'timeout' parameter is present but is not double, exiting.");
        return false;
    }

    std::string remote = config.find("remote").asString();
    std::string local = config.find("local").asString();

    // Optional timeout parameter
    m_streamingPort.timeoutInSeconds = config.check("timeout", yarp::os::Value(0.01), "Timeout parameter").asFloat64();

    m_localRPCPortName = local + "/rpc:i";
    m_localStreamingPortName = local + "/measures:i";
    m_remoteRPCPortName = remote + "/rpc:o";
    m_remoteStreamingPortName = remote + "/measures:o";

    // TODO(traversaro) : as soon as the method for checking port names validity
    //                    are available in YARP ( https://github.com/robotology/yarp/pull/1508 ) add some checks

    // Open ports
    bool ok = m_rpcPort.open(m_localRPCPortName);
    if (!ok)
    {
        yCError(MULTIPLEANALOGSENSORSCLIENT,
                "Failure to open the port %s.",
                m_localRPCPortName.c_str());
        close();
        return false;
    }

    ok = m_streamingPort.open(m_localStreamingPortName);
    m_streamingPort.useCallback();
    if (!ok)
    {
        yCError(MULTIPLEANALOGSENSORSCLIENT,
                "Failure to open the port %s.",
                m_localStreamingPortName.c_str());
        close();
        return false;
    }

    // Connect ports
    if (!m_externalConnection) {
        ok = yarp::os::Network::connect(m_localRPCPortName, m_remoteRPCPortName);
        if (!ok) {
            yCError(MULTIPLEANALOGSENSORSCLIENT,
                    "Failure connecting port %s to %s.",
                    m_localRPCPortName.c_str(),
                    m_remoteRPCPortName.c_str());
            yCError(MULTIPLEANALOGSENSORSCLIENT, "Check that the specified MultipleAnalogSensorsServer is up.");
            close();
            return false;
        }
        m_RPCConnectionActive = true;

        ok = yarp::os::Network::connect(m_remoteStreamingPortName, m_localStreamingPortName);
        if (!ok) {
            yCError(MULTIPLEANALOGSENSORSCLIENT,
                    "Failure connecting port %s to %s.",
                    m_remoteStreamingPortName.c_str(),
                    m_localStreamingPortName.c_str());
            yCError(MULTIPLEANALOGSENSORSCLIENT, "Check that the specified MultipleAnalogSensorsServer is up.");
            close();
            return false;
        }
        m_StreamingConnectionActive = true;

        // Once the connection is active, we just the metadata only once
        ok = m_RPCInterface.yarp().attachAsClient(m_rpcPort);
        if (!ok) {
            yCError(MULTIPLEANALOGSENSORSCLIENT, "Failure opening Thrift-based RPC interface.");
            return false;
        }

        // TODO(traversaro): there is a limitation on the thrift-generated
        // YARP structures related to how to get connection errors during the call
        // If this is ever solved at YARP level, we should properly handle errors
        // here
        m_sensorsMetadata = m_RPCInterface.getMetadata();

    }

    return true;
}

bool MultipleAnalogSensorsClient::close()
{
    if (m_StreamingConnectionActive)
    {
        yarp::os::Network::disconnect(m_remoteStreamingPortName, m_localStreamingPortName);
    }
    if (m_RPCConnectionActive)
    {
        yarp::os::Network::disconnect(m_localRPCPortName, m_remoteRPCPortName);
    }

    m_streamingPort.close();
    m_rpcPort.close();

    return true;
}

size_t MultipleAnalogSensorsClient::genericGetNrOfSensors(const std::vector<SensorMetadata>& metadataVector,
                                                          const SensorMeasurements& measurementsVector) const
{
    if (!m_externalConnection) {
        return metadataVector.size();
    } else {
        std::lock_guard<std::mutex>  guard(m_streamingPort.dataMutex);
        m_streamingPort.updateTimeoutStatus();
        if (m_streamingPort.status == yarp::dev::MAS_OK)  {
            return measurementsVector.measurements.size();
        } else {
            return 0;
        }
    }
}

yarp::dev::MAS_status MultipleAnalogSensorsClient::genericGetStatus() const
{
    std::lock_guard<std::mutex>  guard(m_streamingPort.dataMutex);
    m_streamingPort.updateTimeoutStatus();
    return m_streamingPort.status;
}

bool MultipleAnalogSensorsClient::genericGetName(const  std::vector<SensorMetadata>& metadataVector, const std::string& tag,
                                                   size_t sens_index, std::string& name) const
{
    if (m_externalConnection) {
        yCError(MULTIPLEANALOGSENSORSCLIENT,
                "Missing metadata, the device has been configured with the option"
                "externalConnection set to true.");
        return false;
    }
    if (sens_index >= metadataVector.size())
    {
        yCError(MULTIPLEANALOGSENSORSCLIENT,
                "No sensor of type %s with index %lu (nr of sensors: %lu).",
                tag.c_str(),
                sens_index,
                metadataVector.size());
        return false;
    }

    name = metadataVector[sens_index].name;
    return true;
}

bool MultipleAnalogSensorsClient::genericGetFrameName(const  std::vector<SensorMetadata>& metadataVector, const std::string& tag,
                                                      size_t sens_index, std::string& frameName) const
{
    if (m_externalConnection) {
        yCError(MULTIPLEANALOGSENSORSCLIENT,
                "Missing metadata, the device has been configured with the option"
                "externalConnection set to true.");
        return false;
    }
    if (sens_index >= metadataVector.size())
    {
        yCError(MULTIPLEANALOGSENSORSCLIENT,
                "No sensor of type %s with index %lu (nr of sensors: %lu).",
                tag.c_str(),
                sens_index,
                metadataVector.size());
        return false;
    }

    frameName = metadataVector[sens_index].frameName;
    return true;
}

bool MultipleAnalogSensorsClient::genericGetMeasure(const std::vector<SensorMetadata>& metadataVector, const std::string& tag,
                                                    const SensorMeasurements& measurementsVector,
                                                    size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{

    std::lock_guard<std::mutex> guard(m_streamingPort.dataMutex);
    m_streamingPort.updateTimeoutStatus();
    if (m_streamingPort.status != yarp::dev::MAS_OK)
    {
        yCError(MULTIPLEANALOGSENSORSCLIENT,
                "Sensor of type %s with index %lu has non-MAS_OK status.",
                tag.c_str(),
                sens_index);
        return false;
    }

    if (m_streamingPort.status != (sens_index >= measurementsVector.measurements.size()))
    {
        yCError(MULTIPLEANALOGSENSORSCLIENT,
                "No sensor of type %s with index %lu (nr of sensors: %lu).",
                tag.c_str(),
                sens_index,
                metadataVector.size());
        return false;
    }

    if (!m_externalConnection)
        assert(metadataVector.size() == measurementsVector.measurements.size());

    timestamp = measurementsVector.measurements[sens_index].timestamp;
    out = measurementsVector.measurements[sens_index].measurement;

    return true;
}

size_t MultipleAnalogSensorsClient::genericGetSize(const std::vector<SensorMetadata>& metadataVector,
                                                   const std::string& tag, const SensorMeasurements& measurementsVector, size_t sens_index) const
{
    std::lock_guard<std::mutex> guard(m_streamingPort.dataMutex);
    if (m_streamingPort.status != yarp::dev::MAS_OK)
    {
        yCError(MULTIPLEANALOGSENSORSCLIENT, "No data received, no information on the size of the specified sensor.");
        return 0;
    }


    if (sens_index >= measurementsVector.measurements.size())
    {
        yCError(MULTIPLEANALOGSENSORSCLIENT,
                "No sensor of type %s with index %lu (nr of sensors: %lu).",
                tag.c_str(),
                sens_index,
                metadataVector.size());
        return 0;
    }

    return measurementsVector.measurements[sens_index].measurement.size();
}

/*
All the sensor specific methods (excluding the IOrientationSensor and the ISkinPatches) are just
an instantiation of the following template (note: we avoid code generation for the sake of readability):

{{SensorTag}} : ThreeAxisGyroscopes, ThreeAxisLinearAccelerometers, etc
{{SensorSingular}} : ThreeAxisGyroscope, ThreeAxisLinearAccelerometer, etc

size_t MultipleAnalogSensorsClient::getNrOf{{SensorTag}}() const
{
    return genericGetNrOfSensors(m_sensorsMetadata.{{SensorTag}});
}

MAS_status MultipleAnalogSensorsClient::get{{SensorSingular}}Status(size_t sens_index) const
{
    return genericGetStatus();
}

bool MultipleAnalogSensorsClient::get{{SensorSingular}}Name(size_t sens_index, std::string& name) const
{
    return genericGetName(m_sensorsMetadata.{{SensorTag}}, "{{SensorTag}}", sens_index, name);
}

bool MultipleAnalogSensorsClient::get{{SensorSingular}}Measure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{
    return genericGetMeasure(m_sensorsMetadata.{{SensorTag}}, "{{SensorTag}}",
                             m_streamingPort.receivedData.{{SensorTag}}, sens_index, out, timestamp);
}

For the sensors (EncoderArray and SkinPatch) of which the measurements can change size, we also have:
size_t MultipleAnalogSensorsClient::get{{SensorSingular}}Size(size_t sens_index) const
{
    return genericGetSize({{SensorTag}}, sens_index, m_i{{SensorTag}}, &I{{SensorTag}}::get{{SensorTag}}Size);
}

*/

size_t MultipleAnalogSensorsClient::getNrOfThreeAxisGyroscopes() const
{
    return genericGetNrOfSensors(m_sensorsMetadata.ThreeAxisGyroscopes,
                                 m_streamingPort.receivedData.ThreeAxisGyroscopes);
}

yarp::dev::MAS_status MultipleAnalogSensorsClient::getThreeAxisGyroscopeStatus(size_t sens_index) const
{
    return genericGetStatus();
}

bool MultipleAnalogSensorsClient::getThreeAxisGyroscopeName(size_t sens_index, std::string &name) const
{
    return genericGetName(m_sensorsMetadata.ThreeAxisGyroscopes, "ThreeAxisGyroscopes", sens_index, name);
}

bool MultipleAnalogSensorsClient::getThreeAxisGyroscopeFrameName(size_t sens_index, std::string &frameName) const
{
    return genericGetFrameName(m_sensorsMetadata.ThreeAxisGyroscopes, "ThreeAxisGyroscopes", sens_index, frameName);
}

bool MultipleAnalogSensorsClient::getThreeAxisGyroscopeMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{
    return genericGetMeasure(m_sensorsMetadata.ThreeAxisGyroscopes, "ThreeAxisGyroscopes",
                             m_streamingPort.receivedData.ThreeAxisGyroscopes, sens_index, out, timestamp);
}

size_t MultipleAnalogSensorsClient::getNrOfThreeAxisLinearAccelerometers() const
{
    return genericGetNrOfSensors(m_sensorsMetadata.ThreeAxisLinearAccelerometers,
                                 m_streamingPort.receivedData.ThreeAxisLinearAccelerometers);
}

yarp::dev::MAS_status MultipleAnalogSensorsClient::getThreeAxisLinearAccelerometerStatus(size_t sens_index) const
{
    return genericGetStatus();
}

bool MultipleAnalogSensorsClient::getThreeAxisLinearAccelerometerName(size_t sens_index, std::string &name) const
{
    return genericGetName(m_sensorsMetadata.ThreeAxisLinearAccelerometers, "ThreeAxisLinearAccelerometers", sens_index, name);
}

bool MultipleAnalogSensorsClient::getThreeAxisLinearAccelerometerFrameName(size_t sens_index, std::string &frameName) const
{
    return genericGetFrameName(m_sensorsMetadata.ThreeAxisLinearAccelerometers, "ThreeAxisLinearAccelerometers", sens_index, frameName);
}

bool MultipleAnalogSensorsClient::getThreeAxisLinearAccelerometerMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{
    return genericGetMeasure(m_sensorsMetadata.ThreeAxisLinearAccelerometers, "ThreeAxisLinearAccelerometers",
                             m_streamingPort.receivedData.ThreeAxisLinearAccelerometers, sens_index, out, timestamp);
}

size_t MultipleAnalogSensorsClient::getNrOfThreeAxisMagnetometers() const
{
    return genericGetNrOfSensors(m_sensorsMetadata.ThreeAxisMagnetometers,
                                 m_streamingPort.receivedData.ThreeAxisMagnetometers);
}

yarp::dev::MAS_status MultipleAnalogSensorsClient::getThreeAxisMagnetometerStatus(size_t sens_index) const
{
    return genericGetStatus();
}

bool MultipleAnalogSensorsClient::getThreeAxisMagnetometerName(size_t sens_index, std::string& name) const
{
    return genericGetName(m_sensorsMetadata.ThreeAxisMagnetometers, "ThreeAxisMagnetometers", sens_index, name);
}

bool MultipleAnalogSensorsClient::getThreeAxisMagnetometerFrameName(size_t sens_index, std::string &frameName) const
{
    return genericGetFrameName(m_sensorsMetadata.ThreeAxisMagnetometers, "ThreeAxisMagnetometers", sens_index, frameName);
}

bool MultipleAnalogSensorsClient::getThreeAxisMagnetometerMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{
    return genericGetMeasure(m_sensorsMetadata.ThreeAxisMagnetometers, "ThreeAxisMagnetometers",
                             m_streamingPort.receivedData.ThreeAxisMagnetometers, sens_index, out, timestamp);
}

size_t MultipleAnalogSensorsClient::getNrOfOrientationSensors() const
{
    return genericGetNrOfSensors(m_sensorsMetadata.OrientationSensors,
                                 m_streamingPort.receivedData.OrientationSensors);
}

yarp::dev::MAS_status MultipleAnalogSensorsClient::getOrientationSensorStatus(size_t sens_index) const
{
    return genericGetStatus();
}

bool MultipleAnalogSensorsClient::getOrientationSensorName(size_t sens_index, std::string& name) const
{
    return genericGetName(m_sensorsMetadata.OrientationSensors, "OrientationSensors", sens_index, name);
}

bool MultipleAnalogSensorsClient::getOrientationSensorFrameName(size_t sens_index, std::string &frameName) const
{
    return genericGetFrameName(m_sensorsMetadata.OrientationSensors, "OrientationSensors", sens_index, frameName);
}

bool MultipleAnalogSensorsClient::getOrientationSensorMeasureAsRollPitchYaw(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{
    return genericGetMeasure(m_sensorsMetadata.OrientationSensors, "OrientationSensors",
                             m_streamingPort.receivedData.OrientationSensors, sens_index, out, timestamp);
}

size_t MultipleAnalogSensorsClient::getNrOfPositionSensors() const
{
    return genericGetNrOfSensors(m_sensorsMetadata.PositionSensors,
                                 m_streamingPort.receivedData.PositionSensors);
}

yarp::dev::MAS_status MultipleAnalogSensorsClient::getPositionSensorStatus(size_t sens_index) const
{
    return genericGetStatus();
}

bool MultipleAnalogSensorsClient::getPositionSensorName(size_t sens_index, std::string& name) const
{
    return genericGetName(m_sensorsMetadata.PositionSensors, "PositionSensors", sens_index, name);
}

bool MultipleAnalogSensorsClient::getPositionSensorFrameName(size_t sens_index, std::string& frameName) const
{
    return genericGetFrameName(m_sensorsMetadata.PositionSensors, "PositionSensors", sens_index, frameName);
}

bool MultipleAnalogSensorsClient::getPositionSensorMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{
    return genericGetMeasure(m_sensorsMetadata.PositionSensors, "PositionSensors", m_streamingPort.receivedData.PositionSensors, sens_index, out, timestamp);
}

size_t MultipleAnalogSensorsClient::getNrOfTemperatureSensors() const
{
    return genericGetNrOfSensors(m_sensorsMetadata.TemperatureSensors,
                                 m_streamingPort.receivedData.TemperatureSensors);
}

yarp::dev::MAS_status MultipleAnalogSensorsClient::getTemperatureSensorStatus(size_t sens_index) const
{
    return genericGetStatus();
}

bool MultipleAnalogSensorsClient::getTemperatureSensorName(size_t sens_index, std::string& name) const
{
    return genericGetName(m_sensorsMetadata.TemperatureSensors, "TemperatureSensors", sens_index, name);
}

bool MultipleAnalogSensorsClient::getTemperatureSensorFrameName(size_t sens_index, std::string &frameName) const
{
    return genericGetFrameName(m_sensorsMetadata.TemperatureSensors, "TemperatureSensors", sens_index, frameName);
}

bool MultipleAnalogSensorsClient::getTemperatureSensorMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{
    return genericGetMeasure(m_sensorsMetadata.TemperatureSensors, "TemperatureSensors",
                             m_streamingPort.receivedData.TemperatureSensors, sens_index, out, timestamp);
}

bool MultipleAnalogSensorsClient::getTemperatureSensorMeasure(size_t sens_index, double& out, double& timestamp) const
{
    yarp::sig::Vector dummy(1);
    bool ok = this->getTemperatureSensorMeasure(sens_index, dummy, timestamp);
    out = dummy[0];
    return ok;
}

size_t MultipleAnalogSensorsClient::getNrOfSixAxisForceTorqueSensors() const
{
    return genericGetNrOfSensors(m_sensorsMetadata.SixAxisForceTorqueSensors,
                                 m_streamingPort.receivedData.SixAxisForceTorqueSensors);
}

yarp::dev::MAS_status MultipleAnalogSensorsClient::getSixAxisForceTorqueSensorStatus(size_t sens_index) const
{
    return genericGetStatus();
}

bool MultipleAnalogSensorsClient::getSixAxisForceTorqueSensorName(size_t sens_index, std::string& name) const
{
    return genericGetName(m_sensorsMetadata.SixAxisForceTorqueSensors, "SixAxisForceTorqueSensors", sens_index, name);
}

bool MultipleAnalogSensorsClient::getSixAxisForceTorqueSensorFrameName(size_t sens_index, std::string &frameName) const
{
    return genericGetFrameName(m_sensorsMetadata.SixAxisForceTorqueSensors, "SixAxisForceTorqueSensors", sens_index, frameName);
}

bool MultipleAnalogSensorsClient::getSixAxisForceTorqueSensorMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{
    return genericGetMeasure(m_sensorsMetadata.SixAxisForceTorqueSensors, "SixAxisForceTorqueSensors",
                             m_streamingPort.receivedData.SixAxisForceTorqueSensors, sens_index, out, timestamp);
}

size_t MultipleAnalogSensorsClient::getNrOfContactLoadCellArrays() const
{
    return genericGetNrOfSensors(m_sensorsMetadata.ContactLoadCellArrays,
                                 m_streamingPort.receivedData.ContactLoadCellArrays);
}

yarp::dev::MAS_status MultipleAnalogSensorsClient::getContactLoadCellArrayStatus(size_t sens_index) const
{
    return genericGetStatus();
}

bool MultipleAnalogSensorsClient::getContactLoadCellArrayName(size_t sens_index, std::string& name) const
{
    return genericGetName(m_sensorsMetadata.ContactLoadCellArrays, "ContactLoadCellArrays", sens_index, name);
}

bool MultipleAnalogSensorsClient::getContactLoadCellArrayMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{
    return genericGetMeasure(m_sensorsMetadata.ContactLoadCellArrays, "ContactLoadCellArrays",
                             m_streamingPort.receivedData.ContactLoadCellArrays, sens_index, out, timestamp);
}

size_t MultipleAnalogSensorsClient::getContactLoadCellArraySize(size_t sens_index) const
{
    return genericGetSize(m_sensorsMetadata.ContactLoadCellArrays, "ContactLoadCellArrays",
                          m_streamingPort.receivedData.ContactLoadCellArrays, sens_index);
}

size_t MultipleAnalogSensorsClient::getNrOfEncoderArrays() const
{
    return genericGetNrOfSensors(m_sensorsMetadata.EncoderArrays,
                                 m_streamingPort.receivedData.EncoderArrays);
}

yarp::dev::MAS_status MultipleAnalogSensorsClient::getEncoderArrayStatus(size_t sens_index) const
{
    return genericGetStatus();
}

bool MultipleAnalogSensorsClient::getEncoderArrayName(size_t sens_index, std::string& name) const
{
    return genericGetName(m_sensorsMetadata.EncoderArrays, "EncoderArrays", sens_index, name);
}

bool MultipleAnalogSensorsClient::getEncoderArrayMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{
    return genericGetMeasure(m_sensorsMetadata.EncoderArrays, "EncoderArrays",
                             m_streamingPort.receivedData.EncoderArrays, sens_index, out, timestamp);
}

size_t MultipleAnalogSensorsClient::getEncoderArraySize(size_t sens_index) const
{
    return genericGetSize(m_sensorsMetadata.EncoderArrays, "EncoderArrays",
                          m_streamingPort.receivedData.EncoderArrays, sens_index);
}

size_t MultipleAnalogSensorsClient::getNrOfSkinPatches() const
{
    return genericGetNrOfSensors(m_sensorsMetadata.SkinPatches,
                                 m_streamingPort.receivedData.SkinPatches);
}

yarp::dev::MAS_status MultipleAnalogSensorsClient::getSkinPatchStatus(size_t sens_index) const
{
    return genericGetStatus();
}

bool MultipleAnalogSensorsClient::getSkinPatchName(size_t sens_index, std::string& name) const
{
    return genericGetName(m_sensorsMetadata.SkinPatches, "SkinPatches", sens_index, name);
}

bool MultipleAnalogSensorsClient::getSkinPatchMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{
    return genericGetMeasure(m_sensorsMetadata.SkinPatches, "SkinPatches",
                             m_streamingPort.receivedData.SkinPatches, sens_index, out, timestamp);
}

size_t MultipleAnalogSensorsClient::getSkinPatchSize(size_t sens_index) const
{
    return genericGetSize(m_sensorsMetadata.SkinPatches, "SkinPatches",
                          m_streamingPort.receivedData.SkinPatches, sens_index);
}

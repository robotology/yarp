/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "SensorStreamingData.h"
#include "MultipleAnalogSensorsMetadata.h"

#include "MultipleAnalogSensorsServer.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>


MultipleAnalogSensorsServer::MultipleAnalogSensorsServer() :
        PeriodicThread(0.02)
{
}

MultipleAnalogSensorsServer::~MultipleAnalogSensorsServer() = default;

bool MultipleAnalogSensorsServer::open(yarp::os::Searchable& config)
{
    if (!config.check("name"))
    {
        yError("MultipleAnalogSensorsServer: missing name parameter, exiting.");
        return false;
    }

    if (!config.check("period"))
    {
        yError("MultipleAnalogSensorsClient: missing period parameter, exiting.");
        return false;
    }


    if (!config.find("period").isInt32())
    {
        yError("MultipleAnalogSensorsClient: period parameter is present but it is not an integer, exiting.");
        return false;
    }

    m_periodInS = config.find("period").asInt32() / 1000.0;

    if (m_periodInS <= 0)
    {
        yError("MultipleAnalogSensorsClient: period parameter is present (%f) but it is not a positive integer, exiting.", m_periodInS);
        return false;
    }

    std::string name = config.find("name").asString();

    // Reserve a fair amount of elements
    // It would be great if yarp::sig::Vector had a reserve method
    m_buffer.resize(100);
    m_buffer.resize(0);

    // TODO(traversaro) Add port name validation when ready,
    // see https://github.com/robotology/yarp/pull/1508
    m_RPCPortName = name + "/rpc:o";
    m_streamingPortName = name + "/measures:o";

    return true;
}

bool MultipleAnalogSensorsServer::close()
{
    return this->detachAll();
}

// Note: as soon as we support only C++17, we can switch to using std::invoke
// See https://isocpp.org/wiki/faq/pointers-to-members#fnptr-vs-memfnptr-types
#define MAS_CALL_MEMBER_FN(object, ptrToMember)  ((*object).*(ptrToMember))

template<typename Interface>
bool MultipleAnalogSensorsServer::populateSensorsMetadata(Interface * wrappedDeviceInterface,
                                  std::vector<SensorMetadata>& metadataVector, const std::string& tag,
                                  size_t (Interface::*getNrOfSensorsMethodPtr)() const,
                                  bool (Interface::*getNameMethodPtr)(size_t, std::string&) const,
                                  bool (Interface::*getFrameNameMethodPtr)(size_t, std::string&) const)
{
    if (wrappedDeviceInterface)
    {
        size_t nrOfSensors = MAS_CALL_MEMBER_FN(wrappedDeviceInterface, getNrOfSensorsMethodPtr)();
        metadataVector.resize(nrOfSensors);
        for (size_t i=0; i < nrOfSensors; i++)
        {
            std::string sensorName;
            bool ok = MAS_CALL_MEMBER_FN(wrappedDeviceInterface, getNameMethodPtr)(i, sensorName);
            if (!ok)
            {
                yError() << "MultipleAnalogSensorsServer: Failure in reading name of sensor of type "
                         << tag.c_str() << " at index " << i << " .";
                return false;
            }
            std::string frameName;
            ok = MAS_CALL_MEMBER_FN(wrappedDeviceInterface, getFrameNameMethodPtr)(i, frameName);
            if (!ok)
            {
                yError() << "MultipleAnalogSensorsServer: Failure in reading frame name of sensor of type "
                         << tag.c_str() << " at index " << i << " .";
                return false;
            }

            metadataVector[i].name = sensorName;
            metadataVector[i].frameName = frameName;
            metadataVector[i].additionalMetadata = "";
        }

    }
    else
    {
        metadataVector.resize(0);
    }
    return true;
}

template<typename Interface>
bool MultipleAnalogSensorsServer::populateSensorsMetadataNoFrameName(Interface * wrappedDeviceInterface,
                                                                     std::vector<SensorMetadata>& metadataVector, const std::string& tag,
                                                                     size_t (Interface::*getNrOfSensorsMethodPtr)() const,
                                                                     bool (Interface::*getNameMethodPtr)(size_t, std::string&) const)
{
    if (wrappedDeviceInterface)
    {
        size_t nrOfSensors = MAS_CALL_MEMBER_FN(wrappedDeviceInterface, getNrOfSensorsMethodPtr)();
        metadataVector.resize(nrOfSensors);
        for (size_t i=0; i < nrOfSensors; i++)
        {
            std::string sensorName;
            bool ok = MAS_CALL_MEMBER_FN(wrappedDeviceInterface, getNameMethodPtr)(i, sensorName);
            if (!ok)
            {
                yError() << "MultipleAnalogSensorsServer: Failure in reading name of sensor of type "
                         << tag.c_str() << " at index " << i << " .";
                return false;
            }

            metadataVector[i].name = sensorName;
            metadataVector[i].frameName = "";
            metadataVector[i].additionalMetadata = "";
        }

    }
    else
    {
        metadataVector.resize(0);
    }
    return true;
}


bool MultipleAnalogSensorsServer::populateAllSensorsMetadata()
{
    bool ok = true;
    ok = ok && populateSensorsMetadata(m_iThreeAxisGyroscopes, m_sensorMetadata.ThreeAxisGyroscopes, "ThreeAxisGyroscopes",
                                       &yarp::dev::IThreeAxisGyroscopes::getNrOfThreeAxisGyroscopes,
                                       &yarp::dev::IThreeAxisGyroscopes::getThreeAxisGyroscopeName,
                                       &yarp::dev::IThreeAxisGyroscopes::getThreeAxisGyroscopeFrameName);
    ok = ok && populateSensorsMetadata(m_iThreeAxisLinearAccelerometers, m_sensorMetadata.ThreeAxisLinearAccelerometers, "ThreeAxisLinearAccelerometers",
                                       &yarp::dev::IThreeAxisLinearAccelerometers::getNrOfThreeAxisLinearAccelerometers,
                                       &yarp::dev::IThreeAxisLinearAccelerometers::getThreeAxisLinearAccelerometerName,
                                       &yarp::dev::IThreeAxisLinearAccelerometers::getThreeAxisLinearAccelerometerFrameName);
    ok = ok && populateSensorsMetadata(m_iThreeAxisMagnetometers, m_sensorMetadata.ThreeAxisMagnetometers, "ThreeAxisMagnetometers",
                                       &yarp::dev::IThreeAxisMagnetometers::getNrOfThreeAxisMagnetometers,
                                       &yarp::dev::IThreeAxisMagnetometers::getThreeAxisMagnetometerName,
                                       &yarp::dev::IThreeAxisMagnetometers::getThreeAxisMagnetometerFrameName);
    ok = ok && populateSensorsMetadata(m_iOrientationSensors, m_sensorMetadata.OrientationSensors, "OrientationSensors",
                                       &yarp::dev::IOrientationSensors::getNrOfOrientationSensors,
                                       &yarp::dev::IOrientationSensors::getOrientationSensorName,
                                       &yarp::dev::IOrientationSensors::getOrientationSensorFrameName);
    ok = ok && populateSensorsMetadata(m_iTemperatureSensors, m_sensorMetadata.TemperatureSensors, "TemperatureSensors",
                                       &yarp::dev::ITemperatureSensors::getNrOfTemperatureSensors,
                                       &yarp::dev::ITemperatureSensors::getTemperatureSensorName,
                                       &yarp::dev::ITemperatureSensors::getTemperatureSensorFrameName);
    ok = ok && populateSensorsMetadata(m_iSixAxisForceTorqueSensors, m_sensorMetadata.SixAxisForceTorqueSensors, "SixAxisForceTorqueSensors",
                                       &yarp::dev::ISixAxisForceTorqueSensors::getNrOfSixAxisForceTorqueSensors,
                                       &yarp::dev::ISixAxisForceTorqueSensors::getSixAxisForceTorqueSensorName,
                                       &yarp::dev::ISixAxisForceTorqueSensors::getSixAxisForceTorqueSensorFrameName);
    ok = ok && populateSensorsMetadataNoFrameName(m_iContactLoadCellArrays, m_sensorMetadata.ContactLoadCellArrays, "ContactLoadCellArrays",
                                       &yarp::dev::IContactLoadCellArrays::getNrOfContactLoadCellArrays,
                                       &yarp::dev::IContactLoadCellArrays::getContactLoadCellArrayName);
    ok = ok && populateSensorsMetadataNoFrameName(m_iEncoderArrays, m_sensorMetadata.EncoderArrays, "EncoderArrays",
                                       &yarp::dev::IEncoderArrays::getNrOfEncoderArrays,
                                       &yarp::dev::IEncoderArrays::getEncoderArrayName);
    ok = ok && populateSensorsMetadataNoFrameName(m_iSkinPatches, m_sensorMetadata.SkinPatches, "ISkinPatches",
                                       &yarp::dev::ISkinPatches::getNrOfSkinPatches,
                                       &yarp::dev::ISkinPatches::getSkinPatchName);

    return ok;
}

bool MultipleAnalogSensorsServer::attachAll(const yarp::dev::PolyDriverList& p)
{
    // Attach the device
    if (p.size() > 1)
    {
        yError("MultipleAnalogSensorsServer: this device only supports exposing a "
                 "single MultipleAnalogSensors device on YARP ports, but %d devices have been passed in attachAll.", p.size());
        yError("MultipleAnalogSensorsServer: please use the multipleanalogsensorsremapper device to combine several device in a new device.");
        detachAll();
        return false;
    }

    if (p.size() == 0)
    {
        yError("MultipleAnalogSensorsServer: no device passed to attachAll, please pass a device to expose on YARP ports.");
        return false;
    }

    yarp::dev::PolyDriver* poly = p[0]->poly;

    if (!poly)
    {
        yError("MultipleAnalogSensorsServer: null pointer passed to attachAll.");
        return false;
    }

    // View all the interfaces
    poly->view(m_iThreeAxisGyroscopes);
    poly->view(m_iThreeAxisLinearAccelerometers);
    poly->view(m_iThreeAxisMagnetometers);
    poly->view(m_iOrientationSensors);
    poly->view(m_iTemperatureSensors);
    poly->view(m_iSixAxisForceTorqueSensors);
    poly->view(m_iContactLoadCellArrays);
    poly->view(m_iEncoderArrays);
    poly->view(m_iSkinPatches);


    // Populate the RPC data to be served on the RPC port
    bool ok = populateAllSensorsMetadata();

    if(!ok)
    {
        detachAll();
        return false;
    }

    // Attach was successful, open the ports
    ok = m_streamingPort.open(m_streamingPortName);
    if (!ok)
    {
        yError("MultipleAnalogSensorsServer: failure in opening port named %s.", m_streamingPortName.c_str());
        detachAll();
        return false;
    }

    ok = this->yarp().attachAsServer(m_rpcPort);
    if (!ok)
    {
        yError("MultipleAnalogSensorsServer: failure in attaching RPC port to thrift RPC interface.");
        detachAll();
        return false;
    }

    ok = m_rpcPort.open(m_RPCPortName);
    if (!ok)
    {
        yError("MultipleAnalogSensorsServer: failure in opening port named %s.", m_RPCPortName.c_str());
        detachAll();
        return false;
    }

    // Set rate period
    ok = this->setPeriod(m_periodInS);
    ok = ok && this->start();

    return ok;
}

bool MultipleAnalogSensorsServer::detachAll()
{
    // Stop the thread on detach
    if (this->isRunning())
    {
        this->stop();
    }

    m_rpcPort.close();
    m_streamingPort.close();

    return true;
}

SensorRPCData MultipleAnalogSensorsServer::getMetadata()
{
    return m_sensorMetadata;
}

template<typename Interface>
bool MultipleAnalogSensorsServer::genericStreamData(Interface* wrappedDeviceInterface,
                                                    const std::vector< SensorMetadata >& metadataVector,
                                                    std::vector< SensorMeasurement >& streamingDataVector,
                                                    yarp::dev::MAS_status (Interface::*getStatusMethodPtr)(size_t) const,
                                                    bool (Interface::*getMeasureMethodPtr)(size_t, yarp::sig::Vector&, double&) const)
{
    if (wrappedDeviceInterface)
    {
        size_t nrOfSensors = metadataVector.size();
        streamingDataVector.resize(nrOfSensors);
        for (size_t i=0; i < nrOfSensors; i++)
        {
            yarp::sig::Vector& outputBuffer = streamingDataVector[i].measurement;
            double& outputTimestamp = streamingDataVector[i].timestamp;
            // TODO(traversaro): resize the buffer to the correct size
            MAS_CALL_MEMBER_FN(wrappedDeviceInterface, getMeasureMethodPtr)(i, outputBuffer, outputTimestamp);
            yarp::dev::MAS_status status = MAS_CALL_MEMBER_FN(wrappedDeviceInterface, getStatusMethodPtr)(i);
            if (status != yarp::dev::MAS_OK)
            {
                yError("MultipleAnalogSensorsServer: failure in reading data from sensor %s, no data will be sent on the port.",
                    m_sensorMetadata.ThreeAxisGyroscopes[i].name.c_str());
                return false;
            }
        }
    }

    return true;
}


void MultipleAnalogSensorsServer::run()
{
    SensorStreamingData& streamingData = m_streamingPort.prepare();

    bool ok = true;

    ok = ok && genericStreamData(m_iThreeAxisGyroscopes, m_sensorMetadata.ThreeAxisGyroscopes,
                                 streamingData.ThreeAxisGyroscopes.measurements,
                                 &yarp::dev::IThreeAxisGyroscopes::getThreeAxisGyroscopeStatus,
                                 &yarp::dev::IThreeAxisGyroscopes::getThreeAxisGyroscopeMeasure);

    ok = ok && genericStreamData(m_iThreeAxisLinearAccelerometers, m_sensorMetadata.ThreeAxisLinearAccelerometers,
                                 streamingData.ThreeAxisLinearAccelerometers.measurements,
                                 &yarp::dev::IThreeAxisLinearAccelerometers::getThreeAxisLinearAccelerometerStatus,
                                 &yarp::dev::IThreeAxisLinearAccelerometers::getThreeAxisLinearAccelerometerMeasure);

    ok = ok && genericStreamData(m_iThreeAxisMagnetometers, m_sensorMetadata.ThreeAxisMagnetometers,
                                 streamingData.ThreeAxisMagnetometers.measurements,
                                 &yarp::dev::IThreeAxisMagnetometers::getThreeAxisMagnetometerStatus,
                                 &yarp::dev::IThreeAxisMagnetometers::getThreeAxisMagnetometerMeasure);

    ok = ok && genericStreamData(m_iOrientationSensors, m_sensorMetadata.OrientationSensors,
                                 streamingData.OrientationSensors.measurements,
                                 &yarp::dev::IOrientationSensors::getOrientationSensorStatus,
                                 &yarp::dev::IOrientationSensors::getOrientationSensorMeasureAsRollPitchYaw);

    ok = ok && genericStreamData(m_iTemperatureSensors, m_sensorMetadata.TemperatureSensors,
                                 streamingData.TemperatureSensors.measurements,
                                 &yarp::dev::ITemperatureSensors::getTemperatureSensorStatus,
                                 &yarp::dev::ITemperatureSensors::getTemperatureSensorMeasure);

    ok = ok && genericStreamData(m_iSixAxisForceTorqueSensors, m_sensorMetadata.SixAxisForceTorqueSensors,
                                 streamingData.SixAxisForceTorqueSensors.measurements,
                                 &yarp::dev::ISixAxisForceTorqueSensors::getSixAxisForceTorqueSensorStatus,
                                 &yarp::dev::ISixAxisForceTorqueSensors::getSixAxisForceTorqueSensorMeasure);

    ok = ok && genericStreamData(m_iContactLoadCellArrays, m_sensorMetadata.ContactLoadCellArrays,
                                 streamingData.ContactLoadCellArrays.measurements,
                                 &yarp::dev::IContactLoadCellArrays::getContactLoadCellArrayStatus,
                                 &yarp::dev::IContactLoadCellArrays::getContactLoadCellArrayMeasure);

    ok = ok && genericStreamData(m_iEncoderArrays, m_sensorMetadata.EncoderArrays,
                                 streamingData.EncoderArrays.measurements,
                                 &yarp::dev::IEncoderArrays::getEncoderArrayStatus,
                                 &yarp::dev::IEncoderArrays::getEncoderArrayMeasure);

    ok = ok && genericStreamData(m_iSkinPatches, m_sensorMetadata.SkinPatches,
                                 streamingData.SkinPatches.measurements,
                                 &yarp::dev::ISkinPatches::getSkinPatchStatus,
                                 &yarp::dev::ISkinPatches::getSkinPatchMeasure);

    if (ok)
    {
        m_streamingPort.write();
    }
    else
    {
        m_streamingPort.unprepare();
    }
}

void MultipleAnalogSensorsServer::threadRelease()
{
    return;
}

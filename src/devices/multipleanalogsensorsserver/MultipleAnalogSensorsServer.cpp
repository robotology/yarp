/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
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

namespace yarp {
namespace dev {


MultipleAnalogSensorsServer::MultipleAnalogSensorsServer(): RateThread(20)
{

}

MultipleAnalogSensorsServer::~MultipleAnalogSensorsServer()
{

}

bool MultipleAnalogSensorsServer::open(os::Searchable& config)
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


    if (!config.find("period").isInt())
    {
        yError("MultipleAnalogSensorsClient: period parameter is present but it is not an integer, exiting.");
        return false;
    }

    m_periodInMs = config.find("period").asInt();

    if (m_periodInMs <= 0)
    {
        yError("MultipleAnalogSensorsClient: period parameter is present (%d) but it is not a positive integer, exiting.", m_periodInMs);
        return false;
    }

    std::string name = config.find("name").asString().c_str();

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
                                  bool (Interface::*getNameMethodPtr)(size_t, yarp::os::ConstString&) const,
                                  bool (Interface::*getFrameNameMethodPtr)(size_t, yarp::os::ConstString&) const)
{
    if (wrappedDeviceInterface)
    {
        size_t nrOfSensors = MAS_CALL_MEMBER_FN(wrappedDeviceInterface, getNrOfSensorsMethodPtr)();
        metadataVector.resize(nrOfSensors);
        for (size_t i=0; i < nrOfSensors; i++)
        {
            yarp::os::ConstString sensorName;
            bool ok = MAS_CALL_MEMBER_FN(wrappedDeviceInterface, getNameMethodPtr)(i, sensorName);
            if (!ok)
            {
                yError() << "MultipleAnalogSensorsServer: Failure in reading name of sensor of type "
                         << tag.c_str() << " at index " << i << " .";
                return false;
            }
            yarp::os::ConstString frameName;
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
                                                                     bool (Interface::*getNameMethodPtr)(size_t, yarp::os::ConstString&) const)
{
    if (wrappedDeviceInterface)
    {
        size_t nrOfSensors = MAS_CALL_MEMBER_FN(wrappedDeviceInterface, getNrOfSensorsMethodPtr)();
        metadataVector.resize(nrOfSensors);
        for (size_t i=0; i < nrOfSensors; i++)
        {
            yarp::os::ConstString sensorName;
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
                                       &IThreeAxisGyroscopes::getNrOfThreeAxisGyroscopes,
                                       &IThreeAxisGyroscopes::getThreeAxisGyroscopeName,
                                       &IThreeAxisGyroscopes::getThreeAxisGyroscopeFrameName);
    ok = ok && populateSensorsMetadata(m_iThreeAxisLinearAccelerometers, m_sensorMetadata.ThreeAxisLinearAccelerometers, "ThreeAxisLinearAccelerometers",
                                       &IThreeAxisLinearAccelerometers::getNrOfThreeAxisLinearAccelerometers,
                                       &IThreeAxisLinearAccelerometers::getThreeAxisLinearAccelerometerName,
                                       &IThreeAxisLinearAccelerometers::getThreeAxisLinearAccelerometerFrameName);
    ok = ok && populateSensorsMetadata(m_iThreeAxisMagnetometers, m_sensorMetadata.ThreeAxisMagnetometers, "ThreeAxisMagnetometers",
                                       &IThreeAxisMagnetometers::getNrOfThreeAxisMagnetometers,
                                       &IThreeAxisMagnetometers::getThreeAxisMagnetometerName,
                                       &IThreeAxisMagnetometers::getThreeAxisMagnetometerFrameName);
    ok = ok && populateSensorsMetadata(m_iOrientationSensors, m_sensorMetadata.OrientationSensors, "OrientationSensors",
                                       &IOrientationSensors::getNrOfOrientationSensors,
                                       &IOrientationSensors::getOrientationSensorName,
                                       &IOrientationSensors::getOrientationSensorFrameName);
    ok = ok && populateSensorsMetadata(m_iTemperatureSensors, m_sensorMetadata.TemperatureSensors, "TemperatureSensors",
                                       &ITemperatureSensors::getNrOfTemperatureSensors,
                                       &ITemperatureSensors::getTemperatureSensorName,
                                       &ITemperatureSensors::getTemperatureSensorFrameName);
    ok = ok && populateSensorsMetadata(m_iSixAxisForceTorqueSensors, m_sensorMetadata.SixAxisForceTorqueSensors, "SixAxisForceTorqueSensors",
                                       &ISixAxisForceTorqueSensors::getNrOfSixAxisForceTorqueSensors,
                                       &ISixAxisForceTorqueSensors::getSixAxisForceTorqueSensorName,
                                       &ISixAxisForceTorqueSensors::getSixAxisForceTorqueSensorFrameName);
    ok = ok && populateSensorsMetadataNoFrameName(m_iContactLoadCellArrays, m_sensorMetadata.ContactLoadCellArrays, "ContactLoadCellArrays",
                                       &IContactLoadCellArrays::getNrOfContactLoadCellArrays,
                                       &IContactLoadCellArrays::getContactLoadCellArrayName);
    ok = ok && populateSensorsMetadataNoFrameName(m_iEncoderArrays, m_sensorMetadata.EncoderArrays, "EncoderArrays",
                                       &IEncoderArrays::getNrOfEncoderArrays,
                                       &IEncoderArrays::getEncoderArrayName);
    ok = ok && populateSensorsMetadataNoFrameName(m_iSkinPatches, m_sensorMetadata.SkinPatches, "ISkinPatches",
                                       &ISkinPatches::getNrOfSkinPatches,
                                       &ISkinPatches::getSkinPatchName);

    return ok;
}

bool MultipleAnalogSensorsServer::attachAll(const PolyDriverList& p)
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

    PolyDriver* poly = p[0]->poly;

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
    ok = this->setRate(m_periodInMs);
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

    return false;
}

SensorRPCData MultipleAnalogSensorsServer::getMetadata()
{
    return m_sensorMetadata;
}

template<typename Interface>
bool MultipleAnalogSensorsServer::genericStreamData(Interface* wrappedDeviceInterface,
                                                    const std::vector< SensorMetadata >& metadataVector,
                                                    std::vector< SensorMeasurement >& streamingDataVector,
                                                    MAS_status (Interface::*getStatusMethodPtr)(size_t) const,
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
            MAS_status status = MAS_CALL_MEMBER_FN(wrappedDeviceInterface, getStatusMethodPtr)(i);
            if (status != MAS_OK)
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
                                 &IThreeAxisGyroscopes::getThreeAxisGyroscopeStatus,
                                 &IThreeAxisGyroscopes::getThreeAxisGyroscopeMeasure);

    ok = ok && genericStreamData(m_iThreeAxisLinearAccelerometers, m_sensorMetadata.ThreeAxisLinearAccelerometers,
                                 streamingData.ThreeAxisLinearAccelerometers.measurements,
                                 &IThreeAxisLinearAccelerometers::getThreeAxisLinearAccelerometerStatus,
                                 &IThreeAxisLinearAccelerometers::getThreeAxisLinearAccelerometerMeasure);

    ok = ok && genericStreamData(m_iThreeAxisMagnetometers, m_sensorMetadata.ThreeAxisMagnetometers,
                                 streamingData.ThreeAxisMagnetometers.measurements,
                                 &IThreeAxisMagnetometers::getThreeAxisMagnetometerStatus,
                                 &IThreeAxisMagnetometers::getThreeAxisMagnetometerMeasure);

    ok = ok && genericStreamData(m_iOrientationSensors, m_sensorMetadata.OrientationSensors,
                                 streamingData.OrientationSensors.measurements,
                                 &IOrientationSensors::getOrientationSensorStatus,
                                 &IOrientationSensors::getOrientationSensorMeasureAsRollPitchYaw);

    ok = ok && genericStreamData(m_iTemperatureSensors, m_sensorMetadata.TemperatureSensors,
                                 streamingData.TemperatureSensors.measurements,
                                 &ITemperatureSensors::getTemperatureSensorStatus,
                                 &ITemperatureSensors::getTemperatureSensorMeasure);

    ok = ok && genericStreamData(m_iSixAxisForceTorqueSensors, m_sensorMetadata.SixAxisForceTorqueSensors,
                                 streamingData.SixAxisForceTorqueSensors.measurements,
                                 &ISixAxisForceTorqueSensors::getSixAxisForceTorqueSensorStatus,
                                 &ISixAxisForceTorqueSensors::getSixAxisForceTorqueSensorMeasure);

    ok = ok && genericStreamData(m_iContactLoadCellArrays, m_sensorMetadata.ContactLoadCellArrays,
                                 streamingData.ContactLoadCellArrays.measurements,
                                 &IContactLoadCellArrays::getContactLoadCellArrayStatus,
                                 &IContactLoadCellArrays::getContactLoadCellArrayMeasure);

    ok = ok && genericStreamData(m_iEncoderArrays, m_sensorMetadata.EncoderArrays,
                                 streamingData.EncoderArrays.measurements,
                                 &IEncoderArrays::getEncoderArrayStatus,
                                 &IEncoderArrays::getEncoderArrayMeasure);

    ok = ok && genericStreamData(m_iSkinPatches, m_sensorMetadata.SkinPatches,
                                 streamingData.SkinPatches.measurements,
                                 &ISkinPatches::getSkinPatchStatus,
                                 &ISkinPatches::getSkinPatchMeasure);

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






}
}

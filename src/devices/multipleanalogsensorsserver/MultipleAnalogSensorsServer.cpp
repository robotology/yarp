/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "SensorStreamingData.h"
#include "MultipleAnalogSensorsMetadata.h"

#include "MultipleAnalogSensorsServer.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/Property.h>
#include <yarp/dev/PolyDriverList.h>


namespace {
YARP_LOG_COMPONENT(MULTIPLEANALOGSENSORSSERVER, "yarp.device.multipleanalogsensorsserver")
}

/**
 * Internal identifier of the type of sensors.
 */
enum MAS_SensorTypeServer
{
    ThreeAxisGyroscopes=0,
    ThreeAxisLinearAccelerometers=1,
    ThreeAxisMagnetometers=2,
    OrientationSensors=3,
    TemperatureSensors=4,
    SixAxisForceTorqueSensors=5,
    ContactLoadCellArrays=6,
    EncoderArrays=7,
    SkinPatches=8,
    PositionSensors=9
};

/**
 * Get measure size for sensors with fixed size measure.
 */
inline size_t MAS_getMeasureSizeFromEnum(const MAS_SensorTypeServer type)
{
    switch(type)
    {
        case ThreeAxisGyroscopes:
            return 3;
            break;
        case ThreeAxisLinearAccelerometers:
            return 3;
            break;
        case ThreeAxisMagnetometers:
            return 3;
            break;
        case OrientationSensors:
            return 3;
            break;
        case PositionSensors:
            return 3;
            break;
        case TemperatureSensors:
            return 1;
            break;
        case SixAxisForceTorqueSensors:
            return 6;
            break;
        default:
            assert(false);
            // "MAS_getMeasureSizeFromEnum passed unexepcted enum";
            return 0;
            break;
    }
}

inline std::string MAS_getStatusString(const yarp::dev::MAS_status status)
{
    switch(status)
    {
        case yarp::dev::MAS_OK:
            return "MAS_OK";
            break;
        case yarp::dev::MAS_ERROR:
            return "MAS_ERROR";
            break;
        case yarp::dev::MAS_OVF:
            return "MAS_OVF";
            break;
        case yarp::dev::MAS_TIMEOUT:
            return "MAS_TIMEOUT";
            break;
        case yarp::dev::MAS_WAITING_FOR_FIRST_READ:
            return "MAS_WAITING_FOR_FIRST_READ";
            break;
        case yarp::dev::MAS_UNKNOWN:
            return "MAS_UNKNOWN";
            break;
        default:
            assert(false);
            // "MAS_getStatusString passed unexepcted enum";
            return "";
            break;
    }
}



MultipleAnalogSensorsServer::MultipleAnalogSensorsServer() :
        PeriodicThread(0.02)
{
}

MultipleAnalogSensorsServer::~MultipleAnalogSensorsServer() = default;

bool MultipleAnalogSensorsServer::open(yarp::os::Searchable& config)
{
    if (!config.check("name"))
    {
        yCError(MULTIPLEANALOGSENSORSSERVER, "Missing name parameter, exiting.");
        return false;
    }

    if (!config.check("period"))
    {
        yCError(MULTIPLEANALOGSENSORSSERVER, "Missing period parameter, exiting.");
        return false;
    }

    if (!config.find("period").isInt32())
    {
        yCError(MULTIPLEANALOGSENSORSSERVER, "Period parameter is present but it is not an integer, exiting.");
        return false;
    }

    m_periodInS = config.find("period").asInt32() / 1000.0;

    if (m_periodInS <= 0)
    {
        yCError(MULTIPLEANALOGSENSORSSERVER,
                "Period parameter is present (%f) but it is not a positive integer, exiting.",
                m_periodInS);
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

    if (config.check("subdevice"))
    {
        std::string subdeviceName = config.find("subdevice").asString();

        yarp::os::Property driverConfig;
        driverConfig.fromString(config.toString());
        driverConfig.setMonitor(config.getMonitor(), subdeviceName.c_str()); // pass on any monitoring
        driverConfig.put("device", subdeviceName);

        if (!m_subdevice.open(driverConfig))
        {
            yCError(MULTIPLEANALOGSENSORSSERVER, "Opening subdevice failed.");
            return false;
        }

        yarp::dev::PolyDriverList driverList;
        driverList.push(&m_subdevice, subdeviceName.c_str());

        if (!attachAll(driverList))
        {
            yCError(MULTIPLEANALOGSENSORSSERVER, "Attaching subdevice failed.");
            return false;
        }

        yCInfo(MULTIPLEANALOGSENSORSSERVER,
               "Subdevice \"%s\" successfully configured and attached.",
               subdeviceName.c_str());
        m_isDeviceOwned = true;
    }

    yCDebug(MULTIPLEANALOGSENSORSSERVER, "Open complete");
    return true;
}

bool MultipleAnalogSensorsServer::close()
{
    bool ok = this->detachAll();

    if (m_isDeviceOwned)
    {
        ok &= m_subdevice.close();
        m_isDeviceOwned = false;
    }

    yCDebug(MULTIPLEANALOGSENSORSSERVER, "Close complete");
    return ok;
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
                yCError(MULTIPLEANALOGSENSORSSERVER,
                        "Failure in reading name of sensor of type %s at index %zu.",
                        tag.c_str(),
                        i);
                return false;
            }
            std::string frameName;
            ok = MAS_CALL_MEMBER_FN(wrappedDeviceInterface, getFrameNameMethodPtr)(i, frameName);
            if (!ok)
            {
                yCError(MULTIPLEANALOGSENSORSSERVER,
                        "Failure in reading frame name of sensor of type %s at index %zu.",
                        tag.c_str(),
                        i);
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
                yCError(MULTIPLEANALOGSENSORSSERVER,
                        "Failure in reading name of sensor of type  %s at index %zu.",
                        tag.c_str(),
                        i);
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
    ok = ok && populateSensorsMetadata(m_iPositionSensors, m_sensorMetadata.PositionSensors, "PositionSensors",
                                       &yarp::dev::IPositionSensors::getNrOfPositionSensors,
                                       &yarp::dev::IPositionSensors::getPositionSensorName,
                                       &yarp::dev::IPositionSensors::getPositionSensorFrameName);
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

// Function to resize the measure vectors, variant where the size is given by a method
template<typename Interface>
bool MultipleAnalogSensorsServer::resizeMeasureVectors(Interface* wrappedDeviceInterface,
                                                       const std::vector< SensorMetadata >& metadataVector,
                                                       std::vector< SensorMeasurement >& streamingDataVector,
                                                       size_t (Interface::*getMeasureSizePtr)(size_t) const)
{
    if (wrappedDeviceInterface)
    {
        size_t nrOfSensors = metadataVector.size();
        streamingDataVector.resize(nrOfSensors);
        for (size_t i=0; i < nrOfSensors; i++)
        {
            size_t measureSize = MAS_CALL_MEMBER_FN(wrappedDeviceInterface, getMeasureSizePtr)(i);
            streamingDataVector[i].measurement.resize(measureSize, std::numeric_limits<double>::quiet_NaN());
        }
    }

    return true;
}

// Function to resize the measure vectors, variant where the measure size is given by the type of the sensor
template<typename Interface>
bool MultipleAnalogSensorsServer::resizeMeasureVectors(Interface* wrappedDeviceInterface,
                                                       const std::vector< SensorMetadata >& metadataVector,
                                                       std::vector< SensorMeasurement >& streamingDataVector,
                                                       size_t measureSize)
{
    if (wrappedDeviceInterface)
    {
        size_t nrOfSensors = metadataVector.size();
        streamingDataVector.resize(nrOfSensors);
        for (size_t i=0; i < nrOfSensors; i++)
        {
            streamingDataVector[i].measurement.resize(measureSize, std::numeric_limits<double>::quiet_NaN());
        }
    }

    return true;
}

bool MultipleAnalogSensorsServer::resizeAllMeasureVectors(SensorStreamingData& streamingData)
{
    bool ok = true;
    // The size of each sensor is given in the interface documentation
    ok = ok && resizeMeasureVectors(m_iThreeAxisGyroscopes, m_sensorMetadata.ThreeAxisGyroscopes,
                                    streamingData.ThreeAxisGyroscopes.measurements, MAS_getMeasureSizeFromEnum(ThreeAxisGyroscopes));
    ok = ok && resizeMeasureVectors(m_iThreeAxisLinearAccelerometers, m_sensorMetadata.ThreeAxisLinearAccelerometers,
                                    streamingData.ThreeAxisLinearAccelerometers.measurements, MAS_getMeasureSizeFromEnum(ThreeAxisLinearAccelerometers));
    ok = ok && resizeMeasureVectors(m_iThreeAxisMagnetometers, m_sensorMetadata.ThreeAxisMagnetometers,
                                    streamingData.ThreeAxisMagnetometers.measurements, MAS_getMeasureSizeFromEnum(ThreeAxisMagnetometers));
    ok = ok && resizeMeasureVectors(m_iPositionSensors, m_sensorMetadata.PositionSensors,
                                    streamingData.PositionSensors.measurements, MAS_getMeasureSizeFromEnum(PositionSensors));
    ok = ok && resizeMeasureVectors(m_iOrientationSensors, m_sensorMetadata.OrientationSensors,
                                    streamingData.OrientationSensors.measurements, MAS_getMeasureSizeFromEnum(OrientationSensors));
    ok = ok && resizeMeasureVectors(m_iTemperatureSensors, m_sensorMetadata.TemperatureSensors,
                                    streamingData.TemperatureSensors.measurements, MAS_getMeasureSizeFromEnum(TemperatureSensors));
    ok = ok && resizeMeasureVectors(m_iSixAxisForceTorqueSensors, m_sensorMetadata.SixAxisForceTorqueSensors,
                                    streamingData.SixAxisForceTorqueSensors.measurements, MAS_getMeasureSizeFromEnum(SixAxisForceTorqueSensors));
    ok = ok && resizeMeasureVectors(m_iContactLoadCellArrays, m_sensorMetadata.ContactLoadCellArrays,
                                    streamingData.ContactLoadCellArrays.measurements, &yarp::dev::IContactLoadCellArrays::getContactLoadCellArraySize);
    ok = ok && resizeMeasureVectors(m_iEncoderArrays, m_sensorMetadata.EncoderArrays,
                                    streamingData.EncoderArrays.measurements, &yarp::dev::IEncoderArrays::getEncoderArraySize);
    ok = ok && resizeMeasureVectors(m_iSkinPatches, m_sensorMetadata.SkinPatches,
                                    streamingData.SkinPatches.measurements, &yarp::dev::ISkinPatches::getSkinPatchSize);

    return ok;
}

bool MultipleAnalogSensorsServer::attachAll(const yarp::dev::PolyDriverList& p)
{
    // Attach the device
    if (p.size() > 1)
    {
        yCError(MULTIPLEANALOGSENSORSSERVER,
                "This device only supports exposing a "
                "single MultipleAnalogSensors device on YARP ports, but %d devices have been passed in attachAll.",
                p.size());
        yCError(MULTIPLEANALOGSENSORSSERVER,
                "Please use the multipleanalogsensorsremapper device to combine several device in a new device.");
        close();
        return false;
    }

    if (p.size() == 0)
    {
        yCError(MULTIPLEANALOGSENSORSSERVER, "No device passed to attachAll, please pass a device to expose on YARP ports.");
        close();
        return false;
    }

    yarp::dev::PolyDriver* poly = p[0]->poly;

    if (!poly)
    {
        yCError(MULTIPLEANALOGSENSORSSERVER, "Null pointer passed to attachAll.");
        close();
        return false;
    }

    // View all the interfaces
    poly->view(m_iThreeAxisGyroscopes);
    poly->view(m_iThreeAxisLinearAccelerometers);
    poly->view(m_iThreeAxisMagnetometers);
    poly->view(m_iPositionSensors);
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
        yCError(MULTIPLEANALOGSENSORSSERVER, "Failure in populateAllSensorsMetadata()");
        close();
        return false;
    }

    // Attach was successful, open the ports
    ok = m_streamingPort.open(m_streamingPortName);
    if (!ok)
    {
        yCError(MULTIPLEANALOGSENSORSSERVER, "Failure in opening port named %s.", m_streamingPortName.c_str());
        close();
        return false;
    }

    ok = this->yarp().attachAsServer(m_rpcPort);
    if (!ok)
    {
        yCError(MULTIPLEANALOGSENSORSSERVER, "Failure in attaching RPC port to thrift RPC interface.");
        close();
        return false;
    }

    ok = m_rpcPort.open(m_RPCPortName);
    if (!ok)
    {
        yCError(MULTIPLEANALOGSENSORSSERVER, "Failure in opening port named %s.", m_RPCPortName.c_str());
        close();
        return false;
    }

    // Set rate period
    ok = this->setPeriod(m_periodInS);
    ok = ok && this->start();
    if (!ok)
    {
        yCError(MULTIPLEANALOGSENSORSSERVER, "Failure in starting thread.");
        close();
        return false;
    }

    yCDebug(MULTIPLEANALOGSENSORSSERVER, "Attach complete");
    return true;
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

    yCDebug(MULTIPLEANALOGSENSORSSERVER, "Detach complete");
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
                                                    bool (Interface::*getMeasureMethodPtr)(size_t, yarp::sig::Vector&, double&) const,
                                                    const char* sensorType)
{
    if (wrappedDeviceInterface)
    {
        size_t nrOfSensors = metadataVector.size();
        for (size_t i=0; i < nrOfSensors; i++)
        {
            yarp::sig::Vector& outputBuffer = streamingDataVector[i].measurement;
            double& outputTimestamp = streamingDataVector[i].timestamp;
            yarp::dev::MAS_status status = MAS_CALL_MEMBER_FN(wrappedDeviceInterface, getStatusMethodPtr)(i);
            if (status == yarp::dev::MAS_OK)
            {
                MAS_CALL_MEMBER_FN(wrappedDeviceInterface, getMeasureMethodPtr)(i, outputBuffer, outputTimestamp);
            } else
            {
                yCError(MULTIPLEANALOGSENSORSSERVER,
                        "Failure in reading data from sensor %s of type %s with code %s, no data will be sent on the port.",
                        metadataVector[i].name.c_str(), sensorType, MAS_getStatusString(status).c_str());
                return false;
            }
        }
    }

    return true;
}


void MultipleAnalogSensorsServer::run()
{
    SensorStreamingData& streamingData = m_streamingPort.prepare();

    // Resize the output streaming buffer vectors to be of the correct size
    bool ok = true;
    ok = resizeAllMeasureVectors(streamingData);

    // Populate buffers
    ok = ok && genericStreamData(m_iThreeAxisGyroscopes, m_sensorMetadata.ThreeAxisGyroscopes,
                                 streamingData.ThreeAxisGyroscopes.measurements,
                                 &yarp::dev::IThreeAxisGyroscopes::getThreeAxisGyroscopeStatus,
                                 &yarp::dev::IThreeAxisGyroscopes::getThreeAxisGyroscopeMeasure,
                                 "ThreeAxisGyroscope");

    ok = ok && genericStreamData(m_iThreeAxisLinearAccelerometers, m_sensorMetadata.ThreeAxisLinearAccelerometers,
                                 streamingData.ThreeAxisLinearAccelerometers.measurements,
                                 &yarp::dev::IThreeAxisLinearAccelerometers::getThreeAxisLinearAccelerometerStatus,
                                 &yarp::dev::IThreeAxisLinearAccelerometers::getThreeAxisLinearAccelerometerMeasure,
                                 "ThreeAxisLinearAccelerometer");

    ok = ok && genericStreamData(m_iThreeAxisMagnetometers, m_sensorMetadata.ThreeAxisMagnetometers,
                                 streamingData.ThreeAxisMagnetometers.measurements,
                                 &yarp::dev::IThreeAxisMagnetometers::getThreeAxisMagnetometerStatus,
                                 &yarp::dev::IThreeAxisMagnetometers::getThreeAxisMagnetometerMeasure,
                                 "ThreeAxisMagnetometer");

    ok = ok && genericStreamData(m_iPositionSensors, m_sensorMetadata.PositionSensors,
                                 streamingData.PositionSensors.measurements,
                                 &yarp::dev::IPositionSensors::getPositionSensorStatus,
                                 &yarp::dev::IPositionSensors::getPositionSensorMeasure,
                                 "PositionSensor");

    ok = ok && genericStreamData(m_iOrientationSensors, m_sensorMetadata.OrientationSensors,
                                 streamingData.OrientationSensors.measurements,
                                 &yarp::dev::IOrientationSensors::getOrientationSensorStatus,
                                 &yarp::dev::IOrientationSensors::getOrientationSensorMeasureAsRollPitchYaw,
                                 "OrientationSensor");

    ok = ok && genericStreamData(m_iTemperatureSensors, m_sensorMetadata.TemperatureSensors,
                                 streamingData.TemperatureSensors.measurements,
                                 &yarp::dev::ITemperatureSensors::getTemperatureSensorStatus,
                                 &yarp::dev::ITemperatureSensors::getTemperatureSensorMeasure,
                                 "TemperatureSensor");

    ok = ok && genericStreamData(m_iSixAxisForceTorqueSensors, m_sensorMetadata.SixAxisForceTorqueSensors,
                                 streamingData.SixAxisForceTorqueSensors.measurements,
                                 &yarp::dev::ISixAxisForceTorqueSensors::getSixAxisForceTorqueSensorStatus,
                                 &yarp::dev::ISixAxisForceTorqueSensors::getSixAxisForceTorqueSensorMeasure,
                                 "SixAxisForceTorqueSensor");

    ok = ok && genericStreamData(m_iContactLoadCellArrays, m_sensorMetadata.ContactLoadCellArrays,
                                 streamingData.ContactLoadCellArrays.measurements,
                                 &yarp::dev::IContactLoadCellArrays::getContactLoadCellArrayStatus,
                                 &yarp::dev::IContactLoadCellArrays::getContactLoadCellArrayMeasure,
                                 "ContactLoadCellArrays");

    ok = ok && genericStreamData(m_iEncoderArrays, m_sensorMetadata.EncoderArrays,
                                 streamingData.EncoderArrays.measurements,
                                 &yarp::dev::IEncoderArrays::getEncoderArrayStatus,
                                 &yarp::dev::IEncoderArrays::getEncoderArrayMeasure,
                                 "EncoderArray");

    ok = ok && genericStreamData(m_iSkinPatches, m_sensorMetadata.SkinPatches,
                                 streamingData.SkinPatches.measurements,
                                 &yarp::dev::ISkinPatches::getSkinPatchStatus,
                                 &yarp::dev::ISkinPatches::getSkinPatchMeasure,
                                 "SkinPatch");

    if (ok)
    {
        m_stamp.update();
        m_streamingPort.setEnvelope(m_stamp);
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

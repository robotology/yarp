/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "MultipleAnalogSensorsRemapper.h"

#include <map>

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Searchable.h>

using namespace yarp::os;
using namespace yarp::dev;

namespace {
YARP_LOG_COMPONENT(MULTIPLEANALOGSENSORSREMAPPER, "yarp.device.multipleanalogsensorsremapper")
}

const size_t MAS_NrOfSensorTypes{10};
static_assert(MAS_SensorType::PositionSensors+1 == MAS_NrOfSensorTypes, "Consistency error between MAS_NrOfSensorTypes and MAS_SensorType");

/**
 * Internal identifier of the type of sensors.
 */
inline std::string MAS_getTagFromEnum(const MAS_SensorType type)
{
    switch(type)
    {
        case ThreeAxisGyroscopes:
            return "ThreeAxisGyroscopes";
            break;
        case ThreeAxisLinearAccelerometers:
            return "ThreeAxisLinearAccelerometers";
            break;
        case ThreeAxisMagnetometers:
            return "ThreeAxisMagnetometers";
            break;
        case OrientationSensors:
            return "OrientationSensors";
            break;
        case TemperatureSensors:
            return "TemperatureSensors";
            break;
        case SixAxisForceTorqueSensors:
            return "SixAxisForceTorqueSensors";
            break;
        case ContactLoadCellArrays:
            return "ContactLoadCellArrays";
            break;
        case EncoderArrays:
            return "EncoderArrays";
            break;
        case SkinPatches:
            return "SkinPatches";
            break;
        case PositionSensors:
            return "PositionSensors";
            break;
        default:
            assert(false);
            return "MAS_getTagFromEnum_notExpectedEnum";
            break;
    }
}

bool MultipleAnalogSensorsRemapper::close()
{
    return detachAll();
}

bool MultipleAnalogSensorsRemapper::open(Searchable& config)
{
    Property prop;
    prop.fromString(config.toString());

    m_verbose = (prop.check("verbose","if present, give detailed output"));
    if (m_verbose)
    {
        yCInfo(MULTIPLEANALOGSENSORSREMAPPER, "Running with verbose output\n");
    }

    if(!parseOptions(prop))
    {
        return false;
    }

    return true;
}

// Return an empty list if the key is not found, and an error (false) if the key was found but it is not a list of strings
bool getVectorOfStringFromListInConfig(const std::string& key, const yarp::os::Searchable& config, std::vector<std::string> & vectorOfStrings)
{
    yarp::os::Property prop;
    prop.fromString(config.toString());
    bool keyExists = prop.check(key);

    yarp::os::Bottle *propList=prop.find(key).asList();
    if (!propList && keyExists)
    {
       yCError(MULTIPLEANALOGSENSORSREMAPPER) << "Error parsing parameters: if present " << key << " should be followed by a list of strings.\n";
       return false;
    }

    if (!propList && !keyExists)
    {
        vectorOfStrings.resize(0);
        return true;
    }

    vectorOfStrings.resize(propList->size());
    for (size_t ax=0; ax < propList->size(); ax++)
    {
        vectorOfStrings[ax] = propList->get(ax).asString();
    }

    return true;
}

bool MultipleAnalogSensorsRemapper::parseOptions(const Property& prop)
{
    bool ok = true;

    m_remappedSensors.resize(MAS_NrOfSensorTypes);

    for (size_t i = 0; i < MAS_NrOfSensorTypes; i++)
    {
        auto sensType = static_cast<MAS_SensorType>(i);
        std::string optionName = MAS_getTagFromEnum(sensType) +"Names";
        ok = getVectorOfStringFromListInConfig(optionName , prop, m_remappedSensors[i]);
        if (!ok)
        {
            yCError(MULTIPLEANALOGSENSORSREMAPPER) << optionName << "should be followed by a list of string.";
            return false;
        }
    }

    return ok;
}

// Note: as soon as we support only C++17, we can switch to using std::invoke
// See https://isocpp.org/wiki/faq/pointers-to-members#fnptr-vs-memfnptr-types
#define MAS_CALL_MEMBER_FN(object, ptrToMember)  ((*object).*(ptrToMember))


template<typename Interface>
bool MultipleAnalogSensorsRemapper::genericAttachAll(const MAS_SensorType sensorType,
                                                     std::vector<Interface *>& subDeviceVec,
                                                     const PolyDriverList &polylist,
                                                     bool (Interface::*getNameMethodPtr)(size_t, std::string&) const,
                                                     size_t (Interface::*getNrOfSensorsMethodPtr)() const)
{
    std::map<std::string, SensorInSubDevice> sensorLocationMap;

    subDeviceVec.resize(polylist.size());

    for(int p=0; p<polylist.size(); p++)
    {
        // If this fails it is ok, this just means that this devices does not expose this kind of sensors
        polylist[p]->poly->view(subDeviceVec[p]);

        if (subDeviceVec[p])
        {
            size_t nrOfSensorsInSubDevice = MAS_CALL_MEMBER_FN(subDeviceVec[p], getNrOfSensorsMethodPtr)();
            for (size_t s=0; s < nrOfSensorsInSubDevice; s++)
            {
                std::string name;
                bool ok = MAS_CALL_MEMBER_FN(subDeviceVec[p], getNameMethodPtr)(s,name);
                if (!ok)
                {
                    yCError(MULTIPLEANALOGSENSORSREMAPPER) << "Failure in getting a name in the device " << polylist[p]->key;
                    return false;
                }

                // If the name is already in the map, raise an error
                if (sensorLocationMap.find(name) != sensorLocationMap.end())
                {
                    SensorInSubDevice deviceWithSameName = sensorLocationMap.find(name)->second;
                    yCError(MULTIPLEANALOGSENSORSREMAPPER)
                        << "Sensor ambiguity: sensor with name"
                        << name
                        << "present on both device"
                        << polylist[p]->key
                        << polylist[deviceWithSameName.subDevice]->key;
                    return false;
                }

                sensorLocationMap[name] = SensorInSubDevice(p, s);
            }
        }
    }

    // Fill the indices map given the name of all the subdevices
    std::vector<SensorInSubDevice>& sensIndicesMap = m_indicesMap[static_cast<size_t>(sensorType)];
    sensIndicesMap.resize(m_remappedSensors[sensorType].size());
    for(size_t i=0; i < m_remappedSensors[sensorType].size(); i++)
    {
        std::string name = m_remappedSensors[sensorType][i];
        if (sensorLocationMap.find(name) == sensorLocationMap.end())
        {
            yCError(MULTIPLEANALOGSENSORSREMAPPER) << "Impossible to find sensor name" << name << ", exiting.";
            return false;
        }

        sensIndicesMap[i] = sensorLocationMap.find(name)->second;
    }

    return true;
}



bool MultipleAnalogSensorsRemapper::attachAll(const PolyDriverList &polylist)
{
    bool ok = true;
    m_indicesMap.resize(MAS_NrOfSensorTypes);
    ok = ok && genericAttachAll(ThreeAxisGyroscopes, m_iThreeAxisGyroscopes, polylist,
                                &IThreeAxisGyroscopes::getThreeAxisGyroscopeName, &IThreeAxisGyroscopes::getNrOfThreeAxisGyroscopes);
    ok = ok && genericAttachAll(ThreeAxisLinearAccelerometers, m_iThreeAxisLinearAccelerometers, polylist,
                                &IThreeAxisLinearAccelerometers::getThreeAxisLinearAccelerometerName, &IThreeAxisLinearAccelerometers::getNrOfThreeAxisLinearAccelerometers);
    ok = ok && genericAttachAll(ThreeAxisMagnetometers, m_iThreeAxisMagnetometers, polylist,
                                &IThreeAxisMagnetometers::getThreeAxisMagnetometerName, &IThreeAxisMagnetometers::getNrOfThreeAxisMagnetometers);
    ok = ok && genericAttachAll(PositionSensors, m_iPositionSensors, polylist,
                                &IPositionSensors::getPositionSensorName, &IPositionSensors::getNrOfPositionSensors);
    ok = ok && genericAttachAll(OrientationSensors, m_iOrientationSensors, polylist,
                                &IOrientationSensors::getOrientationSensorName, &IOrientationSensors::getNrOfOrientationSensors);
    ok = ok && genericAttachAll(TemperatureSensors, m_iTemperatureSensors, polylist,
                                &ITemperatureSensors::getTemperatureSensorName, &ITemperatureSensors::getNrOfTemperatureSensors);
    ok = ok && genericAttachAll(SixAxisForceTorqueSensors, m_iSixAxisForceTorqueSensors, polylist,
                                &ISixAxisForceTorqueSensors::getSixAxisForceTorqueSensorName, &ISixAxisForceTorqueSensors::getNrOfSixAxisForceTorqueSensors);
    ok = ok && genericAttachAll(ContactLoadCellArrays, m_iContactLoadCellArrays, polylist,
                                &IContactLoadCellArrays::getContactLoadCellArrayName, &IContactLoadCellArrays::getNrOfContactLoadCellArrays);
    ok = ok && genericAttachAll(EncoderArrays, m_iEncoderArrays, polylist,
                                &IEncoderArrays::getEncoderArrayName, &IEncoderArrays::getNrOfEncoderArrays);
    ok = ok && genericAttachAll(SkinPatches, m_iSkinPatches, polylist,
                                &ISkinPatches::getSkinPatchName, &ISkinPatches::getNrOfSkinPatches);

    return ok;
}

bool MultipleAnalogSensorsRemapper::detachAll()
{
    m_iThreeAxisGyroscopes.resize(0);
    m_iThreeAxisLinearAccelerometers.resize(0);
    m_iThreeAxisMagnetometers.resize(0);
    m_iPositionSensors.resize(0);
    m_iOrientationSensors.resize(0);
    m_iTemperatureSensors.resize(0);
    m_iSixAxisForceTorqueSensors.resize(0);
    m_iContactLoadCellArrays.resize(0);
    m_iEncoderArrays.resize(0);
    m_iSkinPatches.resize(0);
    m_indicesMap.resize(0);
    return true;
}


template<typename Interface>
MAS_status MultipleAnalogSensorsRemapper::genericGetStatus(const MAS_SensorType sensorType,
                                                           size_t& sens_index,
                                                           const std::vector<Interface *>& subDeviceVec,
                                                           MAS_status (Interface::*methodPtr)(size_t) const) const
{
    size_t nrOfAvailableSensors = m_indicesMap[sensorType].size();
    if (sens_index >= nrOfAvailableSensors)
    {
        if (m_verbose)
        {
            yCError(MULTIPLEANALOGSENSORSREMAPPER, "genericGetStatus sens_index %zu out of range of available sensors (%zu).", sens_index, nrOfAvailableSensors);
        }
        return MAS_ERROR;
    }

    SensorInSubDevice subDeviceSensor = m_indicesMap[sensorType][sens_index];
    return MAS_CALL_MEMBER_FN(subDeviceVec[subDeviceSensor.subDevice], methodPtr)(subDeviceSensor.indexInSubDevice);
}

template<typename Interface>
bool MultipleAnalogSensorsRemapper::genericGetName(const MAS_SensorType sensorType,
                                                           size_t& sens_index, std::string &name,
                                                           const std::vector<Interface *>& subDeviceVec,
                                                           bool (Interface::*methodPtr)(size_t, std::string &) const) const
{
    size_t nrOfAvailableSensors = m_indicesMap[sensorType].size();
    if (sens_index >= nrOfAvailableSensors)
    {
        if (m_verbose)
        {
            yCError(MULTIPLEANALOGSENSORSREMAPPER, "genericGetName sens_index %zu out of range of available sensors (%zu).", sens_index, nrOfAvailableSensors);
        }
        return MAS_ERROR;
    }

    SensorInSubDevice subDeviceSensor = m_indicesMap[sensorType][sens_index];
    return MAS_CALL_MEMBER_FN(subDeviceVec[subDeviceSensor.subDevice], methodPtr)(subDeviceSensor.indexInSubDevice, name);
}

template<typename Interface>
bool MultipleAnalogSensorsRemapper::genericGetFrameName(const MAS_SensorType sensorType,
                                                           size_t& sens_index, std::string &name,
                                                           const std::vector<Interface *>& subDeviceVec,
                                                           bool (Interface::*methodPtr)(size_t, std::string &) const) const
{
    size_t nrOfAvailableSensors = m_indicesMap[sensorType].size();
    if (sens_index >= nrOfAvailableSensors)
    {
        if (m_verbose)
        {
            yCError(MULTIPLEANALOGSENSORSREMAPPER, "genericGetFrameName sens_index %zu out of range of available sensors (%zu).", sens_index, nrOfAvailableSensors);
        }
        return MAS_ERROR;
    }

    SensorInSubDevice subDeviceSensor = m_indicesMap[sensorType][sens_index];
    return MAS_CALL_MEMBER_FN(subDeviceVec[subDeviceSensor.subDevice], methodPtr)(subDeviceSensor.indexInSubDevice, name);
}


template<typename Interface>
bool MultipleAnalogSensorsRemapper::genericGetMeasure(const MAS_SensorType sensorType,
                                                           size_t& sens_index, yarp::sig::Vector& out, double& timestamp,
                                                           const std::vector<Interface *>& subDeviceVec,
                                                           bool (Interface::*methodPtr)(size_t, yarp::sig::Vector&, double&) const) const
{
    size_t nrOfAvailableSensors = m_indicesMap[sensorType].size();
    if (sens_index >= nrOfAvailableSensors)
    {
        if (m_verbose)
        {
            yCError(MULTIPLEANALOGSENSORSREMAPPER, "genericGetMeasure sens_index %zu out of range of available sensors (%zu).", sens_index, nrOfAvailableSensors);
        }
        return MAS_ERROR;
    }

    SensorInSubDevice subDeviceSensor = m_indicesMap[sensorType][sens_index];
    return MAS_CALL_MEMBER_FN(subDeviceVec[subDeviceSensor.subDevice], methodPtr)(subDeviceSensor.indexInSubDevice, out, timestamp);
}

template<typename Interface>
size_t MultipleAnalogSensorsRemapper::genericGetSize(const MAS_SensorType sensorType,
                                                           size_t& sens_index,
                                                           const std::vector<Interface *>& subDeviceVec,
                                                           size_t (Interface::*methodPtr)(size_t) const) const
{
    size_t nrOfAvailableSensors = m_indicesMap[sensorType].size();
    if (sens_index >= nrOfAvailableSensors)
    {
        if (m_verbose)
        {
            yCError(MULTIPLEANALOGSENSORSREMAPPER, "genericGetSize sens_index %zu out of range of available sensors (%zu).", sens_index, nrOfAvailableSensors);
        }
        return MAS_ERROR;
    }

    SensorInSubDevice subDeviceSensor = m_indicesMap[sensorType][sens_index];
    return MAS_CALL_MEMBER_FN(subDeviceVec[subDeviceSensor.subDevice], methodPtr)(subDeviceSensor.indexInSubDevice);
}

/*
All the sensor specific methods (excluding the IOrientationSensor and the ISkinPatches) are just an instantiation of the following template (note: we avoid code generation for the sake of readability):

size_t MultipleAnalogSensorsRemapper::getNrOf{{SensorTag}}s() const
{
    return m_indicesMap[{{SensorTag}}s].size();
}

MAS_status MultipleAnalogSensorsRemapper::get{{SensorTag}}Status(size_t sens_index) const
{
    return genericGetStatus({{SensorTag}}s, sens_index, m_i{{SensorTag}}s, &I{{SensorTag}}s::get{{SensorTag}}Status);
}

bool MultipleAnalogSensorsRemapper::get{{SensorTag}}Name(size_t sens_index, std::string& name) const
{
    return genericGetName({{SensorTag}}s, sens_index, name, m_i{{SensorTag}}s, &I{{SensorTag}}s::get{{SensorTag}}Name);
}

bool MultipleAnalogSensorsRemapper::get{{SensorTag}}Measure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{
    return genericGetMeasure({{SensorTag}}s, sens_index, out, timestamp, m_i{{SensorTag}}s, &I{{SensorTag}}s::get{{SensorTag}}Measure);
}

For the sensors (EncoderArray and SkinPatch) of which the measurements can change size, we also have:
size_t MultipleAnalogSensorsRemapper::get{{SensorTag}}Size(size_t sens_index) const
{
    return genericGetSize({{SensorTag}}s, sens_index, m_i{{SensorTag}}s, &I{{SensorTag}}s::get{{SensorTag}}Size);
}

*/

size_t MultipleAnalogSensorsRemapper::getNrOfThreeAxisGyroscopes() const
{
    return m_indicesMap[ThreeAxisGyroscopes].size();
}

MAS_status MultipleAnalogSensorsRemapper::getThreeAxisGyroscopeStatus(size_t sens_index) const
{
    return genericGetStatus(ThreeAxisGyroscopes, sens_index, m_iThreeAxisGyroscopes, &IThreeAxisGyroscopes::getThreeAxisGyroscopeStatus);
}

bool MultipleAnalogSensorsRemapper::getThreeAxisGyroscopeName(size_t sens_index, std::string& name) const
{
     return genericGetName(ThreeAxisGyroscopes, sens_index, name, m_iThreeAxisGyroscopes, &IThreeAxisGyroscopes::getThreeAxisGyroscopeName);
}

bool MultipleAnalogSensorsRemapper::getThreeAxisGyroscopeFrameName(size_t sens_index, std::string& frameName) const
{
     return genericGetFrameName(ThreeAxisGyroscopes, sens_index, frameName, m_iThreeAxisGyroscopes, &IThreeAxisGyroscopes::getThreeAxisGyroscopeFrameName);
}

bool MultipleAnalogSensorsRemapper::getThreeAxisGyroscopeMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{
     return genericGetMeasure(ThreeAxisGyroscopes, sens_index, out, timestamp, m_iThreeAxisGyroscopes, &IThreeAxisGyroscopes::getThreeAxisGyroscopeMeasure);
}

size_t MultipleAnalogSensorsRemapper::getNrOfThreeAxisLinearAccelerometers() const
{
    return m_indicesMap[ThreeAxisLinearAccelerometers].size();
}

MAS_status MultipleAnalogSensorsRemapper::getThreeAxisLinearAccelerometerStatus(size_t sens_index) const
{
    return genericGetStatus(ThreeAxisLinearAccelerometers, sens_index, m_iThreeAxisLinearAccelerometers, &IThreeAxisLinearAccelerometers::getThreeAxisLinearAccelerometerStatus);
}

bool MultipleAnalogSensorsRemapper::getThreeAxisLinearAccelerometerName(size_t sens_index, std::string& name) const
{
     return genericGetName(ThreeAxisLinearAccelerometers, sens_index, name, m_iThreeAxisLinearAccelerometers, &IThreeAxisLinearAccelerometers::getThreeAxisLinearAccelerometerName);
}

bool MultipleAnalogSensorsRemapper::getThreeAxisLinearAccelerometerFrameName(size_t sens_index, std::string& frameName) const
{
     return genericGetFrameName(ThreeAxisLinearAccelerometers, sens_index, frameName, m_iThreeAxisLinearAccelerometers, &IThreeAxisLinearAccelerometers::getThreeAxisLinearAccelerometerFrameName);
}

bool MultipleAnalogSensorsRemapper::getThreeAxisLinearAccelerometerMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{
     return genericGetMeasure(ThreeAxisLinearAccelerometers, sens_index, out, timestamp, m_iThreeAxisLinearAccelerometers, &IThreeAxisLinearAccelerometers::getThreeAxisLinearAccelerometerMeasure);
}

size_t MultipleAnalogSensorsRemapper::getNrOfThreeAxisMagnetometers() const
{
    return m_indicesMap[ThreeAxisMagnetometers].size();
}

MAS_status MultipleAnalogSensorsRemapper::getThreeAxisMagnetometerStatus(size_t sens_index) const
{
    return genericGetStatus(ThreeAxisMagnetometers, sens_index, m_iThreeAxisMagnetometers, &IThreeAxisMagnetometers::getThreeAxisMagnetometerStatus);
}

bool MultipleAnalogSensorsRemapper::getThreeAxisMagnetometerName(size_t sens_index, std::string& name) const
{
     return genericGetName(ThreeAxisMagnetometers, sens_index, name, m_iThreeAxisMagnetometers, &IThreeAxisMagnetometers::getThreeAxisMagnetometerName);
}

bool MultipleAnalogSensorsRemapper::getThreeAxisMagnetometerFrameName(size_t sens_index, std::string& frameName) const
{
     return genericGetFrameName(ThreeAxisMagnetometers, sens_index, frameName, m_iThreeAxisMagnetometers, &IThreeAxisMagnetometers::getThreeAxisMagnetometerFrameName);
}

bool MultipleAnalogSensorsRemapper::getThreeAxisMagnetometerMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{
     return genericGetMeasure(ThreeAxisMagnetometers, sens_index, out, timestamp, m_iThreeAxisMagnetometers, &IThreeAxisMagnetometers::getThreeAxisMagnetometerMeasure);
}

size_t MultipleAnalogSensorsRemapper::getNrOfPositionSensors() const
{
    return m_indicesMap[PositionSensors].size();
}

MAS_status MultipleAnalogSensorsRemapper::getPositionSensorStatus(size_t sens_index) const
{
    return genericGetStatus(PositionSensors, sens_index, m_iPositionSensors, &IPositionSensors::getPositionSensorStatus);
}

bool MultipleAnalogSensorsRemapper::getPositionSensorName(size_t sens_index, std::string& name) const
{
    return genericGetName(PositionSensors, sens_index, name, m_iPositionSensors, &IPositionSensors::getPositionSensorName);
}

bool MultipleAnalogSensorsRemapper::getPositionSensorFrameName(size_t sens_index, std::string& frameName) const
{
    return genericGetFrameName(PositionSensors, sens_index, frameName, m_iPositionSensors, &IPositionSensors::getPositionSensorFrameName);
}

bool MultipleAnalogSensorsRemapper::getPositionSensorMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{
    return genericGetMeasure(PositionSensors, sens_index, out, timestamp, m_iPositionSensors, &IPositionSensors::getPositionSensorMeasure);
}

size_t MultipleAnalogSensorsRemapper::getNrOfOrientationSensors() const
{
    return m_indicesMap[OrientationSensors].size();
}

MAS_status MultipleAnalogSensorsRemapper::getOrientationSensorStatus(size_t sens_index) const
{
    return genericGetStatus(OrientationSensors, sens_index, m_iOrientationSensors, &IOrientationSensors::getOrientationSensorStatus);
}

bool MultipleAnalogSensorsRemapper::getOrientationSensorName(size_t sens_index, std::string& name) const
{
     return genericGetName(OrientationSensors, sens_index, name, m_iOrientationSensors, &IOrientationSensors::getOrientationSensorName);
}

bool MultipleAnalogSensorsRemapper::getOrientationSensorFrameName(size_t sens_index, std::string& frameName) const
{
     return genericGetFrameName(OrientationSensors, sens_index, frameName, m_iOrientationSensors, &IOrientationSensors::getOrientationSensorFrameName);
}

bool MultipleAnalogSensorsRemapper::getOrientationSensorMeasureAsRollPitchYaw(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{
     return genericGetMeasure(OrientationSensors, sens_index, out, timestamp, m_iOrientationSensors, &IOrientationSensors::getOrientationSensorMeasureAsRollPitchYaw);
}

size_t MultipleAnalogSensorsRemapper::getNrOfTemperatureSensors() const
{
    return m_indicesMap[TemperatureSensors].size();
}

MAS_status MultipleAnalogSensorsRemapper::getTemperatureSensorStatus(size_t sens_index) const
{
    return genericGetStatus(TemperatureSensors, sens_index, m_iTemperatureSensors, &ITemperatureSensors::getTemperatureSensorStatus);
}

bool MultipleAnalogSensorsRemapper::getTemperatureSensorName(size_t sens_index, std::string& name) const
{
     return genericGetName(TemperatureSensors, sens_index, name, m_iTemperatureSensors, &ITemperatureSensors::getTemperatureSensorName);
}

bool MultipleAnalogSensorsRemapper::getTemperatureSensorFrameName(size_t sens_index, std::string& frameName) const
{
     return genericGetFrameName(TemperatureSensors, sens_index, frameName, m_iTemperatureSensors, &ITemperatureSensors::getTemperatureSensorFrameName);
}

bool MultipleAnalogSensorsRemapper::getTemperatureSensorMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{
     return genericGetMeasure(TemperatureSensors, sens_index, out, timestamp, m_iTemperatureSensors, &ITemperatureSensors::getTemperatureSensorMeasure);
}

bool MultipleAnalogSensorsRemapper::getTemperatureSensorMeasure(size_t sens_index, double& out, double& timestamp) const
{
    yarp::sig::Vector dummy(1);
    bool ok = genericGetMeasure(TemperatureSensors, sens_index, dummy, timestamp, m_iTemperatureSensors, &ITemperatureSensors::getTemperatureSensorMeasure);
    out = dummy[0];
    return ok;
}

size_t MultipleAnalogSensorsRemapper::getNrOfSixAxisForceTorqueSensors() const
{
    return m_indicesMap[SixAxisForceTorqueSensors].size();
}

MAS_status MultipleAnalogSensorsRemapper::getSixAxisForceTorqueSensorStatus(size_t sens_index) const
{
    return genericGetStatus(SixAxisForceTorqueSensors, sens_index, m_iSixAxisForceTorqueSensors, &ISixAxisForceTorqueSensors::getSixAxisForceTorqueSensorStatus);
}

bool MultipleAnalogSensorsRemapper::getSixAxisForceTorqueSensorName(size_t sens_index, std::string& name) const
{
     return genericGetName(SixAxisForceTorqueSensors, sens_index, name, m_iSixAxisForceTorqueSensors, &ISixAxisForceTorqueSensors::getSixAxisForceTorqueSensorName);
}

bool MultipleAnalogSensorsRemapper::getSixAxisForceTorqueSensorFrameName(size_t sens_index, std::string& frameName) const
{
     return genericGetFrameName(SixAxisForceTorqueSensors, sens_index, frameName, m_iSixAxisForceTorqueSensors, &ISixAxisForceTorqueSensors::getSixAxisForceTorqueSensorFrameName);
}

bool MultipleAnalogSensorsRemapper::getSixAxisForceTorqueSensorMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{
     return genericGetMeasure(SixAxisForceTorqueSensors, sens_index, out, timestamp, m_iSixAxisForceTorqueSensors, &ISixAxisForceTorqueSensors::getSixAxisForceTorqueSensorMeasure);
}

size_t MultipleAnalogSensorsRemapper::getNrOfContactLoadCellArrays() const
{
    return m_indicesMap[ContactLoadCellArrays].size();
}

MAS_status MultipleAnalogSensorsRemapper::getContactLoadCellArrayStatus(size_t sens_index) const
{
    return genericGetStatus(ContactLoadCellArrays, sens_index, m_iContactLoadCellArrays, &IContactLoadCellArrays::getContactLoadCellArrayStatus);
}

bool MultipleAnalogSensorsRemapper::getContactLoadCellArrayName(size_t sens_index, std::string& name) const
{
     return genericGetName(ContactLoadCellArrays, sens_index, name, m_iContactLoadCellArrays, &IContactLoadCellArrays::getContactLoadCellArrayName);
}

bool MultipleAnalogSensorsRemapper::getContactLoadCellArrayMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{
     return genericGetMeasure(ContactLoadCellArrays, sens_index, out, timestamp, m_iContactLoadCellArrays, &IContactLoadCellArrays::getContactLoadCellArrayMeasure);
}

size_t MultipleAnalogSensorsRemapper::getContactLoadCellArraySize(size_t sens_index) const
{
    return genericGetSize(ContactLoadCellArrays, sens_index, m_iContactLoadCellArrays, &IContactLoadCellArrays::getContactLoadCellArraySize);
}

size_t MultipleAnalogSensorsRemapper::getNrOfEncoderArrays() const
{
    return m_indicesMap[EncoderArrays].size();
}

MAS_status MultipleAnalogSensorsRemapper::getEncoderArrayStatus(size_t sens_index) const
{
    return genericGetStatus(EncoderArrays, sens_index, m_iEncoderArrays, &IEncoderArrays::getEncoderArrayStatus);
}

bool MultipleAnalogSensorsRemapper::getEncoderArrayName(size_t sens_index, std::string& name) const
{
     return genericGetName(EncoderArrays, sens_index, name, m_iEncoderArrays, &IEncoderArrays::getEncoderArrayName);
}

bool MultipleAnalogSensorsRemapper::getEncoderArrayMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{
     return genericGetMeasure(EncoderArrays, sens_index, out, timestamp, m_iEncoderArrays, &IEncoderArrays::getEncoderArrayMeasure);
}

size_t MultipleAnalogSensorsRemapper::getEncoderArraySize(size_t sens_index) const
{
    return genericGetSize(EncoderArrays, sens_index, m_iEncoderArrays, &IEncoderArrays::getEncoderArraySize);
}

size_t MultipleAnalogSensorsRemapper::getNrOfSkinPatches() const
{
    return m_indicesMap[SkinPatches].size();
}

MAS_status MultipleAnalogSensorsRemapper::getSkinPatchStatus(size_t sens_index) const
{
    return genericGetStatus(SkinPatches, sens_index, m_iSkinPatches, &ISkinPatches::getSkinPatchStatus);
}

bool MultipleAnalogSensorsRemapper::getSkinPatchName(size_t sens_index, std::string& name) const
{
     return genericGetName(SkinPatches, sens_index, name, m_iSkinPatches, &ISkinPatches::getSkinPatchName);
}

bool MultipleAnalogSensorsRemapper::getSkinPatchMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{
     return genericGetMeasure(SkinPatches, sens_index, out, timestamp, m_iSkinPatches, &ISkinPatches::getSkinPatchMeasure);
}

size_t MultipleAnalogSensorsRemapper::getSkinPatchSize(size_t sens_index) const
{
    return genericGetSize(SkinPatches, sens_index, m_iSkinPatches, &ISkinPatches::getSkinPatchSize);
}

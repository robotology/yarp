/*
 * Copyright (C) 2018 iCub Facility, Istituto Italiano di Tecnologia (IIT)
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_DEV_MULTIPLEANALOGSENSORSINTERFACES_H
#define YARP_DEV_MULTIPLEANALOGSENSORSINTERFACES_H

#include <cassert>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/sig/Matrix.h>
#include <yarp/sig/Vector.h>

namespace yarp
{
namespace dev
{

/**
 * Status of a given analog sensor exposed by a multiple analog sensors interface. 
 */
enum MAS_status
{
   MAS_OK=0,
   MAS_ERROR=1,      // generic error
   MAS_OVF=2,        // overflow
   MAS_TIMEOUT=3,
   MAS_WAITING_FOR_FIRST_READ=4
};

/**
 * Internal identifier of the type of sensors.
 * TODO(traversaro): move to private headers
 */
enum MAS_SensorType
{
    ThreeAxisGyroscopes=0,
    ThreeAxisLinearAccelerometers=1,
    ThreeAxisMagnetometers=2,
    OrientationSensors=3,
    TemperatureSensors=4,
    SixAxisForceTorqueSensors=5,
    ContactLoadCellArrays=6,
    EncoderArrays=7,
    SkinPatches=8
};

const size_t MAS_NrOfSensorTypes{9};
static_assert(MAS_SensorType::SkinPatches+1 == MAS_NrOfSensorTypes, "Error");

/**
 * Internal identifier of the type of sensors.
 * TODO(traversaro): move to private headers
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
        default:
            assert(false);
            return "MAS_getTagFromEnum_notExpectedEnum";
            break;
    }
}


/**
 * @ingroup dev_iface 
 * @defgroup dev_iface_multiple_analog Multiple Analog Sensor Interfaces 
 * 
 * The interfaces present in this group are meant to be implemented by 
 * devices that expose sensor that can be efficiently expressed as vector of
 * real numbers, so-called "analog" sensors. 
 * 
 * For more information see the documentation of server, client and remapper devices that operate on these interfaces: 
 * 
 * * yarp::dev::MultipleAnalogSensorsServer
 * * yarp::dev::MultipleAnalogSensorsClient
 * * yarp::dev::MultipleAnalogSensorsRemapper
 * 
 */

/**
 * @ingroup dev_iface_multiple_analog
 *
 * \brief Device interface to one or multiple three axis gyroscopes. 
 * 
 * | Sensor Tag  |
 * |:-----------------:|
 * | `ThreeAxisGyroscopes` |
 * 
 */
class YARP_dev_API IThreeAxisGyroscopes
{
public:
   /**
    * Get the number of three axis gyroscopes exposed by this sensor. 
    */
   virtual size_t getNrOfThreeAxisGyroscopes() const = 0;
   
   /**
    * Get the status of the specified sensor. 
    */
   virtual yarp::dev::MAS_status getThreeAxisGyroscopeStatus(size_t sens_index) const = 0;
   
   /**
    * Get the name of the specified sensor. 
    */
   virtual bool getThreeAxisGyroscopeName(size_t sens_index, yarp::os::ConstString &name) const = 0;
   
   /**
    * Get the name of the frame of the specified sensor.
    */
   virtual bool getThreeAxisGyroscopeFrameName(size_t sens_index, yarp::os::ConstString &frameName) const = 0;

   /**
    * Get the last reading of the specified sensor. 
    */
   virtual bool getThreeAxisGyroscopeMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const = 0;

   virtual ~IThreeAxisGyroscopes(){}
};


/**
 * @ingroup dev_iface_multiple_analog
 *
 * \brief Device interface to one or multiple three axis linear accelerometers. 
 * 
 * | Sensor Tag  |
 * |:-----------------:|
 * | `ThreeAxisLinearAccelerometers` |
 */
class YARP_dev_API IThreeAxisLinearAccelerometers
{
public:
   /**
    * Get the number of three axis linear accelerometers exposed by this device. 
    */
   virtual size_t getNrOfThreeAxisLinearAccelerometers() const = 0;
   
   /**
    * Get the status of the specified sensor. 
    */
   virtual yarp::dev::MAS_status getThreeAxisLinearAccelerometerStatus(size_t sens_index) const = 0;
   
   /**
    * Get the name of the specified sensor. 
    */
   virtual bool getThreeAxisLinearAccelerometerName(size_t sens_index, yarp::os::ConstString &name) const = 0;

   /**
    * Get the name of the frame of the specified sensor.
    */
   virtual bool getThreeAxisLinearAccelerometerFrameName(size_t sens_index, yarp::os::ConstString &frameName) const = 0;
   
   /**
    * Get the last reading of the specified sensor. 
    */
   virtual bool getThreeAxisLinearAccelerometerMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const = 0;

   virtual ~IThreeAxisLinearAccelerometers(){}
};

/**
 * @ingroup dev_iface_multiple_analog
 *
 * \brief Device interface to one or multiple three axis magnetometers. 
 * 
 * | Sensor Tag  |
 * |:-----------------:|
 * | `ThreeAxisMagnetometers` |
 */
class YARP_dev_API IThreeAxisMagnetometers
{
public:
   /**
    * Get the number of magnetometers exposed by this device. 
    */
   virtual size_t getNrOfThreeAxisMagnetometers() const = 0;
   
   /**
    * Get the status of the specified sensor. 
    */
   virtual yarp::dev::MAS_status getThreeAxisMagnetometerStatus(size_t sens_index) const = 0;
   
   /**
    * Get the name of the specified sensor. 
    */
   virtual bool getThreeAxisMagnetometerName(size_t sens_index, yarp::os::ConstString &name) const = 0;

   /**
    * Get the name of the frame of the specified sensor.
    */
   virtual bool getThreeAxisMagnetometerFrameName(size_t sens_index, yarp::os::ConstString &frameName) const = 0;
   
   /**
    * Get the last reading of the specified sensor. 
    */
   virtual bool getThreeAxisMagnetometerMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const = 0;

   virtual ~IThreeAxisMagnetometers(){}
};

/**
 * @ingroup dev_iface_multiple_analog
 *
 * \brief Device interface to one or multiple orientation sensors, such as IMUs with on board estimation algorithms.
 * 
 * | Sensor Tag  |
 * |:-----------------:|
 * | `OrientationSensors` |
 */
class YARP_dev_API IOrientationSensors
{
public:
   /**
    * Get the number of orientation sensors exposed by this device. 
    */
   virtual size_t getNrOfOrientationSensors() const = 0;
   
   /**
    * Get the status of the specified sensor. 
    */
   virtual yarp::dev::MAS_status getOrientationSensorStatus(size_t sens_index) const = 0;
   
   /**
    * Get the name of the specified sensor. 
    */
   virtual bool getOrientationSensorName(size_t sens_index, yarp::os::ConstString &name) const = 0;

   /**
    * Get the name of the frame of the specified sensor.
    */
   virtual bool getOrientationSensorFrameName(size_t sens_index, yarp::os::ConstString &frameName) const = 0;
   
   /**
    * Get the last reading of the orientation sensor as roll pitch yaw.
    */
   virtual bool getOrientationSensorMeasureAsRollPitchYaw(size_t sens_index, yarp::sig::Vector& rpy, double& timestamp) const = 0;
   
   virtual ~IOrientationSensors(){}
};


/**
 * @ingroup dev_iface_multiple_analog
 *
 * \brief Device interface to one or multiple orientation sensors, such as IMUs with on board estimation algorithms.
 * 
 * | Sensor Tag  |
 * |:-----------------:|
 * | `TemperatureSensors` |
 */
class YARP_dev_API ITemperatureSensors
{
public:
   /**
    * Get the number of temperature sensors exposed by this device. 
    */
   virtual size_t getNrOfTemperatureSensors() const = 0;
   
   /**
    * Get the status of the specified sensor. 
    */
   virtual yarp::dev::MAS_status getTemperatureSensorStatus(size_t sens_index) const = 0;
   
   /**
    * Get the name of the specified sensor. 
    */
   virtual bool getTemperatureSensorName(size_t sens_index, yarp::os::ConstString &name) const = 0;

   /**
    * Get the name of the frame of the specified sensor.
    */
   virtual bool getTemperatureSensorFrameName(size_t sens_index, yarp::os::ConstString &frameName) const = 0;
   
   /**
    * Get the last reading of the specified sensor. 
    * TODO(traversaro) : make the method swig-friendly
    */
   virtual bool getTemperatureSensorMeasure(size_t sens_index, double& out, double& timestamp) const = 0;
   
   /**
    * Get the last reading of the specified sensor. 
    */
   virtual bool getTemperatureSensorMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const = 0;
   
   
   virtual ~ITemperatureSensors(){}
};

/**
 * @ingroup dev_iface_multiple_analog
 *
 * \brief Device interface to one or multiple six axis force torque sensor.
 * 
 * The first three element of the returned vector are the three-axis forces, while the last three elements are the three-axis torques. 
 * 
 * | Sensor Tag  |
 * |:-----------------:|
 * | `SixAxisForceTorqueSensors` |
 */
class YARP_dev_API ISixAxisForceTorqueSensors
{
public:
   /**
    * Get the number of six axis force torque sensors exposed by this device. 
    */
   virtual size_t getNrOfSixAxisForceTorqueSensors() const = 0;
   
   /**
    * Get the status of the specified sensor. 
    */
   virtual yarp::dev::MAS_status getSixAxisForceTorqueSensorStatus(size_t sens_index) const = 0;
   
   /**
    * Get the name of the specified sensor. 
    */
   virtual bool getSixAxisForceTorqueSensorName(size_t sens_index, yarp::os::ConstString &name) const = 0;

   /**
    * Get the name of the frame of the specified sensor.
    */
   virtual bool getSixAxisForceTorqueSensorFrameName(size_t sens_index, yarp::os::ConstString &frameName) const = 0;

   /**
    * Get the last reading of the specified sensor. 
    */
   virtual bool getSixAxisForceTorqueSensorMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const = 0;
   
   
   virtual ~ISixAxisForceTorqueSensors(){}
};

/**
 * @ingroup dev_iface_multiple_analog
 *
 * \brief Device interface to one or multiple contact load cell arrays.
 * 
 * | Sensor Tag  |
 * |:-----------------:|
 * | `ContactLoadCellArrays` |
 * 
 * @note This interface is meant to expose array of sensors of normal force 
 *       for contact, such as the FSR array present in the Nao Robot ( http://doc.aldebaran.com/1-14/family/robots/fsr_robot.html ). 
 */
class YARP_dev_API IContactLoadCellArrays
{
public:
   /**
    * Get the number of contact load cell array exposed by this device. 
    */
   virtual size_t getNrOfContactLoadCellArrays() const = 0;
   
   /**
    * Get the status of the specified sensor. 
    */
   virtual yarp::dev::MAS_status getContactLoadCellArrayStatus(size_t sens_index) const = 0;
   
   /**
    * Get the name of the specified sensor. 
    */
   virtual bool getContactLoadCellArrayName(size_t sens_index, yarp::os::ConstString &name) const = 0;

   /**
    * Get the last reading of the specified sensor. 
    */
   virtual bool getContactLoadCellArrayMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const = 0;
   
   /**
    * Get the size of the specified contact load cell array
    */
   virtual size_t getContactLoadCellArraySize(size_t sens_index) const = 0;
   
   
   virtual ~IContactLoadCellArrays(){}
};

/**
 * @ingroup dev_iface_multiple_analog
 *
 * \brief Device interface to one or multiple arrays of encoders. 
 * 
 * | Sensor Tag  |
 * |:-----------------:|
 * | `EncoderArrays` |
 * 
 * @note This interface is tipically used for group of encoders that are not 
 *       explicitly controlled by one of the interfaces tipically used for 
 *       motor control, such as IEncoders and IPositionControl, such as 
 *       encoders measuring the complete state in an underactuated mechanism.
 */
class YARP_dev_API IEncoderArrays
{
public:
   /**
    * Get the number of encoder arrays exposed by this device. 
    */
   virtual size_t getNrOfEncoderArrays() const = 0;
   
   /**
    * Get the status of the specified sensor. 
    */
   virtual yarp::dev::MAS_status getEncoderArrayStatus(size_t sens_index) const = 0;
   
   /**
    * Get the name of the specified sensor. 
    */
   virtual bool getEncoderArrayName(size_t sens_index, yarp::os::ConstString &name) const = 0;

   /**
    * Get the last reading of the specified sensor. 
    */
   virtual bool getEncoderArrayMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const = 0;
   
   /**
    * Get the size of the specified encoder array
    */
   virtual size_t getEncoderArraySize(size_t sens_index) const = 0;
   
   
   virtual ~IEncoderArrays(){}
};

/**
 * @ingroup dev_iface_multiple_analog
 * 
 * \brief Device interface to one or more groups (patches) of tactile sensors (skin). 
 * 
 * | Sensor Tag  |
 * |:-----------------:|
 * | `SkinPatches` |
 * 
 * 
 * \brief Device interface to one or multiple patches of tacticle sensors. 
 */
class YARP_dev_API ISkinPatches
{
public:
   /**
    * Get the number of skin patches exposed by this device. 
    */
   virtual size_t getNrOfSkinPatches() const = 0;
   
   /**
    * Get the status of the specified sensor. 
    */
   virtual yarp::dev::MAS_status getSkinPatchStatus(size_t sens_index) const = 0;
   
   /**
    * Get the name of the specified sensor. 
    */
   virtual bool getSkinPatchName(size_t sens_index, yarp::os::ConstString &name) const = 0;

   /**
    * Get the last reading of the specified sensor. 
    */
   virtual bool getSkinPatchMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const = 0;
   
   /**
    * Get the size of the specified skin patch
    */
   virtual size_t getSkinPatchSize(size_t sens_index) const = 0;
   
   
   virtual ~ISkinPatches(){}
};

}
}

#endif



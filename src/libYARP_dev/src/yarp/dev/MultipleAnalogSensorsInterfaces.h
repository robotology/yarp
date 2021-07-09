/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_MULTIPLEANALOGSENSORSINTERFACES_H
#define YARP_DEV_MULTIPLEANALOGSENSORSINTERFACES_H

#include <cassert>
#include <string>

#include <yarp/dev/api.h>
#include <yarp/sig/Vector.h>

namespace yarp
{
    namespace dev
    {
        class IThreeAxisGyroscopes;
        class IThreeAxisLinearAccelerometers;
        class IThreeAxisMagnetometers;
        class IPositionSensors;
        class IOrientationSensors;
        class ITemperatureSensors;
        class ISixAxisForceTorqueSensors;
        class IContactLoadCellArrays;
        class IEncoderArrays;
        class ISkinPatches;

        /**
         * Status of a given analog sensor exposed by a multiple analog sensors interface.
         */
        enum MAS_status
        {
            MAS_OK=0,         ///< The sensor is working correctly.
            MAS_ERROR=1,      ///< The sensor is in generic error state.
            MAS_OVF=2,        ///< The sensor reached an overflow.
            MAS_TIMEOUT=3,    ///< The sensor is read through the network, and the latest measurement was received before an implementation-define timeout period.
            MAS_WAITING_FOR_FIRST_READ=4, ///< The sensor is read through the network, and the device is waiting to receive the first measurement.
            MAS_UNKNOWN=5     ///< The sensor is in an unknown state.
        };
    }
}

/**
 * @ingroup dev_iface_multiple_analog
 *
 * \brief Device interface to one or multiple three axis gyroscopes.
 *
 * This interface is used to expose the measurement of a gyroscope device.
 *
 * The measure of a gyroscope is an 3-dimensional angular velocity, and the unit of measurement
 * used in this device is degrees/seconds.
 *
 * | Sensor Tag  |
 * |:-----------------:|
 * | `ThreeAxisGyroscopes` |
 *
 */
class YARP_dev_API yarp::dev::IThreeAxisGyroscopes
{
public:
    /**
     * Get the number of three axis gyroscopes exposed by this sensor.
     */
    virtual size_t getNrOfThreeAxisGyroscopes() const = 0;

    /**
     * Get the status of the specified sensor.
     *
     * @param[in] sens_index The index of the specified sensor (should be between 0 and getNrOfThreeAxisGyroscopes()-1).
     */
    virtual yarp::dev::MAS_status getThreeAxisGyroscopeStatus(size_t sens_index) const = 0;

    /**
     * Get the name of the specified sensor.
     *
     * @param[in] sens_index The index of the specified sensor (should be between 0 and getNrOfThreeAxisGyroscopes()-1).
     * @return false if an error occurred, true otherwise.
     */
    virtual bool getThreeAxisGyroscopeName(size_t sens_index, std::string &name) const = 0;

    /**
     * Get the name of the frame of the specified sensor.
     *
     * @param[in] sens_index The index of the specified sensor (should be between 0 and getNrOfThreeAxisGyroscopes()-1).
     * @return false if an error occurred, true otherwise.
     */
    virtual bool getThreeAxisGyroscopeFrameName(size_t sens_index, std::string &frameName) const = 0;

    /**
     * \brief Get the last reading of the gyroscope.
     *
     * @param[in] sens_index The index of the specified sensor (should be between 0 and getNrOfThreeAxisGyroscopes()-1).
     * @param[out] out The requested measure. The vector should be 3-dimensional. The measure is expressed in degrees/seconds.
     * @param[out] timestamp The timestamp of the requested measure, expressed in seconds.
     * @return false if an error occurred, true otherwise.
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
class YARP_dev_API yarp::dev::IThreeAxisLinearAccelerometers
{
public:
    /**
     * \brief Get the number of three axis linear accelerometers exposed by this device.
     */
    virtual size_t getNrOfThreeAxisLinearAccelerometers() const = 0;

    /**
     * Get the status of the specified sensor.
     */
    virtual yarp::dev::MAS_status getThreeAxisLinearAccelerometerStatus(size_t sens_index) const = 0;

    /**
     * Get the name of the specified sensor.
     * @return false if an error occurred, true otherwise.
     */
    virtual bool getThreeAxisLinearAccelerometerName(size_t sens_index, std::string &name) const = 0;

    /**
     * Get the name of the frame of the specified sensor.
     * @return false if an error occurred, true otherwise.
     */
    virtual bool getThreeAxisLinearAccelerometerFrameName(size_t sens_index, std::string &frameName) const = 0;

    /**
     * Get the last reading of the specified sensor.
     *
     * @param[in] sens_index The index of the specified sensor (should be between 0 and getNrOfThreeAxisLinearAccelerometers()-1).
     * @param[out] out The requested measure. The vector should be 3-dimensional. The measure is expressed in meters^2/seconds.
     * @param[out] timestamp The timestamp of the requested measure, expressed in seconds.
     * @return false if an error occurred, true otherwise.
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
class YARP_dev_API yarp::dev::IThreeAxisMagnetometers
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
     * @return false if an error occurred, true otherwise.
     */
    virtual bool getThreeAxisMagnetometerName(size_t sens_index, std::string &name) const = 0;

    /**
     * Get the name of the frame of the specified sensor.
     * @return false if an error occurred, true otherwise.
     */
    virtual bool getThreeAxisMagnetometerFrameName(size_t sens_index, std::string &frameName) const = 0;

    /**
     * Get the last reading of the specified sensor.
     *
     * @param[in] sens_index The index of the specified sensor (should be between 0 and getNrOfThreeAxisMagnetometers()-1).
     * @param[out] out The requested measure. The vector should be 3-dimensional. The measure is expressed in tesla .
     * @param[out] timestamp The timestamp of the requested measure, expressed in seconds.
     * @return false if an error occurred, true otherwise.
     */
    virtual bool getThreeAxisMagnetometerMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const = 0;

    virtual ~IThreeAxisMagnetometers(){}
};

/**
 * @ingroup dev_iface_multiple_analog
 *
 * \brief Device interface to one or multiple position sensors, such as UWB localization sensors.
 * The device returns the relative position between a lab or surface-fixed frame and
 * a frame rigidly attached to the sensor.
 *
 * The definition of the lab or surface-fixed frame is sensor-specific.
 *
 * | Sensor Tag  |
 * |:-----------------:|
 * | `PositionSensors` |
 */
class YARP_dev_API yarp::dev::IPositionSensors
{
public:
    /**
     * Get the number of position sensors exposed by this device.
     */
    virtual size_t getNrOfPositionSensors() const = 0;

    /**
     * Get the status of the specified sensor.
     */
    virtual yarp::dev::MAS_status getPositionSensorStatus(size_t sens_index) const = 0;

    /**
     * Get the name of the specified sensor.
     * @return false if an error occurred, true otherwise.
     */
    virtual bool getPositionSensorName(size_t sens_index, std::string& name) const = 0;

    /**
     * Get the name of the frame of the specified sensor.
     *
     * @note This is an implementation specific method, that may return the name of the sensor
     *       frame in a scenegraph
     *
     * @return false if an error occurred, true otherwise.
     */
    virtual bool getPositionSensorFrameName(size_t sens_index, std::string& frameName) const = 0;

    /**
     * Get the last reading of the position sensor as x y z.
     *
     * @param[in] sens_index The index of the specified sensor (should be between 0 and getNrOfPositionSensors()-1).
     * @param[out] out The requested measure. The vector should be 3-dimensional. The measure is expressed in meters.
     * @param[out] timestamp The timestamp of the requested measure, expressed in seconds.
     * @return false if an error occurred, true otherwise.
     */
    virtual bool getPositionSensorMeasure(size_t sens_index, yarp::sig::Vector& xyz, double& timestamp) const = 0;

    virtual ~IPositionSensors()
    {
    }
};

/**
 * @ingroup dev_iface_multiple_analog
 *
 * \brief Device interface to one or multiple orientation sensors, such as IMUs with on board estimation algorithms.
 *
 * The device returns the relative orientation between a lab or surface-fixed frame and
 * a frame rigidly attached to the sensor.
 *
 * The definition of the lab or surface-fixed frame is sensor-specific.
 *
 * | Sensor Tag  |
 * |:-----------------:|
 * | `OrientationSensors` |
 */
class YARP_dev_API yarp::dev::IOrientationSensors
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
     * @return false if an error occurred, true otherwise.
     */
    virtual bool getOrientationSensorName(size_t sens_index, std::string &name) const = 0;

    /**
     * Get the name of the frame of the specified sensor.
     *
     * @note This is an implementation specific method, that may return the name of the sensor
     *       frame in a scenegraph
     *
     * @return false if an error occurred, true otherwise.
     */
    virtual bool getOrientationSensorFrameName(size_t sens_index, std::string &frameName) const = 0;

    /**
     * Get the last reading of the orientation sensor as roll pitch yaw.
     *
     * If \f$ f \f$ is the lab or surface fixed frame, and \f$ s \f$ is the sensor-fixed
     * frame, this methods returns the angles \f$ r \in [-180, 180] , p \in [-90, 90], y \in [-180, 180]\f$
     * such that
     * \f[
     * {}^f R_s = RotZ\left(\frac{\pi}{180}y\right)*RotY\left(\frac{\pi}{180}p\right)*RotX\left(\frac{\pi}{180}r\right)
     * \f]
     * with
     * \f[
     * RotZ(\theta)
     *  =
     *  \begin{bmatrix}
     *      \cos(\theta)      & -\sin(\theta) & 0              \\
     *      \sin(\theta)      & \cos(\theta)  & 0              \\
     *      0                 & 0             & 1              \\
     *  \end{bmatrix}
     * \f]
     * ,
     * \f[
     * RotY(\theta)
     * =
     *  \begin{bmatrix}
     *      \cos(\theta)      & 0             & \sin(\theta)   \\
     *      0                 & 1             & 0              \\
     *      -\sin(\theta)     & 0             & \cos(\theta)   \\
     *  \end{bmatrix}
     * \f]
     * and
     * \f[
     * RotX(\theta)
     * =
     *  \begin{bmatrix}
     *      1 & 0             & 0              \\
     *      0 & \cos(\theta)  & - \sin(\theta) \\
     *      0 & \sin(\theta)  & \cos(\theta)   \\
     *  \end{bmatrix}
     * \f]
     * where \f$ {}^f R_s \in \mathbb{R}^{3 \times 3} \f$ is the rotation that left-multiplied by a 3d column vector
     * expressed in \f$ s \f$ it returns it expressed in \f$ f \f$ .
     *
     * @param[in] sens_index The index of the specified sensor (should be between 0 and getNrOfOrientationSensors()-1).
     * @param[out] out The requested measure. The vector should be 3-dimensional. The measure is expressed in degrees .
     * @param[out] timestamp The timestamp of the requested measure, expressed in seconds.
     * @return false if an error occurred, true otherwise.
     */
    virtual bool getOrientationSensorMeasureAsRollPitchYaw(size_t sens_index, yarp::sig::Vector& rpy, double& timestamp) const = 0;

    virtual ~IOrientationSensors(){}
};


/**
 * @ingroup dev_iface_multiple_analog
 *
 * \brief Device interface to one or multiple temperature sensors.
 *
 * | Sensor Tag  |
 * |:-----------------:|
 * | `TemperatureSensors` |
 */
class YARP_dev_API yarp::dev::ITemperatureSensors
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
    virtual bool getTemperatureSensorName(size_t sens_index, std::string &name) const = 0;

    /**
     * Get the name of the frame of the specified sensor.
     */
    virtual bool getTemperatureSensorFrameName(size_t sens_index, std::string &frameName) const = 0;

    /**
     * Get the last reading of the specified sensor.
     * TODO(traversaro) : make the method swig-friendly
     */
    virtual bool getTemperatureSensorMeasure(size_t sens_index, double& out, double& timestamp) const = 0;

    /**
     * Get the last reading of the specified sensor.
     *
     * @param[in] sens_index The index of the specified sensor (should be between 0 and getNrOfTemperatureSensors()-1).
     * @param[out] out The requested measure. The vector should be 1-dimensional. The measure is expressed in celsius degrees .
     * @param[out] timestamp The timestamp of the requested measure, expressed in seconds.
     * @return false if an error occurred, true otherwise.
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
class YARP_dev_API yarp::dev::ISixAxisForceTorqueSensors
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
    virtual bool getSixAxisForceTorqueSensorName(size_t sens_index, std::string &name) const = 0;

    /**
     * Get the name of the frame of the specified sensor.
     */
    virtual bool getSixAxisForceTorqueSensorFrameName(size_t sens_index, std::string &frameName) const = 0;

    /**
     * Get the last reading of the specified sensor.
     *
     * @param[in] sens_index The index of the specified sensor (should be between 0 and getNrOfSixAxisForceTorqueSensors()-1).
     * @param[out] out The requested measure. The vector should be 6-dimensional.
     *                 The measure is expressed in Newton for the first three elements, Newton Meters for the last three elements.
     * @param[out] timestamp The timestamp of the requested measure, expressed in seconds.
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
class YARP_dev_API yarp::dev::IContactLoadCellArrays
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
     * @return false if an error occurred, true otherwise.
     */
    virtual bool getContactLoadCellArrayName(size_t sens_index, std::string &name) const = 0;

    /**
     * Get the last reading of the specified sensor.
     *
     * @param[in] sens_index The index of the specified sensor (should be between 0 and getContactLoadCellArrayMeasure()-1).
     * @param[out] out The requested measure. The vector should be getContactLoadCellArrayMeasure(sens_index)-dimensional.
     *                 The measure is expressed in Newton.
     * @param[out] timestamp The timestamp of the requested measure, expressed in seconds.
     * @return false if an error occurred, true otherwise.
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
 * @note This interface is typically used for group of encoders that are not
 *       explicitly controlled by one of the interfaces typically used for
 *       motor control, such as IEncoders and IPositionControl, such as
 *       encoders measuring the complete state in an underactuated mechanism.
 */
class YARP_dev_API yarp::dev::IEncoderArrays
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
    virtual bool getEncoderArrayName(size_t sens_index, std::string &name) const = 0;

    /**
     * Get the last reading of the specified sensor.
     *
     * @param[in] sens_index The index of the specified sensor (should be between 0 and getNrOfEncoderArrays()-1).
     * @param[out] out The requested measure. The vector should be getEncoderArraySize(sens_index)-dimensional.
     *                 The measure is expressed in Newton.
     * @param[out] timestamp The timestamp of the requested measure, expressed in seconds.
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
class YARP_dev_API yarp::dev::ISkinPatches
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
    virtual bool getSkinPatchName(size_t sens_index, std::string &name) const = 0;

    /**
     * Get the last reading of the specified sensor.
     *
     * @param[in] sens_index The index of the specified sensor (should be between 0 and getNrOfSkinPatches()-1).
     * @param[out] out The requested measure. The vector should be getNrOfSkinPatches(sens_index)-dimensional.
     *                 The measure is expressed in implementation-specific unit of measure.
     * @param[out] timestamp The timestamp of the requested measure, expressed in seconds.
     */
    virtual bool getSkinPatchMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const = 0;

    /**
     * Get the size of the specified skin patch
     */
    virtual size_t getSkinPatchSize(size_t sens_index) const = 0;


    virtual ~ISkinPatches(){}
};

#endif

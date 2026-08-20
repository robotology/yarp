/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef YARP_DEV_MULTIPLEANALOGSENSORSREMAPPER_MULTIPLEANALOGSENSORSREMAPPER_H
#define YARP_DEV_MULTIPLEANALOGSENSORSREMAPPER_MULTIPLEANALOGSENSORSREMAPPER_H

#include <yarp/dev/MultipleAnalogSensorsInterfaces.h>
#include <yarp/dev/IMultipleWrapper.h>
#include <yarp/dev/ReturnValue.h>

#include <vector>
#include <map>


/**
 * Internal identifier of the type of sensors.
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
    SkinPatches=8,
    PositionSensors=9,
    LinearVelocitySensors=10,
    ThreeAxisAngularAccelerometers=11
};

/**
 * @ingroup dev_impl_remappers
 *
 * \brief `multipleanalogsensorsremapper` : device that takes a list of sensors from multiple analog sensors device and exposes them as a single device exposing MultipleAnalogSensors interface.
 *
 * | YARP device name |
 * |:-----------------:|
 * | `multipleanalogsensorsremapper` |
 *
 *  Parameters required by this device are:
 * | Parameter name | SubParameter   | Type    | Units          | Default Value | Required                    | Description                                                       | Notes |
 * |:--------------:|:--------------:|:-------:|:--------------:|:-------------:|:--------------------------: |:-----------------------------------------------------------------:|:-----:|
 * | {sensorTag}Names |      -       | vector of strings  | -      |   -           | Yes     | Ordered list of name that must belong of the remapped device. The list also defines which index will be associated to a given sensor in the remapped devices. For example, if the list (sensorNameA,sensorNameB,sensorNameC) is given, these sensors will have respectively index 0, 1 and 2 in the remapped device.  |  |
 *
 * The sensorTag is a tag identifying the specific sensor interface, see \ref dev_iface_multiple_analog for a list of possible sensors.
 * The tag of each sensor interface is provided in the doxygen documentation of the specific interface.
 *
 * Configuration file using .ini format.
 *
 * \code{.unparsed}
 *  device multipleanalogsensorsremapper
 *  ThreeAxisGyroscopesNames (head torso)
 *  SixAxisForceTorqueSensorsNames (left_foot right_foot left_leg right_leg)
 * \endcode
 *
 * Configuration of the device from C++ code.
 * \code{.cpp}
 *   Property options;
 *   options.put("device","multipleanalogsensorsremapper");
 *   Bottle gyrosNames;
 *   Bottle & gyrosList = gyrosNames.addList();
 *   gyrosList.addString("head");
 *   gyrosList.addString("torso");
 *   options.put("ThreeAxisGyroscopesNames", gyrosNames.get(0));
 *   Bottle ftNames;
 *   Bottle & ftList = ftNames.addList();
 *   ftList.addString("left_foot");
 *   ftList.addString("right_foot");
 *   ftList.addString("left_leg");
 *   ftList.addString("right_leg");
 *   options.put("SixAxisForceTorqueSensorsNames", ftNames.get(0));
 *
 *   // Actually open the device
 *   PolyDriver multipleAnalogRemappedDevice(options);
 *
 *   // Use it as  you would use any controlboard device
 *   // ...
 * \endcode
 *
 */
class MultipleAnalogSensorsRemapper :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IMultipleWrapper,
        public yarp::dev::IThreeAxisGyroscopes,
        public yarp::dev::IThreeAxisLinearAccelerometers,
        public yarp::dev::IThreeAxisAngularAccelerometers,
        public yarp::dev::IThreeAxisMagnetometers,
        public yarp::dev::IOrientationSensors,
        public yarp::dev::ITemperatureSensors,
        public yarp::dev::ISixAxisForceTorqueSensors,
        public yarp::dev::IContactLoadCellArrays,
        public yarp::dev::IEncoderArrays,
        public yarp::dev::ISkinPatches,
        public yarp::dev::IPositionSensors,
        public yarp::dev::ILinearVelocitySensors
{
private:
    bool m_verbose{false};

    bool parseOptions(const yarp::os::Property& prop);

    // Map from remapped indices to underlyng subdevice indices
    class SensorInSubDevice
    {
    public:
        SensorInSubDevice(): subDevice(0), indexInSubDevice(0)
        {}
        SensorInSubDevice(size_t p_subDevice, size_t p_indexInSubDevice):
            subDevice(p_subDevice), indexInSubDevice(p_indexInSubDevice) {}

        size_t subDevice;
        size_t indexInSubDevice;
    };

    std::vector< std::vector<std::string> > m_remappedSensors;
    std::vector< std::vector<SensorInSubDevice> > m_indicesMap;

    std::vector<yarp::dev::IThreeAxisGyroscopes*> m_iThreeAxisGyroscopes;
    std::vector<yarp::dev::IThreeAxisLinearAccelerometers*> m_iThreeAxisLinearAccelerometers;
    std::vector<yarp::dev::IThreeAxisAngularAccelerometers*> m_iThreeAxisAngularAccelerometers;
    std::vector<yarp::dev::IThreeAxisMagnetometers*> m_iThreeAxisMagnetometers;
    std::vector<yarp::dev::IOrientationSensors*> m_iOrientationSensors;
    std::vector<yarp::dev::ITemperatureSensors*> m_iTemperatureSensors;
    std::vector<yarp::dev::ISixAxisForceTorqueSensors*> m_iSixAxisForceTorqueSensors;
    std::vector<yarp::dev::IContactLoadCellArrays*> m_iContactLoadCellArrays;
    std::vector<yarp::dev::IEncoderArrays*> m_iEncoderArrays;
    std::vector<yarp::dev::ISkinPatches*> m_iSkinPatches;
    std::vector<yarp::dev::IPositionSensors*> m_iPositionSensors;
    std::vector<yarp::dev::ILinearVelocitySensors*> m_iLinearVelocitySensors;


    // Templated methods to streamline of the function implementation for all the different sensors
    // This part is complicated, but is useful to avoid a huge code duplication
    // To understand this code, make sure that you are familiar with:
    //   * Method templates ( http://en.cppreference.com/w/cpp/language/member_template )
    //   * Pointer to method functions ( http://en.cppreference.com/w/cpp/language/pointer#Pointers_to_member_functions )
    template<typename Interface>
    yarp::dev::MAS_status genericGetStatus(const MAS_SensorType sensorType,
                                size_t& sens_index,
                                const std::vector<Interface *>& subDeviceVec,
                                yarp::dev::MAS_status (Interface::*methodPtr)(size_t) const) const;
    template<typename Interface>
    yarp::dev::ReturnValue genericGetName(const MAS_SensorType sensorType,
                                size_t& sens_index, std::string &name,
                                const std::vector<Interface *>& subDeviceVec,
                                yarp::dev::ReturnValue (Interface::*methodPtr)(size_t, std::string&) const) const;
    template<typename Interface>
    yarp::dev::ReturnValue genericGetFrameName(const MAS_SensorType sensorType,
                                size_t& sens_index, std::string &name,
                                const std::vector<Interface *>& subDeviceVec,
                                yarp::dev::ReturnValue (Interface::*methodPtr)(size_t, std::string&) const) const;
    template<typename Interface>
    yarp::dev::ReturnValue genericGetMeasure(const MAS_SensorType sensorType,
                                 size_t& sens_index, yarp::sig::Vector& out, double& timestamp,
                                 const std::vector<Interface *>& subDeviceVec,
                                 yarp::dev::ReturnValue (Interface::*methodPtr)(size_t, yarp::sig::Vector&, double&) const) const;
    template<typename Interface>
    size_t genericGetSize(const MAS_SensorType sensorType,
                                 size_t& sens_index,
                                 const std::vector<Interface *>& subDeviceVec,
                                 size_t (Interface::*methodPtr)(size_t) const) const;

    template<typename Interface>
    bool genericAttachAll(const MAS_SensorType sensorType,
                          std::vector<Interface *>& subDeviceVec,
                          const yarp::dev::PolyDriverList &polylist,
                          yarp::dev::ReturnValue (Interface::*getNameMethodPtr)(size_t, std::string&) const,
                          yarp::dev::ReturnValue (Interface::*getNrOfSensorsMethodPtr)(size_t& ) const);

public:
    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    /** MultipeWrapper methods */
    bool attachAll(const yarp::dev::PolyDriverList &p) override;
    bool detachAll() override;

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
};


#endif

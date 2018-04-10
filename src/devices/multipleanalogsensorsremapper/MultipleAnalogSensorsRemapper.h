/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */


#ifndef YARP_DEV_MULTIPLEANALOGSENSORSREMAPPER_MULTIPLEANALOGSENSORSREMAPPER_H
#define YARP_DEV_MULTIPLEANALOGSENSORSREMAPPER_MULTIPLEANALOGSENSORSREMAPPER_H

#include <yarp/dev/MultipleAnalogSensorsInterfaces.h>
#include <yarp/dev/Wrapper.h>

#include <vector>
#include <map>

namespace yarp {
namespace dev {

/**
* \brief `multipleanalogsensorsremapper` : device that takes a list of sensor from multiple analog sensors device and expose them as a single device exposing MultipleAnalogSensors interface.
* 
* | YARP device name |
* |:-----------------:|
* | `multipleanalogsensorsremapper` |
* 
*  Parameters required by this device are:
* | Parameter name | SubParameter   | Type    | Units          | Default Value | Required                    | Description                                                       | Notes |
* |:--------------:|:--------------:|:-------:|:--------------:|:-------------:|:--------------------------: |:-----------------------------------------------------------------:|:-----:|
* | {sensorTag}Names |      -       | vector of strings  | -      |   -           | Yes     | Ordered list of name  that must belong of the remapped device. The list also defines the index that the sensor will  |  |
*
* The sensorTag is a tag identifing the spefici sensor interface, see \ref dev_iface_multiple_analog for a list of possible sensors.
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
class MultipleAnalogSensorsRemapper: public yarp::dev::DeviceDriver,
                                     public yarp::dev::IMultipleWrapper,
                                     public yarp::dev::IThreeAxisGyroscopes,
                                     public yarp::dev::IThreeAxisLinearAccelerometers,
                                     public yarp::dev::IThreeAxisMagnetometers,
                                     public yarp::dev::IOrientationSensors,
                                     public yarp::dev::ITemperatureSensors,
                                     public yarp::dev::ISixAxisForceTorqueSensors,
                                     public yarp::dev::IContactLoadCellArrays,
                                     public yarp::dev::IEncoderArrays,
                                     public yarp::dev::ISkinPatches
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
    std::vector<yarp::dev::IThreeAxisMagnetometers*> m_iThreeAxisMagnetometers;
    std::vector<yarp::dev::IOrientationSensors*> m_iOrientationSensors;
    std::vector<yarp::dev::ITemperatureSensors*> m_iTemperatureSensors;
    std::vector<yarp::dev::ISixAxisForceTorqueSensors*> m_iSixAxisForceTorqueSensors;
    std::vector<yarp::dev::IContactLoadCellArrays*> m_iContactLoadCellArrays;
    std::vector<yarp::dev::IEncoderArrays*> m_iEncoderArrays;
    std::vector<yarp::dev::ISkinPatches*> m_iSkinPatches;

    // Templated methods to streamline of the function implementation for all the different sensors
    // This part is complicated, but is useful to avoid a huge code duplication
    // To understand this code, make sure that you are familiar with:
    //   * Method templates ( http://en.cppreference.com/w/cpp/language/member_template )
    //   * Pointer to method functions ( http://en.cppreference.com/w/cpp/language/pointer#Pointers_to_member_functions )
    template<typename Interface>
    MAS_status genericGetStatus(const MAS_SensorType sensorType,
                                size_t& sens_index,
                                const std::vector<Interface *>& subDeviceVec,
                                MAS_status (Interface::*methodPtr)(size_t) const) const;
    template<typename Interface>
    bool genericGetName(const MAS_SensorType sensorType,
                                size_t& sens_index, yarp::os::ConstString &name,
                                const std::vector<Interface *>& subDeviceVec,
                                bool (Interface::*methodPtr)(size_t, yarp::os::ConstString&) const) const;
    template<typename Interface>
    bool genericGetFrameName(const MAS_SensorType sensorType,
                                size_t& sens_index, yarp::os::ConstString &name,
                                const std::vector<Interface *>& subDeviceVec,
                                bool (Interface::*methodPtr)(size_t, yarp::os::ConstString&) const) const;
    template<typename Interface>
    bool genericGetMeasure(const MAS_SensorType sensorType,
                                 size_t& sens_index, yarp::sig::Vector& out, double& timestamp,
                                 const std::vector<Interface *>& subDeviceVec,
                                 bool (Interface::*methodPtr)(size_t, yarp::sig::Vector&, double&) const) const;
    template<typename Interface>
    size_t genericGetSize(const MAS_SensorType sensorType,
                                 size_t& sens_index,
                                 const std::vector<Interface *>& subDeviceVec,
                                 size_t (Interface::*methodPtr)(size_t) const) const;

    template<typename Interface>
    bool genericAttachAll(const MAS_SensorType sensorType,
                          std::vector<Interface *>& subDeviceVec,
                          const PolyDriverList &polylist,
                          bool (Interface::*getNameMethodPtr)(size_t, yarp::os::ConstString&) const,
                          size_t (Interface::*getNrOfSensorsMethodPtr)() const);

public:
    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    /** MultipeWrapper methods */
    virtual bool attachAll(const PolyDriverList &p) override;
    virtual bool detachAll() override;

    /* IThreeAxisGyroscopes methods */ 
    virtual size_t getNrOfThreeAxisGyroscopes() const override;
    virtual yarp::dev::MAS_status getThreeAxisGyroscopeStatus(size_t sens_index) const override;
    virtual bool getThreeAxisGyroscopeName(size_t sens_index, yarp::os::ConstString &name) const override;
    virtual bool getThreeAxisGyroscopeFrameName(size_t sens_index, yarp::os::ConstString &frameName) const override;
    virtual bool getThreeAxisGyroscopeMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;
    
    /* IThreeAxisLinearAccelerometers methods */ 
    virtual size_t getNrOfThreeAxisLinearAccelerometers() const override;
    virtual yarp::dev::MAS_status getThreeAxisLinearAccelerometerStatus(size_t sens_index) const override;
    virtual bool getThreeAxisLinearAccelerometerName(size_t sens_index, yarp::os::ConstString &name) const override;
    virtual bool getThreeAxisLinearAccelerometerFrameName(size_t sens_index, yarp::os::ConstString &frameName) const override;
    virtual bool getThreeAxisLinearAccelerometerMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;
    
    /* IThreeAxisMagnetometers methods */ 
    virtual size_t getNrOfThreeAxisMagnetometers() const override;
    virtual yarp::dev::MAS_status getThreeAxisMagnetometerStatus(size_t sens_index) const override;
    virtual bool getThreeAxisMagnetometerName(size_t sens_index, yarp::os::ConstString &name) const override;
    virtual bool getThreeAxisMagnetometerFrameName(size_t sens_index, yarp::os::ConstString &frameName) const override;
    virtual bool getThreeAxisMagnetometerMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;
    
    /* IOrientationSensors methods */ 
    virtual size_t getNrOfOrientationSensors() const override;
    virtual yarp::dev::MAS_status getOrientationSensorStatus(size_t sens_index) const override;
    virtual bool getOrientationSensorName(size_t sens_index, yarp::os::ConstString &name) const override;
    virtual bool getOrientationSensorFrameName(size_t sens_index, yarp::os::ConstString &frameName) const override;
    virtual bool getOrientationSensorMeasureAsRollPitchYaw(size_t sens_index, yarp::sig::Vector& rpy, double& timestamp) const override;
    
    /* ITemperatureSensors methods */
    virtual size_t getNrOfTemperatureSensors() const override;
    virtual yarp::dev::MAS_status getTemperatureSensorStatus(size_t sens_index) const override;
    virtual bool getTemperatureSensorName(size_t sens_index, yarp::os::ConstString &name) const override;
    virtual bool getTemperatureSensorFrameName(size_t sens_index, yarp::os::ConstString &frameName) const override;
    virtual bool getTemperatureSensorMeasure(size_t sens_index, double& out, double& timestamp) const override;
    virtual bool getTemperatureSensorMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;
    
    /* ISixAxisForceTorqueSensors */ 
    virtual size_t getNrOfSixAxisForceTorqueSensors() const override;
    virtual yarp::dev::MAS_status getSixAxisForceTorqueSensorStatus(size_t sens_index) const override;
    virtual bool getSixAxisForceTorqueSensorName(size_t sens_index, yarp::os::ConstString &name) const override;
    virtual bool getSixAxisForceTorqueSensorFrameName(size_t sens_index, yarp::os::ConstString &frame) const override;
    virtual bool getSixAxisForceTorqueSensorMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;

    /* IContactLoadCellArrays */
    virtual size_t getNrOfContactLoadCellArrays() const override;
    virtual yarp::dev::MAS_status getContactLoadCellArrayStatus(size_t sens_index) const override;
    virtual bool getContactLoadCellArrayName(size_t sens_index, yarp::os::ConstString &name) const override;
    virtual bool getContactLoadCellArrayMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;
    virtual size_t getContactLoadCellArraySize(size_t sens_index) const override;
    
    /* IEncoderArrays */
    virtual size_t getNrOfEncoderArrays() const override;
    virtual yarp::dev::MAS_status getEncoderArrayStatus(size_t sens_index) const override;
    virtual bool getEncoderArrayName(size_t sens_index, yarp::os::ConstString &name) const override;
    virtual bool getEncoderArrayMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;
    virtual size_t getEncoderArraySize(size_t sens_index) const override;
    
    /* ISkinPatches */
    virtual size_t getNrOfSkinPatches() const override;
    virtual yarp::dev::MAS_status getSkinPatchStatus(size_t sens_index) const override;
    virtual bool getSkinPatchName(size_t sens_index, yarp::os::ConstString &name) const override;
    virtual bool getSkinPatchMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;
    virtual size_t getSkinPatchSize(size_t sens_index) const override;
};

}
}


#endif

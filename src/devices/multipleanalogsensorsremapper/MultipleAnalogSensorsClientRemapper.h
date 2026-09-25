/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_MULTIPLEANALOGSENSORSREMAPPER_MULTIPLEANALOGSENSORSCLIENTREMAPPER_H
#define YARP_DEV_MULTIPLEANALOGSENSORSREMAPPER_MULTIPLEANALOGSENSORSCLIENTREMAPPER_H

#include <yarp/dev/PolyDriver.h>

#include "MultipleAnalogSensorsRemapper.h"

/**
 *  @ingroup dev_impl_network_clients
 *
 * @brief `MultipleAnalogSensorsClientRemapper` A device that takes a list of sensor, a list of all
 * multiple analog sensors client and expose them as a single device exposing MultipleAnalogSensors
 * interface.
 *
 * \section MultipleAnalogSensorsRemapper
 *
 *  Parameters required by this device are:
 * | Parameter name | SubParameter   | Type    | Units          | Default Value | Required     | Description                                                       | Notes |
 * |:--------------:|:--------------:|:-------:|:--------------:|:-------------:|:-----------: |:-----------------------------------------------------------------:|:-----:|
 * | {sensorTag}Names |      -       | vector of strings  | -      |   -           | Yes     | Ordered list of name  that must belong of the remapped device. The list also defines the index that the sensor will  |  |
 * | multipleAnalogSensorsClients |     -     | vector of strings  | -   |   -           | Yes          | List of remote prefix used by the MultipleAnalogSensorsClients.           | The element of this list are then passed as "remote" parameter to the MultipleAnalogSensorsClient device. |
 * | localPortPrefix |     -         | string             | -   |   -           | Yes          | All ports opened by this device will start with this prefix       |       |
 * | MULTIPLE_ANALOG_SENSORS_CLIENTS_OPTIONS | - | group              | -   |   -           | No           | Options that will be passed directly to the MultipleAnalogSensorsClient devices | |
 * The sensorTag is a tag identifing the spefici sensor interface, see \ref dev_iface_multiple_analog for a list of possible sensors.
 * The tag of each sensor interface is provided in the doxygen documentation of the specific interface.
 *
 *
 * Configuration file using .ini format.
 *
 * \code{.unparsed}
 *  device multipleanalogsensorsclientremapper
 *  ThreeAxisGyroscopesNames (l_foot_ft_gyro,  r_arm_ft_gyro)
 *  SixAxisForceTorqueSensorsNames (r_foot_ft,  r_arm_ft)
 *  multipleAnalogSensorsClients (/icub/left_foot/imu /icub/right_arm/imu)
 *
 *  [MULTIPLE_ANALOG_SENSORS_CLIENTS_OPTIONS]
 *  carrier udp
 *  timeout 0.2
 * ...
 * \endcode
 *
 * Configuration of the device from C++ code.
 * \code{.cpp}
 *   Property options;
 *   options.put("device","multipleanalogsensorsclientremapper");
 *   Bottle threeAxisGyroscopesNames;
 *   Bottle & threeAxisGyroscopesList = threeAxisGyroscopesNames.addList();
 *   threeAxisGyroscopesList.addString("l_foot_ft_gyro");
 *   threeAxisGyroscopesList.addString("r_arm_ft_gyro");
 *   options.put("ThreeAxisGyroscopesNames",threeAxisGyroscopesNames.get(0))
 *
 *   Bottle sixAxisForceTorqueSensorsNames;
 *   Bottle & sixAxisForceTorqueSensorsList = sixAxisForceTorqueSensorsNames.addList();
 *   sixAxisForceTorqueSensorsList.addString("l_foot_ft_gyro");
 *   sixAxisForceTorqueSensorsList.addString("r_arm_ft_gyro");
 *   options.put("SixAxisForceTorqueSensorsNames",sixAxisForceTorqueSensorsNames.get(0))
 *
 *   Bottle multipleAnalogSensorsClients;
 *   Bottle & multipleAnalogSensorsClientsList = multipleAnalogSensorsClients.addList();
 *   multipleAnalogSensorsClientsList.addString("/icub/left_foot/imu");
 *   multipleAnalogSensorsClientsList.addString("/icub/right_arm/imu");
 *   options.put("multipleAnalogSensorsClients",multipleAnalogSensorsClients.get(0));
 *
 *   options.put("localPortPrefix",/test");
 *
 *   Property & multipleAnalogSensorsClientsOpts = options.addGroup("MULTIPLE_ANALOG_SENSORS_CLIENTS_OPTIONS");
 *   multipleAnalogSensorsClientsOpts.put("carrier", "udp");
 *   multipleAnalogSensorsClientsOpts.put("timeout", 0.2);
 *
 *   // Actually open the device
 *   PolyDriver robotDevice(options);
 *
 *   // Use it as  you would use any controlboard device
 *   // ...
 * \endcode
 *
 */

class MultipleAnalogSensorsClientRemapper : public MultipleAnalogSensorsRemapper
{
private:
    /**
     * List of MultipleAnalogSensorsClient devices opened by the MultipleAnalogSensorsClientRemapper
     * device.
     */
    std::vector<yarp::dev::PolyDriver*> m_multipleAnalogSensorsClientsDevices;


    // Close all opened  MultipleAnalogSensorsClients
    void closeAllMultipleAnalogSensorsClients();

public:
    MultipleAnalogSensorsClientRemapper() = default;
    MultipleAnalogSensorsClientRemapper(const MultipleAnalogSensorsClientRemapper&) = delete;
    MultipleAnalogSensorsClientRemapper(MultipleAnalogSensorsClientRemapper&&) = delete;
    MultipleAnalogSensorsClientRemapper& operator=(const MultipleAnalogSensorsClientRemapper&) = delete;
    MultipleAnalogSensorsClientRemapper& operator=(MultipleAnalogSensorsClientRemapper&&) = delete;
    ~MultipleAnalogSensorsClientRemapper() = default;

   /**
     * Open the device driver.
     * @param prop is a Searchable object which contains the parameters.
     * Allowed parameters are described in the class documentation.
     */
    bool open(yarp::os::Searchable &prop) override;

    /**
     * Close the device driver by deallocating all resources and closing ports.
     * @return true if successful or false otherwise.
     */
    bool close() override;
};

#endif

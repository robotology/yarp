/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.

// Generated from the following "sensor_msgs/BatteryState" msg definition:
//   
//   # Constants are chosen to match the enums in the linux kernel
//   # defined in include/linux/power_supply.h as of version 3.7
//   # The one difference is for style reasons the constants are
//   # all uppercase not mixed case.
//   
//   # Power supply status constants
//   uint8 POWER_SUPPLY_STATUS_UNKNOWN = 0
//   uint8 POWER_SUPPLY_STATUS_CHARGING = 1
//   uint8 POWER_SUPPLY_STATUS_DISCHARGING = 2
//   uint8 POWER_SUPPLY_STATUS_NOT_CHARGING = 3
//   uint8 POWER_SUPPLY_STATUS_FULL = 4
//   
//   # Power supply health constants
//   uint8 POWER_SUPPLY_HEALTH_UNKNOWN = 0
//   uint8 POWER_SUPPLY_HEALTH_GOOD = 1
//   uint8 POWER_SUPPLY_HEALTH_OVERHEAT = 2
//   uint8 POWER_SUPPLY_HEALTH_DEAD = 3
//   uint8 POWER_SUPPLY_HEALTH_OVERVOLTAGE = 4
//   uint8 POWER_SUPPLY_HEALTH_UNSPEC_FAILURE = 5
//   uint8 POWER_SUPPLY_HEALTH_COLD = 6
//   uint8 POWER_SUPPLY_HEALTH_WATCHDOG_TIMER_EXPIRE = 7
//   uint8 POWER_SUPPLY_HEALTH_SAFETY_TIMER_EXPIRE = 8
//   
//   # Power supply technology (chemistry) constants
//   uint8 POWER_SUPPLY_TECHNOLOGY_UNKNOWN = 0
//   uint8 POWER_SUPPLY_TECHNOLOGY_NIMH = 1
//   uint8 POWER_SUPPLY_TECHNOLOGY_LION = 2
//   uint8 POWER_SUPPLY_TECHNOLOGY_LIPO = 3
//   uint8 POWER_SUPPLY_TECHNOLOGY_LIFE = 4
//   uint8 POWER_SUPPLY_TECHNOLOGY_NICD = 5
//   uint8 POWER_SUPPLY_TECHNOLOGY_LIMN = 6
//   
//   Header  header
//   float32 voltage          # Voltage in Volts (Mandatory)
//   float32 current          # Negative when discharging (A)  (If unmeasured NaN)
//   float32 charge           # Current charge in Ah  (If unmeasured NaN)
//   float32 capacity         # Capacity in Ah (last full capacity)  (If unmeasured NaN)
//   float32 design_capacity  # Capacity in Ah (design capacity)  (If unmeasured NaN)
//   float32 percentage       # Charge percentage on 0 to 1 range  (If unmeasured NaN)
//   uint8   power_supply_status     # The charging status as reported. Values defined above
//   uint8   power_supply_health     # The battery health metric. Values defined above
//   uint8   power_supply_technology # The battery chemistry. Values defined above
//   bool    present          # True if the battery is present
//   
//   float32[] cell_voltage   # An array of individual cell voltages for each cell in the pack
//                            # If individual voltages unknown but number of cells known set each to NaN
//   string location          # The location into which the battery is inserted. (slot number or plug)
//   string serial_number     # The best approximation of the battery serial number
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_sensor_msgs_BatteryState_h
#define YARP_ROSMSG_sensor_msgs_BatteryState_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>

namespace yarp {
namespace rosmsg {
namespace sensor_msgs {

class BatteryState : public yarp::os::idl::WirePortable
{
public:
    static const unsigned char POWER_SUPPLY_STATUS_UNKNOWN = 0;
    static const unsigned char POWER_SUPPLY_STATUS_CHARGING = 1;
    static const unsigned char POWER_SUPPLY_STATUS_DISCHARGING = 2;
    static const unsigned char POWER_SUPPLY_STATUS_NOT_CHARGING = 3;
    static const unsigned char POWER_SUPPLY_STATUS_FULL = 4;
    static const unsigned char POWER_SUPPLY_HEALTH_UNKNOWN = 0;
    static const unsigned char POWER_SUPPLY_HEALTH_GOOD = 1;
    static const unsigned char POWER_SUPPLY_HEALTH_OVERHEAT = 2;
    static const unsigned char POWER_SUPPLY_HEALTH_DEAD = 3;
    static const unsigned char POWER_SUPPLY_HEALTH_OVERVOLTAGE = 4;
    static const unsigned char POWER_SUPPLY_HEALTH_UNSPEC_FAILURE = 5;
    static const unsigned char POWER_SUPPLY_HEALTH_COLD = 6;
    static const unsigned char POWER_SUPPLY_HEALTH_WATCHDOG_TIMER_EXPIRE = 7;
    static const unsigned char POWER_SUPPLY_HEALTH_SAFETY_TIMER_EXPIRE = 8;
    static const unsigned char POWER_SUPPLY_TECHNOLOGY_UNKNOWN = 0;
    static const unsigned char POWER_SUPPLY_TECHNOLOGY_NIMH = 1;
    static const unsigned char POWER_SUPPLY_TECHNOLOGY_LION = 2;
    static const unsigned char POWER_SUPPLY_TECHNOLOGY_LIPO = 3;
    static const unsigned char POWER_SUPPLY_TECHNOLOGY_LIFE = 4;
    static const unsigned char POWER_SUPPLY_TECHNOLOGY_NICD = 5;
    static const unsigned char POWER_SUPPLY_TECHNOLOGY_LIMN = 6;
    yarp::rosmsg::std_msgs::Header header;
    yarp::os::NetFloat32 voltage;
    yarp::os::NetFloat32 current;
    yarp::os::NetFloat32 charge;
    yarp::os::NetFloat32 capacity;
    yarp::os::NetFloat32 design_capacity;
    yarp::os::NetFloat32 percentage;
    unsigned char power_supply_status;
    unsigned char power_supply_health;
    unsigned char power_supply_technology;
    bool present;
    std::vector<yarp::os::NetFloat32> cell_voltage;
    std::string location;
    std::string serial_number;

    BatteryState() :
            header(),
            voltage(0.0),
            current(0.0),
            charge(0.0),
            capacity(0.0),
            design_capacity(0.0),
            percentage(0.0),
            power_supply_status(0),
            power_supply_health(0),
            power_supply_technology(0),
            present(false),
            cell_voltage(),
            location(""),
            serial_number("")
    {
    }

    void clear()
    {
        // *** POWER_SUPPLY_STATUS_UNKNOWN ***

        // *** POWER_SUPPLY_STATUS_CHARGING ***

        // *** POWER_SUPPLY_STATUS_DISCHARGING ***

        // *** POWER_SUPPLY_STATUS_NOT_CHARGING ***

        // *** POWER_SUPPLY_STATUS_FULL ***

        // *** POWER_SUPPLY_HEALTH_UNKNOWN ***

        // *** POWER_SUPPLY_HEALTH_GOOD ***

        // *** POWER_SUPPLY_HEALTH_OVERHEAT ***

        // *** POWER_SUPPLY_HEALTH_DEAD ***

        // *** POWER_SUPPLY_HEALTH_OVERVOLTAGE ***

        // *** POWER_SUPPLY_HEALTH_UNSPEC_FAILURE ***

        // *** POWER_SUPPLY_HEALTH_COLD ***

        // *** POWER_SUPPLY_HEALTH_WATCHDOG_TIMER_EXPIRE ***

        // *** POWER_SUPPLY_HEALTH_SAFETY_TIMER_EXPIRE ***

        // *** POWER_SUPPLY_TECHNOLOGY_UNKNOWN ***

        // *** POWER_SUPPLY_TECHNOLOGY_NIMH ***

        // *** POWER_SUPPLY_TECHNOLOGY_LION ***

        // *** POWER_SUPPLY_TECHNOLOGY_LIPO ***

        // *** POWER_SUPPLY_TECHNOLOGY_LIFE ***

        // *** POWER_SUPPLY_TECHNOLOGY_NICD ***

        // *** POWER_SUPPLY_TECHNOLOGY_LIMN ***

        // *** header ***
        header.clear();

        // *** voltage ***
        voltage = 0.0;

        // *** current ***
        current = 0.0;

        // *** charge ***
        charge = 0.0;

        // *** capacity ***
        capacity = 0.0;

        // *** design_capacity ***
        design_capacity = 0.0;

        // *** percentage ***
        percentage = 0.0;

        // *** power_supply_status ***
        power_supply_status = 0;

        // *** power_supply_health ***
        power_supply_health = 0;

        // *** power_supply_technology ***
        power_supply_technology = 0;

        // *** present ***
        present = false;

        // *** cell_voltage ***
        cell_voltage.clear();

        // *** location ***
        location = "";

        // *** serial_number ***
        serial_number = "";
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** voltage ***
        if (!connection.expectBlock((char*)&voltage, 4)) {
            return false;
        }

        // *** current ***
        if (!connection.expectBlock((char*)&current, 4)) {
            return false;
        }

        // *** charge ***
        if (!connection.expectBlock((char*)&charge, 4)) {
            return false;
        }

        // *** capacity ***
        if (!connection.expectBlock((char*)&capacity, 4)) {
            return false;
        }

        // *** design_capacity ***
        if (!connection.expectBlock((char*)&design_capacity, 4)) {
            return false;
        }

        // *** percentage ***
        if (!connection.expectBlock((char*)&percentage, 4)) {
            return false;
        }

        // *** power_supply_status ***
        if (!connection.expectBlock((char*)&power_supply_status, 1)) {
            return false;
        }

        // *** power_supply_health ***
        if (!connection.expectBlock((char*)&power_supply_health, 1)) {
            return false;
        }

        // *** power_supply_technology ***
        if (!connection.expectBlock((char*)&power_supply_technology, 1)) {
            return false;
        }

        // *** present ***
        if (!connection.expectBlock((char*)&present, 1)) {
            return false;
        }

        // *** cell_voltage ***
        int len = connection.expectInt();
        cell_voltage.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&cell_voltage[0], sizeof(yarp::os::NetFloat32)*len)) {
            return false;
        }

        // *** location ***
        len = connection.expectInt();
        location.resize(len);
        if (!connection.expectBlock((char*)location.c_str(), len)) {
            return false;
        }

        // *** serial_number ***
        len = connection.expectInt();
        serial_number.resize(len);
        if (!connection.expectBlock((char*)serial_number.c_str(), len)) {
            return false;
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(35)) {
            return false;
        }

        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** voltage ***
        voltage = reader.expectDouble();

        // *** current ***
        current = reader.expectDouble();

        // *** charge ***
        charge = reader.expectDouble();

        // *** capacity ***
        capacity = reader.expectDouble();

        // *** design_capacity ***
        design_capacity = reader.expectDouble();

        // *** percentage ***
        percentage = reader.expectDouble();

        // *** power_supply_status ***
        power_supply_status = reader.expectInt();

        // *** power_supply_health ***
        power_supply_health = reader.expectInt();

        // *** power_supply_technology ***
        power_supply_technology = reader.expectInt();

        // *** present ***
        present = reader.expectInt();

        // *** cell_voltage ***
        if (connection.expectInt() != (BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE)) {
            return false;
        }
        int len = connection.expectInt();
        cell_voltage.resize(len);
        for (int i=0; i<len; i++) {
            cell_voltage[i] = (yarp::os::NetFloat32)connection.expectDouble();
        }

        // *** location ***
        if (!reader.readString(location)) {
            return false;
        }

        // *** serial_number ***
        if (!reader.readString(serial_number)) {
            return false;
        }

        return !connection.isError();
    }

    using yarp::os::idl::WirePortable::read;
    bool read(yarp::os::ConnectionReader& connection) override
    {
        return (connection.isBareMode() ? readBare(connection)
                                        : readBottle(connection));
    }

    bool writeBare(yarp::os::ConnectionWriter& connection) override
    {
        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** voltage ***
        connection.appendBlock((char*)&voltage, 4);

        // *** current ***
        connection.appendBlock((char*)&current, 4);

        // *** charge ***
        connection.appendBlock((char*)&charge, 4);

        // *** capacity ***
        connection.appendBlock((char*)&capacity, 4);

        // *** design_capacity ***
        connection.appendBlock((char*)&design_capacity, 4);

        // *** percentage ***
        connection.appendBlock((char*)&percentage, 4);

        // *** power_supply_status ***
        connection.appendBlock((char*)&power_supply_status, 1);

        // *** power_supply_health ***
        connection.appendBlock((char*)&power_supply_health, 1);

        // *** power_supply_technology ***
        connection.appendBlock((char*)&power_supply_technology, 1);

        // *** present ***
        connection.appendBlock((char*)&present, 1);

        // *** cell_voltage ***
        connection.appendInt(cell_voltage.size());
        if (cell_voltage.size()>0) {
            connection.appendExternalBlock((char*)&cell_voltage[0], sizeof(yarp::os::NetFloat32)*cell_voltage.size());
        }

        // *** location ***
        connection.appendInt(location.length());
        connection.appendExternalBlock((char*)location.c_str(), location.length());

        // *** serial_number ***
        connection.appendInt(serial_number.length());
        connection.appendExternalBlock((char*)serial_number.c_str(), serial_number.length());

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(35);

        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** voltage ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)voltage);

        // *** current ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)current);

        // *** charge ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)charge);

        // *** capacity ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)capacity);

        // *** design_capacity ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)design_capacity);

        // *** percentage ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)percentage);

        // *** power_supply_status ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)power_supply_status);

        // *** power_supply_health ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)power_supply_health);

        // *** power_supply_technology ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)power_supply_technology);

        // *** present ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)present);

        // *** cell_voltage ***
        connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE);
        connection.appendInt(cell_voltage.size());
        for (size_t i=0; i<cell_voltage.size(); i++) {
            connection.appendDouble((double)cell_voltage[i]);
        }

        // *** location ***
        connection.appendInt(BOTTLE_TAG_STRING);
        connection.appendInt(location.length());
        connection.appendExternalBlock((char*)location.c_str(), location.length());

        // *** serial_number ***
        connection.appendInt(BOTTLE_TAG_STRING);
        connection.appendInt(serial_number.length());
        connection.appendExternalBlock((char*)serial_number.c_str(), serial_number.length());

        connection.convertTextMode();
        return !connection.isError();
    }

    using yarp::os::idl::WirePortable::write;
    bool write(yarp::os::ConnectionWriter& connection) override
    {
        return (connection.isBareMode() ? writeBare(connection)
                                        : writeBottle(connection));
    }

    // This class will serialize ROS style or YARP style depending on protocol.
    // If you need to force a serialization style, use one of these classes:
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::BatteryState> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::BatteryState> bottleStyle;

    // Give source text for class, ROS will need this
    static yarp::os::ConstString typeText()
    {
        return yarp::os::ConstString("\
\n\
# Constants are chosen to match the enums in the linux kernel\n\
# defined in include/linux/power_supply.h as of version 3.7\n\
# The one difference is for style reasons the constants are\n\
# all uppercase not mixed case.\n\
\n\
# Power supply status constants\n\
uint8 POWER_SUPPLY_STATUS_UNKNOWN = 0\n\
uint8 POWER_SUPPLY_STATUS_CHARGING = 1\n\
uint8 POWER_SUPPLY_STATUS_DISCHARGING = 2\n\
uint8 POWER_SUPPLY_STATUS_NOT_CHARGING = 3\n\
uint8 POWER_SUPPLY_STATUS_FULL = 4\n\
\n\
# Power supply health constants\n\
uint8 POWER_SUPPLY_HEALTH_UNKNOWN = 0\n\
uint8 POWER_SUPPLY_HEALTH_GOOD = 1\n\
uint8 POWER_SUPPLY_HEALTH_OVERHEAT = 2\n\
uint8 POWER_SUPPLY_HEALTH_DEAD = 3\n\
uint8 POWER_SUPPLY_HEALTH_OVERVOLTAGE = 4\n\
uint8 POWER_SUPPLY_HEALTH_UNSPEC_FAILURE = 5\n\
uint8 POWER_SUPPLY_HEALTH_COLD = 6\n\
uint8 POWER_SUPPLY_HEALTH_WATCHDOG_TIMER_EXPIRE = 7\n\
uint8 POWER_SUPPLY_HEALTH_SAFETY_TIMER_EXPIRE = 8\n\
\n\
# Power supply technology (chemistry) constants\n\
uint8 POWER_SUPPLY_TECHNOLOGY_UNKNOWN = 0\n\
uint8 POWER_SUPPLY_TECHNOLOGY_NIMH = 1\n\
uint8 POWER_SUPPLY_TECHNOLOGY_LION = 2\n\
uint8 POWER_SUPPLY_TECHNOLOGY_LIPO = 3\n\
uint8 POWER_SUPPLY_TECHNOLOGY_LIFE = 4\n\
uint8 POWER_SUPPLY_TECHNOLOGY_NICD = 5\n\
uint8 POWER_SUPPLY_TECHNOLOGY_LIMN = 6\n\
\n\
Header  header\n\
float32 voltage          # Voltage in Volts (Mandatory)\n\
float32 current          # Negative when discharging (A)  (If unmeasured NaN)\n\
float32 charge           # Current charge in Ah  (If unmeasured NaN)\n\
float32 capacity         # Capacity in Ah (last full capacity)  (If unmeasured NaN)\n\
float32 design_capacity  # Capacity in Ah (design capacity)  (If unmeasured NaN)\n\
float32 percentage       # Charge percentage on 0 to 1 range  (If unmeasured NaN)\n\
uint8   power_supply_status     # The charging status as reported. Values defined above\n\
uint8   power_supply_health     # The battery health metric. Values defined above\n\
uint8   power_supply_technology # The battery chemistry. Values defined above\n\
bool    present          # True if the battery is present\n\
\n\
float32[] cell_voltage   # An array of individual cell voltages for each cell in the pack\n\
                         # If individual voltages unknown but number of cells known set each to NaN\n\
string location          # The location into which the battery is inserted. (slot number or plug)\n\
string serial_number     # The best approximation of the battery serial number\n\
") + yarp::os::ConstString("\n\
================================================================================\n\
MSG: std_msgs/Header\n\
") + yarp::rosmsg::std_msgs::Header::typeText();
    }

    yarp::os::ConstString getTypeText() const
    {
        return yarp::rosmsg::sensor_msgs::BatteryState::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("sensor_msgs/BatteryState", "sensor_msgs/BatteryState");
        typ.addProperty("md5sum", yarp::os::Value("417c393bc3ef8e9b2f3754f4962fe60e"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace sensor_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_sensor_msgs_BatteryState_h

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
#include <yarp/os/Type.h>
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
    static const std::uint8_t POWER_SUPPLY_STATUS_UNKNOWN = 0;
    static const std::uint8_t POWER_SUPPLY_STATUS_CHARGING = 1;
    static const std::uint8_t POWER_SUPPLY_STATUS_DISCHARGING = 2;
    static const std::uint8_t POWER_SUPPLY_STATUS_NOT_CHARGING = 3;
    static const std::uint8_t POWER_SUPPLY_STATUS_FULL = 4;
    static const std::uint8_t POWER_SUPPLY_HEALTH_UNKNOWN = 0;
    static const std::uint8_t POWER_SUPPLY_HEALTH_GOOD = 1;
    static const std::uint8_t POWER_SUPPLY_HEALTH_OVERHEAT = 2;
    static const std::uint8_t POWER_SUPPLY_HEALTH_DEAD = 3;
    static const std::uint8_t POWER_SUPPLY_HEALTH_OVERVOLTAGE = 4;
    static const std::uint8_t POWER_SUPPLY_HEALTH_UNSPEC_FAILURE = 5;
    static const std::uint8_t POWER_SUPPLY_HEALTH_COLD = 6;
    static const std::uint8_t POWER_SUPPLY_HEALTH_WATCHDOG_TIMER_EXPIRE = 7;
    static const std::uint8_t POWER_SUPPLY_HEALTH_SAFETY_TIMER_EXPIRE = 8;
    static const std::uint8_t POWER_SUPPLY_TECHNOLOGY_UNKNOWN = 0;
    static const std::uint8_t POWER_SUPPLY_TECHNOLOGY_NIMH = 1;
    static const std::uint8_t POWER_SUPPLY_TECHNOLOGY_LION = 2;
    static const std::uint8_t POWER_SUPPLY_TECHNOLOGY_LIPO = 3;
    static const std::uint8_t POWER_SUPPLY_TECHNOLOGY_LIFE = 4;
    static const std::uint8_t POWER_SUPPLY_TECHNOLOGY_NICD = 5;
    static const std::uint8_t POWER_SUPPLY_TECHNOLOGY_LIMN = 6;
    yarp::rosmsg::std_msgs::Header header;
    yarp::conf::float32_t voltage;
    yarp::conf::float32_t current;
    yarp::conf::float32_t charge;
    yarp::conf::float32_t capacity;
    yarp::conf::float32_t design_capacity;
    yarp::conf::float32_t percentage;
    std::uint8_t power_supply_status;
    std::uint8_t power_supply_health;
    std::uint8_t power_supply_technology;
    bool present;
    std::vector<yarp::conf::float32_t> cell_voltage;
    std::string location;
    std::string serial_number;

    BatteryState() :
            header(),
            voltage(0.0f),
            current(0.0f),
            charge(0.0f),
            capacity(0.0f),
            design_capacity(0.0f),
            percentage(0.0f),
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
        voltage = 0.0f;

        // *** current ***
        current = 0.0f;

        // *** charge ***
        charge = 0.0f;

        // *** capacity ***
        capacity = 0.0f;

        // *** design_capacity ***
        design_capacity = 0.0f;

        // *** percentage ***
        percentage = 0.0f;

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
        voltage = connection.expectFloat32();

        // *** current ***
        current = connection.expectFloat32();

        // *** charge ***
        charge = connection.expectFloat32();

        // *** capacity ***
        capacity = connection.expectFloat32();

        // *** design_capacity ***
        design_capacity = connection.expectFloat32();

        // *** percentage ***
        percentage = connection.expectFloat32();

        // *** power_supply_status ***
        power_supply_status = connection.expectInt8();

        // *** power_supply_health ***
        power_supply_health = connection.expectInt8();

        // *** power_supply_technology ***
        power_supply_technology = connection.expectInt8();

        // *** present ***
        if (!connection.expectBlock((char*)&present, 1)) {
            return false;
        }

        // *** cell_voltage ***
        int len = connection.expectInt32();
        cell_voltage.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&cell_voltage[0], sizeof(yarp::conf::float32_t)*len)) {
            return false;
        }

        // *** location ***
        len = connection.expectInt32();
        location.resize(len);
        if (!connection.expectBlock((char*)location.c_str(), len)) {
            return false;
        }

        // *** serial_number ***
        len = connection.expectInt32();
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
        voltage = reader.expectFloat32();

        // *** current ***
        current = reader.expectFloat32();

        // *** charge ***
        charge = reader.expectFloat32();

        // *** capacity ***
        capacity = reader.expectFloat32();

        // *** design_capacity ***
        design_capacity = reader.expectFloat32();

        // *** percentage ***
        percentage = reader.expectFloat32();

        // *** power_supply_status ***
        power_supply_status = reader.expectInt8();

        // *** power_supply_health ***
        power_supply_health = reader.expectInt8();

        // *** power_supply_technology ***
        power_supply_technology = reader.expectInt8();

        // *** present ***
        present = reader.expectInt8();

        // *** cell_voltage ***
        if (connection.expectInt32() != (BOTTLE_TAG_LIST|BOTTLE_TAG_FLOAT32)) {
            return false;
        }
        int len = connection.expectInt32();
        cell_voltage.resize(len);
        for (int i=0; i<len; i++) {
            cell_voltage[i] = (yarp::conf::float32_t)connection.expectFloat32();
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

    bool writeBare(yarp::os::ConnectionWriter& connection) const override
    {
        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** voltage ***
        connection.appendFloat32(voltage);

        // *** current ***
        connection.appendFloat32(current);

        // *** charge ***
        connection.appendFloat32(charge);

        // *** capacity ***
        connection.appendFloat32(capacity);

        // *** design_capacity ***
        connection.appendFloat32(design_capacity);

        // *** percentage ***
        connection.appendFloat32(percentage);

        // *** power_supply_status ***
        connection.appendInt8(power_supply_status);

        // *** power_supply_health ***
        connection.appendInt8(power_supply_health);

        // *** power_supply_technology ***
        connection.appendInt8(power_supply_technology);

        // *** present ***
        connection.appendBlock((char*)&present, 1);

        // *** cell_voltage ***
        connection.appendInt32(cell_voltage.size());
        if (cell_voltage.size()>0) {
            connection.appendExternalBlock((char*)&cell_voltage[0], sizeof(yarp::conf::float32_t)*cell_voltage.size());
        }

        // *** location ***
        connection.appendInt32(location.length());
        connection.appendExternalBlock((char*)location.c_str(), location.length());

        // *** serial_number ***
        connection.appendInt32(serial_number.length());
        connection.appendExternalBlock((char*)serial_number.c_str(), serial_number.length());

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(35);

        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** voltage ***
        connection.appendInt32(BOTTLE_TAG_FLOAT32);
        connection.appendFloat32(voltage);

        // *** current ***
        connection.appendInt32(BOTTLE_TAG_FLOAT32);
        connection.appendFloat32(current);

        // *** charge ***
        connection.appendInt32(BOTTLE_TAG_FLOAT32);
        connection.appendFloat32(charge);

        // *** capacity ***
        connection.appendInt32(BOTTLE_TAG_FLOAT32);
        connection.appendFloat32(capacity);

        // *** design_capacity ***
        connection.appendInt32(BOTTLE_TAG_FLOAT32);
        connection.appendFloat32(design_capacity);

        // *** percentage ***
        connection.appendInt32(BOTTLE_TAG_FLOAT32);
        connection.appendFloat32(percentage);

        // *** power_supply_status ***
        connection.appendInt32(BOTTLE_TAG_INT8);
        connection.appendInt8(power_supply_status);

        // *** power_supply_health ***
        connection.appendInt32(BOTTLE_TAG_INT8);
        connection.appendInt8(power_supply_health);

        // *** power_supply_technology ***
        connection.appendInt32(BOTTLE_TAG_INT8);
        connection.appendInt8(power_supply_technology);

        // *** present ***
        connection.appendInt32(BOTTLE_TAG_INT8);
        connection.appendInt8(present);

        // *** cell_voltage ***
        connection.appendInt32(BOTTLE_TAG_LIST|BOTTLE_TAG_FLOAT32);
        connection.appendInt32(cell_voltage.size());
        for (size_t i=0; i<cell_voltage.size(); i++) {
            connection.appendFloat32(cell_voltage[i]);
        }

        // *** location ***
        connection.appendInt32(BOTTLE_TAG_STRING);
        connection.appendInt32(location.length());
        connection.appendExternalBlock((char*)location.c_str(), location.length());

        // *** serial_number ***
        connection.appendInt32(BOTTLE_TAG_STRING);
        connection.appendInt32(serial_number.length());
        connection.appendExternalBlock((char*)serial_number.c_str(), serial_number.length());

        connection.convertTextMode();
        return !connection.isError();
    }

    using yarp::os::idl::WirePortable::write;
    bool write(yarp::os::ConnectionWriter& connection) const override
    {
        return (connection.isBareMode() ? writeBare(connection)
                                        : writeBottle(connection));
    }

    // This class will serialize ROS style or YARP style depending on protocol.
    // If you need to force a serialization style, use one of these classes:
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::BatteryState> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::BatteryState> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "sensor_msgs/BatteryState";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "476f837fa6771f6e16e3bf4ef96f8770";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
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
\n\
================================================================================\n\
MSG: std_msgs/Header\n\
# Standard metadata for higher-level stamped data types.\n\
# This is generally used to communicate timestamped data \n\
# in a particular coordinate frame.\n\
# \n\
# sequence ID: consecutively increasing ID \n\
uint32 seq\n\
#Two-integer timestamp that is expressed as:\n\
# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')\n\
# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')\n\
# time-handling sugar is provided by the client library\n\
time stamp\n\
#Frame this data is associated with\n\
# 0: no frame\n\
# 1: global frame\n\
string frame_id\n\
";

    yarp::os::Type getType() const override
    {
        yarp::os::Type typ = yarp::os::Type::byName(typeName, typeName);
        typ.addProperty("md5sum", yarp::os::Value(typeChecksum));
        typ.addProperty("message_definition", yarp::os::Value(typeText));
        return typ;
    }
};

} // namespace sensor_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_sensor_msgs_BatteryState_h

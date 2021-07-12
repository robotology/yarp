/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "sensor_msgs/JoyFeedback" msg definition:
//   # Declare of the type of feedback
//   uint8 TYPE_LED    = 0
//   uint8 TYPE_RUMBLE = 1
//   uint8 TYPE_BUZZER = 2
//   
//   uint8 type
//   
//   # This will hold an id number for each type of each feedback.
//   # Example, the first led would be id=0, the second would be id=1
//   uint8 id
//   
//   # Intensity of the feedback, from 0.0 to 1.0, inclusive.  If device is
//   # actually binary, driver should treat 0<=x<0.5 as off, 0.5<=x<=1 as on.
//   float32 intensity
//   
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_sensor_msgs_JoyFeedback_h
#define YARP_ROSMSG_sensor_msgs_JoyFeedback_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>

namespace yarp {
namespace rosmsg {
namespace sensor_msgs {

class JoyFeedback : public yarp::os::idl::WirePortable
{
public:
    static const std::uint8_t TYPE_LED = 0;
    static const std::uint8_t TYPE_RUMBLE = 1;
    static const std::uint8_t TYPE_BUZZER = 2;
    std::uint8_t type;
    std::uint8_t id;
    yarp::conf::float32_t intensity;

    JoyFeedback() :
            type(0),
            id(0),
            intensity(0.0f)
    {
    }

    void clear()
    {
        // *** TYPE_LED ***

        // *** TYPE_RUMBLE ***

        // *** TYPE_BUZZER ***

        // *** type ***
        type = 0;

        // *** id ***
        id = 0;

        // *** intensity ***
        intensity = 0.0f;
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** type ***
        type = connection.expectInt8();

        // *** id ***
        id = connection.expectInt8();

        // *** intensity ***
        intensity = connection.expectFloat32();

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(6)) {
            return false;
        }

        // *** type ***
        type = reader.expectInt8();

        // *** id ***
        id = reader.expectInt8();

        // *** intensity ***
        intensity = reader.expectFloat32();

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
        // *** type ***
        connection.appendInt8(type);

        // *** id ***
        connection.appendInt8(id);

        // *** intensity ***
        connection.appendFloat32(intensity);

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(6);

        // *** type ***
        connection.appendInt32(BOTTLE_TAG_INT8);
        connection.appendInt8(type);

        // *** id ***
        connection.appendInt32(BOTTLE_TAG_INT8);
        connection.appendInt8(id);

        // *** intensity ***
        connection.appendInt32(BOTTLE_TAG_FLOAT32);
        connection.appendFloat32(intensity);

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::JoyFeedback> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::JoyFeedback> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "sensor_msgs/JoyFeedback";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "f4dcd73460360d98f36e55ee7f2e46f1";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
# Declare of the type of feedback\n\
uint8 TYPE_LED    = 0\n\
uint8 TYPE_RUMBLE = 1\n\
uint8 TYPE_BUZZER = 2\n\
\n\
uint8 type\n\
\n\
# This will hold an id number for each type of each feedback.\n\
# Example, the first led would be id=0, the second would be id=1\n\
uint8 id\n\
\n\
# Intensity of the feedback, from 0.0 to 1.0, inclusive.  If device is\n\
# actually binary, driver should treat 0<=x<0.5 as off, 0.5<=x<=1 as on.\n\
float32 intensity\n\
\n\
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

#endif // YARP_ROSMSG_sensor_msgs_JoyFeedback_h

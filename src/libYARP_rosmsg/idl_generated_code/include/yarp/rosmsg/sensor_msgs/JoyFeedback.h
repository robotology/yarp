/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>

namespace yarp {
namespace rosmsg {
namespace sensor_msgs {

class JoyFeedback : public yarp::os::idl::WirePortable
{
public:
    static const unsigned char TYPE_LED = 0;
    static const unsigned char TYPE_RUMBLE = 1;
    static const unsigned char TYPE_BUZZER = 2;
    unsigned char type;
    unsigned char id;
    yarp::os::NetFloat32 intensity;

    JoyFeedback() :
            type(0),
            id(0),
            intensity(0.0)
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
        intensity = 0.0;
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** type ***
        if (!connection.expectBlock((char*)&type, 1)) {
            return false;
        }

        // *** id ***
        if (!connection.expectBlock((char*)&id, 1)) {
            return false;
        }

        // *** intensity ***
        if (!connection.expectBlock((char*)&intensity, 4)) {
            return false;
        }

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
        type = reader.expectInt();

        // *** id ***
        id = reader.expectInt();

        // *** intensity ***
        intensity = reader.expectDouble();

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
        // *** type ***
        connection.appendBlock((char*)&type, 1);

        // *** id ***
        connection.appendBlock((char*)&id, 1);

        // *** intensity ***
        connection.appendBlock((char*)&intensity, 4);

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(6);

        // *** type ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)type);

        // *** id ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)id);

        // *** intensity ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)intensity);

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::JoyFeedback> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::JoyFeedback> bottleStyle;

    // Give source text for class, ROS will need this
    static std::string typeText()
    {
        return std::string("\
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
");
    }

    std::string getTypeText() const
    {
        return yarp::rosmsg::sensor_msgs::JoyFeedback::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("sensor_msgs/JoyFeedback", "sensor_msgs/JoyFeedback");
        typ.addProperty("md5sum", yarp::os::Value("f4dcd73460360d98f36e55ee7f2e46f1"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace sensor_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_sensor_msgs_JoyFeedback_h

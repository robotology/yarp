/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "sensor_msgs/JoyFeedbackArray" msg definition:
//   # This message publishes values for multiple feedback at once. 
//   JoyFeedback[] array// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_sensor_msgs_JoyFeedbackArray_h
#define YARP_ROSMSG_sensor_msgs_JoyFeedbackArray_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/sensor_msgs/JoyFeedback.h>

namespace yarp {
namespace rosmsg {
namespace sensor_msgs {

class JoyFeedbackArray : public yarp::os::idl::WirePortable
{
public:
    std::vector<yarp::rosmsg::sensor_msgs::JoyFeedback> array;

    JoyFeedbackArray() :
            array()
    {
    }

    void clear()
    {
        // *** array ***
        array.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** array ***
        int len = connection.expectInt32();
        array.resize(len);
        for (int i=0; i<len; i++) {
            if (!array[i].read(connection)) {
                return false;
            }
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(1)) {
            return false;
        }

        // *** array ***
        if (connection.expectInt32() != BOTTLE_TAG_LIST) {
            return false;
        }
        int len = connection.expectInt32();
        array.resize(len);
        for (int i=0; i<len; i++) {
            if (!array[i].read(connection)) {
                return false;
            }
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
        // *** array ***
        connection.appendInt32(array.size());
        for (size_t i=0; i<array.size(); i++) {
            if (!array[i].write(connection)) {
                return false;
            }
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(1);

        // *** array ***
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(array.size());
        for (size_t i=0; i<array.size(); i++) {
            if (!array[i].write(connection)) {
                return false;
            }
        }

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::JoyFeedbackArray> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::JoyFeedbackArray> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "sensor_msgs/JoyFeedbackArray";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "cde5730a895b1fc4dee6f91b754b213d";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
# This message publishes values for multiple feedback at once. \n\
JoyFeedback[] array\n\
================================================================================\n\
MSG: sensor_msgs/JoyFeedback\n\
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

#endif // YARP_ROSMSG_sensor_msgs_JoyFeedbackArray_h

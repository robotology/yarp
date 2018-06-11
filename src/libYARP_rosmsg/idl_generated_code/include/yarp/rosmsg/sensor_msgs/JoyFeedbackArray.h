/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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

    // Give source text for class, ROS will need this
    static std::string typeText()
    {
        return std::string("\
# This message publishes values for multiple feedback at once. \n\
JoyFeedback[] array") + std::string("\n\
================================================================================\n\
MSG: sensor_msgs/JoyFeedback\n\
") + yarp::rosmsg::sensor_msgs::JoyFeedback::typeText();
    }

    std::string getTypeText() const
    {
        return yarp::rosmsg::sensor_msgs::JoyFeedbackArray::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() const override
    {
        yarp::os::Type typ = yarp::os::Type::byName("sensor_msgs/JoyFeedbackArray", "sensor_msgs/JoyFeedbackArray");
        typ.addProperty("md5sum", yarp::os::Value("cde5730a895b1fc4dee6f91b754b213d"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace sensor_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_sensor_msgs_JoyFeedbackArray_h

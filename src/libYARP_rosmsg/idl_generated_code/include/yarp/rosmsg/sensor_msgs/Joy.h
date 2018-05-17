/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.

// Generated from the following "sensor_msgs/Joy" msg definition:
//   # Reports the state of a joysticks axes and buttons.
//   Header header           # timestamp in the header is the time the data is received from the joystick
//   float32[] axes          # the axes measurements from a joystick
//   int32[] buttons         # the buttons measurements from a joystick 
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_sensor_msgs_Joy_h
#define YARP_ROSMSG_sensor_msgs_Joy_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>

namespace yarp {
namespace rosmsg {
namespace sensor_msgs {

class Joy : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::std_msgs::Header header;
    std::vector<yarp::os::NetFloat32> axes;
    std::vector<yarp::os::NetInt32> buttons;

    Joy() :
            header(),
            axes(),
            buttons()
    {
    }

    void clear()
    {
        // *** header ***
        header.clear();

        // *** axes ***
        axes.clear();

        // *** buttons ***
        buttons.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** axes ***
        int len = connection.expectInt();
        axes.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&axes[0], sizeof(yarp::os::NetFloat32)*len)) {
            return false;
        }

        // *** buttons ***
        len = connection.expectInt();
        buttons.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&buttons[0], sizeof(yarp::os::NetInt32)*len)) {
            return false;
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(3)) {
            return false;
        }

        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** axes ***
        if (connection.expectInt() != (BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE)) {
            return false;
        }
        int len = connection.expectInt();
        axes.resize(len);
        for (int i=0; i<len; i++) {
            axes[i] = (yarp::os::NetFloat32)connection.expectDouble();
        }

        // *** buttons ***
        if (connection.expectInt() != (BOTTLE_TAG_LIST|BOTTLE_TAG_INT)) {
            return false;
        }
        len = connection.expectInt();
        buttons.resize(len);
        for (int i=0; i<len; i++) {
            buttons[i] = (yarp::os::NetInt32)connection.expectInt();
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

        // *** axes ***
        connection.appendInt(axes.size());
        if (axes.size()>0) {
            connection.appendExternalBlock((char*)&axes[0], sizeof(yarp::os::NetFloat32)*axes.size());
        }

        // *** buttons ***
        connection.appendInt(buttons.size());
        if (buttons.size()>0) {
            connection.appendExternalBlock((char*)&buttons[0], sizeof(yarp::os::NetInt32)*buttons.size());
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(3);

        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** axes ***
        connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE);
        connection.appendInt(axes.size());
        for (size_t i=0; i<axes.size(); i++) {
            connection.appendDouble((double)axes[i]);
        }

        // *** buttons ***
        connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_INT);
        connection.appendInt(buttons.size());
        for (size_t i=0; i<buttons.size(); i++) {
            connection.appendInt((int)buttons[i]);
        }

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::Joy> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::Joy> bottleStyle;

    // Give source text for class, ROS will need this
    static std::string typeText()
    {
        return std::string("\
# Reports the state of a joysticks axes and buttons.\n\
Header header           # timestamp in the header is the time the data is received from the joystick\n\
float32[] axes          # the axes measurements from a joystick\n\
int32[] buttons         # the buttons measurements from a joystick \n\
") + std::string("\n\
================================================================================\n\
MSG: std_msgs/Header\n\
") + yarp::rosmsg::std_msgs::Header::typeText();
    }

    std::string getTypeText() const
    {
        return yarp::rosmsg::sensor_msgs::Joy::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("sensor_msgs/Joy", "sensor_msgs/Joy");
        typ.addProperty("md5sum", yarp::os::Value("5a9ea5f83505693b71e785041e67a8bb"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace sensor_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_sensor_msgs_Joy_h

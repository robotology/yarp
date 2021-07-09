/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
#include <yarp/os/Type.h>
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
    std::vector<yarp::conf::float32_t> axes;
    std::vector<std::int32_t> buttons;

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
        int len = connection.expectInt32();
        axes.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&axes[0], sizeof(yarp::conf::float32_t)*len)) {
            return false;
        }

        // *** buttons ***
        len = connection.expectInt32();
        buttons.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&buttons[0], sizeof(std::int32_t)*len)) {
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
        if (connection.expectInt32() != (BOTTLE_TAG_LIST|BOTTLE_TAG_FLOAT32)) {
            return false;
        }
        int len = connection.expectInt32();
        axes.resize(len);
        for (int i=0; i<len; i++) {
            axes[i] = (yarp::conf::float32_t)connection.expectFloat32();
        }

        // *** buttons ***
        if (connection.expectInt32() != (BOTTLE_TAG_LIST|BOTTLE_TAG_INT32)) {
            return false;
        }
        len = connection.expectInt32();
        buttons.resize(len);
        for (int i=0; i<len; i++) {
            buttons[i] = (std::int32_t)connection.expectInt32();
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

        // *** axes ***
        connection.appendInt32(axes.size());
        if (axes.size()>0) {
            connection.appendExternalBlock((char*)&axes[0], sizeof(yarp::conf::float32_t)*axes.size());
        }

        // *** buttons ***
        connection.appendInt32(buttons.size());
        if (buttons.size()>0) {
            connection.appendExternalBlock((char*)&buttons[0], sizeof(std::int32_t)*buttons.size());
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(3);

        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** axes ***
        connection.appendInt32(BOTTLE_TAG_LIST|BOTTLE_TAG_FLOAT32);
        connection.appendInt32(axes.size());
        for (size_t i=0; i<axes.size(); i++) {
            connection.appendFloat32(axes[i]);
        }

        // *** buttons ***
        connection.appendInt32(BOTTLE_TAG_LIST|BOTTLE_TAG_INT32);
        connection.appendInt32(buttons.size());
        for (size_t i=0; i<buttons.size(); i++) {
            connection.appendInt32(buttons[i]);
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::Joy> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::Joy> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "sensor_msgs/Joy";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "5a9ea5f83505693b71e785041e67a8bb";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
# Reports the state of a joysticks axes and buttons.\n\
Header header           # timestamp in the header is the time the data is received from the joystick\n\
float32[] axes          # the axes measurements from a joystick\n\
int32[] buttons         # the buttons measurements from a joystick \n\
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

#endif // YARP_ROSMSG_sensor_msgs_Joy_h

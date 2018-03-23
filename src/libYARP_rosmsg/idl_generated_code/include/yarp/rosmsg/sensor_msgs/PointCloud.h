/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.

// Generated from the following "sensor_msgs/PointCloud" msg definition:
//   # This message holds a collection of 3d points, plus optional additional
//   # information about each point.
//   
//   # Time of sensor data acquisition, coordinate frame ID.
//   Header header
//   
//   # Array of 3d points. Each Point32 should be interpreted as a 3d point
//   # in the frame given in the header.
//   geometry_msgs/Point32[] points
//   
//   # Each channel should have the same number of elements as points array,
//   # and the data in each channel should correspond 1:1 with each point.
//   # Channel names in common practice are listed in ChannelFloat32.msg.
//   ChannelFloat32[] channels
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_sensor_msgs_PointCloud_h
#define YARP_ROSMSG_sensor_msgs_PointCloud_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>
#include <yarp/rosmsg/geometry_msgs/Point32.h>
#include <yarp/rosmsg/sensor_msgs/ChannelFloat32.h>

namespace yarp {
namespace rosmsg {
namespace sensor_msgs {

class PointCloud : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::std_msgs::Header header;
    std::vector<yarp::rosmsg::geometry_msgs::Point32> points;
    std::vector<yarp::rosmsg::sensor_msgs::ChannelFloat32> channels;

    PointCloud() :
            header(),
            points(),
            channels()
    {
    }

    void clear()
    {
        // *** header ***
        header.clear();

        // *** points ***
        points.clear();

        // *** channels ***
        channels.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** points ***
        int len = connection.expectInt();
        points.resize(len);
        for (int i=0; i<len; i++) {
            if (!points[i].read(connection)) {
                return false;
            }
        }

        // *** channels ***
        len = connection.expectInt();
        channels.resize(len);
        for (int i=0; i<len; i++) {
            if (!channels[i].read(connection)) {
                return false;
            }
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

        // *** points ***
        if (connection.expectInt() != BOTTLE_TAG_LIST) {
            return false;
        }
        int len = connection.expectInt();
        points.resize(len);
        for (int i=0; i<len; i++) {
            if (!points[i].read(connection)) {
                return false;
            }
        }

        // *** channels ***
        if (connection.expectInt() != BOTTLE_TAG_LIST) {
            return false;
        }
        len = connection.expectInt();
        channels.resize(len);
        for (int i=0; i<len; i++) {
            if (!channels[i].read(connection)) {
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

    bool writeBare(yarp::os::ConnectionWriter& connection) override
    {
        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** points ***
        connection.appendInt(points.size());
        for (size_t i=0; i<points.size(); i++) {
            if (!points[i].write(connection)) {
                return false;
            }
        }

        // *** channels ***
        connection.appendInt(channels.size());
        for (size_t i=0; i<channels.size(); i++) {
            if (!channels[i].write(connection)) {
                return false;
            }
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

        // *** points ***
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(points.size());
        for (size_t i=0; i<points.size(); i++) {
            if (!points[i].write(connection)) {
                return false;
            }
        }

        // *** channels ***
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(channels.size());
        for (size_t i=0; i<channels.size(); i++) {
            if (!channels[i].write(connection)) {
                return false;
            }
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::PointCloud> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::PointCloud> bottleStyle;

    // Give source text for class, ROS will need this
    static yarp::os::ConstString typeText()
    {
        return yarp::os::ConstString("\
# This message holds a collection of 3d points, plus optional additional\n\
# information about each point.\n\
\n\
# Time of sensor data acquisition, coordinate frame ID.\n\
Header header\n\
\n\
# Array of 3d points. Each Point32 should be interpreted as a 3d point\n\
# in the frame given in the header.\n\
geometry_msgs/Point32[] points\n\
\n\
# Each channel should have the same number of elements as points array,\n\
# and the data in each channel should correspond 1:1 with each point.\n\
# Channel names in common practice are listed in ChannelFloat32.msg.\n\
ChannelFloat32[] channels\n\
") + yarp::os::ConstString("\n\
================================================================================\n\
MSG: std_msgs/Header\n\
") + yarp::rosmsg::std_msgs::Header::typeText() + yarp::os::ConstString("\n\
================================================================================\n\
MSG: geometry_msgs/Point32\n\
") + yarp::rosmsg::geometry_msgs::Point32::typeText() + yarp::os::ConstString("\n\
================================================================================\n\
MSG: sensor_msgs/ChannelFloat32\n\
") + yarp::rosmsg::sensor_msgs::ChannelFloat32::typeText();
    }

    yarp::os::ConstString getTypeText() const
    {
        return yarp::rosmsg::sensor_msgs::PointCloud::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("sensor_msgs/PointCloud", "sensor_msgs/PointCloud");
        typ.addProperty("md5sum", yarp::os::Value("4a210f70acdf10eaffc5341a10892a11"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace sensor_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_sensor_msgs_PointCloud_h

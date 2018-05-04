/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.

// Generated from the following "sensor_msgs/SetCameraInfo" msg definition:
//   # This service requests that a camera stores the given CameraInfo 
//   # as that camera's calibration information.
//   #
//   # The width and height in the camera_info field should match what the
//   # camera is currently outputting on its camera_info topic, and the camera
//   # will assume that the region of the imager that is being referred to is
//   # the region that the camera is currently capturing.
//   
//   sensor_msgs/CameraInfo camera_info # The camera_info to store
//   ---
//   bool success          # True if the call succeeded
//   string status_message # Used to give details about success
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_sensor_msgs_SetCameraInfo_h
#define YARP_ROSMSG_sensor_msgs_SetCameraInfo_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/sensor_msgs/CameraInfo.h>

namespace yarp {
namespace rosmsg {
namespace sensor_msgs {

class SetCameraInfo : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::sensor_msgs::CameraInfo camera_info;

    SetCameraInfo() :
            camera_info()
    {
    }

    void clear()
    {
        // *** camera_info ***
        camera_info.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** camera_info ***
        if (!camera_info.read(connection)) {
            return false;
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

        // *** camera_info ***
        if (!camera_info.read(connection)) {
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
        // *** camera_info ***
        if (!camera_info.write(connection)) {
            return false;
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(1);

        // *** camera_info ***
        if (!camera_info.write(connection)) {
            return false;
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::SetCameraInfo> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::SetCameraInfo> bottleStyle;

    // Give source text for class, ROS will need this
    static yarp::os::ConstString typeText()
    {
        return yarp::os::ConstString("\
# This service requests that a camera stores the given CameraInfo \n\
# as that camera's calibration information.\n\
#\n\
# The width and height in the camera_info field should match what the\n\
# camera is currently outputting on its camera_info topic, and the camera\n\
# will assume that the region of the imager that is being referred to is\n\
# the region that the camera is currently capturing.\n\
\n\
sensor_msgs/CameraInfo camera_info # The camera_info to store\n\
---\n\
bool success          # True if the call succeeded\n\
string status_message # Used to give details about success\n\
") + yarp::os::ConstString("\n\
================================================================================\n\
MSG: sensor_msgs/CameraInfo\n\
") + yarp::rosmsg::sensor_msgs::CameraInfo::typeText();
    }

    yarp::os::ConstString getTypeText() const
    {
        return yarp::rosmsg::sensor_msgs::SetCameraInfo::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("sensor_msgs/SetCameraInfo", "sensor_msgs/SetCameraInfo");
        typ.addProperty("md5sum", yarp::os::Value("ee34be01fdeee563d0d99cd594d5581d"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace sensor_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_sensor_msgs_SetCameraInfo_h

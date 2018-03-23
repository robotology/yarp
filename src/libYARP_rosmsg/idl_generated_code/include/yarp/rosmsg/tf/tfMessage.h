/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.

// Generated from the following "tf/tfMessage" msg definition:
//   geometry_msgs/TransformStamped[] transforms
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_tf_tfMessage_h
#define YARP_ROSMSG_tf_tfMessage_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/geometry_msgs/TransformStamped.h>

namespace yarp {
namespace rosmsg {
namespace tf {

class tfMessage : public yarp::os::idl::WirePortable
{
public:
    std::vector<yarp::rosmsg::geometry_msgs::TransformStamped> transforms;

    tfMessage() :
            transforms()
    {
    }

    void clear()
    {
        // *** transforms ***
        transforms.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** transforms ***
        int len = connection.expectInt();
        transforms.resize(len);
        for (int i=0; i<len; i++) {
            if (!transforms[i].read(connection)) {
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

        // *** transforms ***
        if (connection.expectInt() != BOTTLE_TAG_LIST) {
            return false;
        }
        int len = connection.expectInt();
        transforms.resize(len);
        for (int i=0; i<len; i++) {
            if (!transforms[i].read(connection)) {
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
        // *** transforms ***
        connection.appendInt(transforms.size());
        for (size_t i=0; i<transforms.size(); i++) {
            if (!transforms[i].write(connection)) {
                return false;
            }
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(1);

        // *** transforms ***
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(transforms.size());
        for (size_t i=0; i<transforms.size(); i++) {
            if (!transforms[i].write(connection)) {
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::tf::tfMessage> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::tf::tfMessage> bottleStyle;

    // Give source text for class, ROS will need this
    static yarp::os::ConstString typeText()
    {
        return yarp::os::ConstString("\
geometry_msgs/TransformStamped[] transforms\n\
") + yarp::os::ConstString("\n\
================================================================================\n\
MSG: geometry_msgs/TransformStamped\n\
") + yarp::rosmsg::geometry_msgs::TransformStamped::typeText();
    }

    yarp::os::ConstString getTypeText() const
    {
        return yarp::rosmsg::tf::tfMessage::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("tf/tfMessage", "tf/tfMessage");
        typ.addProperty("md5sum", yarp::os::Value("8c6a1a3d0e669cd77bc1c3c5819f493b"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace tf
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_tf_tfMessage_h

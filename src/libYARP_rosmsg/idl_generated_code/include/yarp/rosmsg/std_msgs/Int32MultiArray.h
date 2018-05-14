/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.

// Generated from the following "std_msgs/Int32MultiArray" msg definition:
//   # Please look at the MultiArrayLayout message definition for
//   # documentation on all multiarrays.
//   
//   MultiArrayLayout  layout        # specification of data layout
//   int32[]           data          # array of data
//   
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_std_msgs_Int32MultiArray_h
#define YARP_ROSMSG_std_msgs_Int32MultiArray_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/MultiArrayLayout.h>

namespace yarp {
namespace rosmsg {
namespace std_msgs {

class Int32MultiArray : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::std_msgs::MultiArrayLayout layout;
    std::vector<yarp::os::NetInt32> data;

    Int32MultiArray() :
            layout(),
            data()
    {
    }

    void clear()
    {
        // *** layout ***
        layout.clear();

        // *** data ***
        data.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** layout ***
        if (!layout.read(connection)) {
            return false;
        }

        // *** data ***
        int len = connection.expectInt();
        data.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&data[0], sizeof(yarp::os::NetInt32)*len)) {
            return false;
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(2)) {
            return false;
        }

        // *** layout ***
        if (!layout.read(connection)) {
            return false;
        }

        // *** data ***
        if (connection.expectInt() != (BOTTLE_TAG_LIST|BOTTLE_TAG_INT)) {
            return false;
        }
        int len = connection.expectInt();
        data.resize(len);
        for (int i=0; i<len; i++) {
            data[i] = (yarp::os::NetInt32)connection.expectInt();
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
        // *** layout ***
        if (!layout.write(connection)) {
            return false;
        }

        // *** data ***
        connection.appendInt(data.size());
        if (data.size()>0) {
            connection.appendExternalBlock((char*)&data[0], sizeof(yarp::os::NetInt32)*data.size());
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(2);

        // *** layout ***
        if (!layout.write(connection)) {
            return false;
        }

        // *** data ***
        connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_INT);
        connection.appendInt(data.size());
        for (size_t i=0; i<data.size(); i++) {
            connection.appendInt((int)data[i]);
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::std_msgs::Int32MultiArray> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::std_msgs::Int32MultiArray> bottleStyle;

    // Give source text for class, ROS will need this
    static std::string typeText()
    {
        return std::string("\
# Please look at the MultiArrayLayout message definition for\n\
# documentation on all multiarrays.\n\
\n\
MultiArrayLayout  layout        # specification of data layout\n\
int32[]           data          # array of data\n\
\n\
") + std::string("\n\
================================================================================\n\
MSG: std_msgs/MultiArrayLayout\n\
") + yarp::rosmsg::std_msgs::MultiArrayLayout::typeText();
    }

    std::string getTypeText() const
    {
        return yarp::rosmsg::std_msgs::Int32MultiArray::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("std_msgs/Int32MultiArray", "std_msgs/Int32MultiArray");
        typ.addProperty("md5sum", yarp::os::Value("1d99f79f8b325b44fee908053e9c945b"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace std_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_std_msgs_Int32MultiArray_h

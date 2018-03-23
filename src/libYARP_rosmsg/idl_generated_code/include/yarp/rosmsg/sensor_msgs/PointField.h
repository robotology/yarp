/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.

// Generated from the following "sensor_msgs/PointField" msg definition:
//   # This message holds the description of one point entry in the
//   # PointCloud2 message format.
//   uint8 INT8    = 1
//   uint8 UINT8   = 2
//   uint8 INT16   = 3
//   uint8 UINT16  = 4
//   uint8 INT32   = 5
//   uint8 UINT32  = 6
//   uint8 FLOAT32 = 7
//   uint8 FLOAT64 = 8
//   
//   string name      # Name of field
//   uint32 offset    # Offset from start of point struct
//   uint8  datatype  # Datatype enumeration, see above
//   uint32 count     # How many elements in the field
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_sensor_msgs_PointField_h
#define YARP_ROSMSG_sensor_msgs_PointField_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>

namespace yarp {
namespace rosmsg {
namespace sensor_msgs {

class PointField : public yarp::os::idl::WirePortable
{
public:
    static const unsigned char INT8 = 1;
    static const unsigned char UINT8 = 2;
    static const unsigned char INT16 = 3;
    static const unsigned char UINT16 = 4;
    static const unsigned char INT32 = 5;
    static const unsigned char UINT32 = 6;
    static const unsigned char FLOAT32 = 7;
    static const unsigned char FLOAT64 = 8;
    std::string name;
    yarp::os::NetUint32 offset;
    unsigned char datatype;
    yarp::os::NetUint32 count;

    PointField() :
            name(""),
            offset(0),
            datatype(0),
            count(0)
    {
    }

    void clear()
    {
        // *** INT8 ***

        // *** UINT8 ***

        // *** INT16 ***

        // *** UINT16 ***

        // *** INT32 ***

        // *** UINT32 ***

        // *** FLOAT32 ***

        // *** FLOAT64 ***

        // *** name ***
        name = "";

        // *** offset ***
        offset = 0;

        // *** datatype ***
        datatype = 0;

        // *** count ***
        count = 0;
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** name ***
        int len = connection.expectInt();
        name.resize(len);
        if (!connection.expectBlock((char*)name.c_str(), len)) {
            return false;
        }

        // *** offset ***
        offset = connection.expectInt();

        // *** datatype ***
        if (!connection.expectBlock((char*)&datatype, 1)) {
            return false;
        }

        // *** count ***
        count = connection.expectInt();

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(12)) {
            return false;
        }

        // *** name ***
        if (!reader.readString(name)) {
            return false;
        }

        // *** offset ***
        offset = reader.expectInt();

        // *** datatype ***
        datatype = reader.expectInt();

        // *** count ***
        count = reader.expectInt();

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
        // *** name ***
        connection.appendInt(name.length());
        connection.appendExternalBlock((char*)name.c_str(), name.length());

        // *** offset ***
        connection.appendInt(offset);

        // *** datatype ***
        connection.appendBlock((char*)&datatype, 1);

        // *** count ***
        connection.appendInt(count);

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(12);

        // *** name ***
        connection.appendInt(BOTTLE_TAG_STRING);
        connection.appendInt(name.length());
        connection.appendExternalBlock((char*)name.c_str(), name.length());

        // *** offset ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)offset);

        // *** datatype ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)datatype);

        // *** count ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)count);

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::PointField> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::PointField> bottleStyle;

    // Give source text for class, ROS will need this
    static yarp::os::ConstString typeText()
    {
        return yarp::os::ConstString("\
# This message holds the description of one point entry in the\n\
# PointCloud2 message format.\n\
uint8 INT8    = 1\n\
uint8 UINT8   = 2\n\
uint8 INT16   = 3\n\
uint8 UINT16  = 4\n\
uint8 INT32   = 5\n\
uint8 UINT32  = 6\n\
uint8 FLOAT32 = 7\n\
uint8 FLOAT64 = 8\n\
\n\
string name      # Name of field\n\
uint32 offset    # Offset from start of point struct\n\
uint8  datatype  # Datatype enumeration, see above\n\
uint32 count     # How many elements in the field\n\
");
    }

    yarp::os::ConstString getTypeText() const
    {
        return yarp::rosmsg::sensor_msgs::PointField::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("sensor_msgs/PointField", "sensor_msgs/PointField");
        typ.addProperty("md5sum", yarp::os::Value("268eacb2962780ceac86cbd17e328150"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace sensor_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_sensor_msgs_PointField_h

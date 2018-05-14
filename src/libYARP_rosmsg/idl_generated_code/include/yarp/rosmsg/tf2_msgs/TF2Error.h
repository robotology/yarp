/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.

// Generated from the following "tf2_msgs/TF2Error" msg definition:
//   uint8 NO_ERROR = 0
//   uint8 LOOKUP_ERROR = 1
//   uint8 CONNECTIVITY_ERROR = 2
//   uint8 EXTRAPOLATION_ERROR = 3
//   uint8 INVALID_ARGUMENT_ERROR = 4
//   uint8 TIMEOUT_ERROR = 5
//   uint8 TRANSFORM_ERROR = 6
//   
//   uint8 error
//   string error_string
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_tf2_msgs_TF2Error_h
#define YARP_ROSMSG_tf2_msgs_TF2Error_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>

namespace yarp {
namespace rosmsg {
namespace tf2_msgs {

class TF2Error : public yarp::os::idl::WirePortable
{
public:
    static const unsigned char NO_ERROR = 0;
    static const unsigned char LOOKUP_ERROR = 1;
    static const unsigned char CONNECTIVITY_ERROR = 2;
    static const unsigned char EXTRAPOLATION_ERROR = 3;
    static const unsigned char INVALID_ARGUMENT_ERROR = 4;
    static const unsigned char TIMEOUT_ERROR = 5;
    static const unsigned char TRANSFORM_ERROR = 6;
    unsigned char error;
    std::string error_string;

    TF2Error() :
            error(0),
            error_string("")
    {
    }

    void clear()
    {
        // *** NO_ERROR ***

        // *** LOOKUP_ERROR ***

        // *** CONNECTIVITY_ERROR ***

        // *** EXTRAPOLATION_ERROR ***

        // *** INVALID_ARGUMENT_ERROR ***

        // *** TIMEOUT_ERROR ***

        // *** TRANSFORM_ERROR ***

        // *** error ***
        error = 0;

        // *** error_string ***
        error_string = "";
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** error ***
        if (!connection.expectBlock((char*)&error, 1)) {
            return false;
        }

        // *** error_string ***
        int len = connection.expectInt();
        error_string.resize(len);
        if (!connection.expectBlock((char*)error_string.c_str(), len)) {
            return false;
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(9)) {
            return false;
        }

        // *** error ***
        error = reader.expectInt();

        // *** error_string ***
        if (!reader.readString(error_string)) {
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
        // *** error ***
        connection.appendBlock((char*)&error, 1);

        // *** error_string ***
        connection.appendInt(error_string.length());
        connection.appendExternalBlock((char*)error_string.c_str(), error_string.length());

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(9);

        // *** error ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)error);

        // *** error_string ***
        connection.appendInt(BOTTLE_TAG_STRING);
        connection.appendInt(error_string.length());
        connection.appendExternalBlock((char*)error_string.c_str(), error_string.length());

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::tf2_msgs::TF2Error> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::tf2_msgs::TF2Error> bottleStyle;

    // Give source text for class, ROS will need this
    static std::string typeText()
    {
        return std::string("\
uint8 NO_ERROR = 0\n\
uint8 LOOKUP_ERROR = 1\n\
uint8 CONNECTIVITY_ERROR = 2\n\
uint8 EXTRAPOLATION_ERROR = 3\n\
uint8 INVALID_ARGUMENT_ERROR = 4\n\
uint8 TIMEOUT_ERROR = 5\n\
uint8 TRANSFORM_ERROR = 6\n\
\n\
uint8 error\n\
string error_string\n\
");
    }

    std::string getTypeText() const
    {
        return yarp::rosmsg::tf2_msgs::TF2Error::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("tf2_msgs/TF2Error", "tf2_msgs/TF2Error");
        typ.addProperty("md5sum", yarp::os::Value("bc6848fd6fd750c92e38575618a4917d"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace tf2_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_tf2_msgs_TF2Error_h

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "std_msgs/MultiArrayDimension" msg definition:
//   string label   # label of given dimension
//   uint32 size    # size of given dimension (in type units)
//   uint32 stride  # stride of given dimension// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_std_msgs_MultiArrayDimension_h
#define YARP_ROSMSG_std_msgs_MultiArrayDimension_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>

namespace yarp {
namespace rosmsg {
namespace std_msgs {

class MultiArrayDimension : public yarp::os::idl::WirePortable
{
public:
    std::string label;
    std::uint32_t size;
    std::uint32_t stride;

    MultiArrayDimension() :
            label(""),
            size(0),
            stride(0)
    {
    }

    void clear()
    {
        // *** label ***
        label = "";

        // *** size ***
        size = 0;

        // *** stride ***
        stride = 0;
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** label ***
        int len = connection.expectInt32();
        label.resize(len);
        if (!connection.expectBlock((char*)label.c_str(), len)) {
            return false;
        }

        // *** size ***
        size = connection.expectInt32();

        // *** stride ***
        stride = connection.expectInt32();

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(3)) {
            return false;
        }

        // *** label ***
        if (!reader.readString(label)) {
            return false;
        }

        // *** size ***
        size = reader.expectInt32();

        // *** stride ***
        stride = reader.expectInt32();

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
        // *** label ***
        connection.appendInt32(label.length());
        connection.appendExternalBlock((char*)label.c_str(), label.length());

        // *** size ***
        connection.appendInt32(size);

        // *** stride ***
        connection.appendInt32(stride);

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(3);

        // *** label ***
        connection.appendInt32(BOTTLE_TAG_STRING);
        connection.appendInt32(label.length());
        connection.appendExternalBlock((char*)label.c_str(), label.length());

        // *** size ***
        connection.appendInt32(BOTTLE_TAG_INT32);
        connection.appendInt32(size);

        // *** stride ***
        connection.appendInt32(BOTTLE_TAG_INT32);
        connection.appendInt32(stride);

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::std_msgs::MultiArrayDimension> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::std_msgs::MultiArrayDimension> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "std_msgs/MultiArrayDimension";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "4cd0c83a8683deae40ecdac60e53bfa8";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
string label   # label of given dimension\n\
uint32 size    # size of given dimension (in type units)\n\
uint32 stride  # stride of given dimension\n\
";

    yarp::os::Type getType() const override
    {
        yarp::os::Type typ = yarp::os::Type::byName(typeName, typeName);
        typ.addProperty("md5sum", yarp::os::Value(typeChecksum));
        typ.addProperty("message_definition", yarp::os::Value(typeText));
        return typ;
    }
};

} // namespace std_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_std_msgs_MultiArrayDimension_h

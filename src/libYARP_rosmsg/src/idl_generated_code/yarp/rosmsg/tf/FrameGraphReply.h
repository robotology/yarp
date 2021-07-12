/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "tf/FrameGraphReply" msg definition:
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_tf_FrameGraphReply_h
#define YARP_ROSMSG_tf_FrameGraphReply_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>

namespace yarp {
namespace rosmsg {
namespace tf {

class FrameGraphReply : public yarp::os::idl::WirePortable
{
public:
    std::string dot_graph;

    FrameGraphReply() :
            dot_graph("")
    {
    }

    void clear()
    {
        // *** dot_graph ***
        dot_graph = "";
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** dot_graph ***
        int len = connection.expectInt32();
        dot_graph.resize(len);
        if (!connection.expectBlock((char*)dot_graph.c_str(), len)) {
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

        // *** dot_graph ***
        if (!reader.readString(dot_graph)) {
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

    bool writeBare(yarp::os::ConnectionWriter& connection) const override
    {
        // *** dot_graph ***
        connection.appendInt32(dot_graph.length());
        connection.appendExternalBlock((char*)dot_graph.c_str(), dot_graph.length());

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(1);

        // *** dot_graph ***
        connection.appendInt32(BOTTLE_TAG_STRING);
        connection.appendInt32(dot_graph.length());
        connection.appendExternalBlock((char*)dot_graph.c_str(), dot_graph.length());

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::tf::FrameGraphReply> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::tf::FrameGraphReply> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "tf/FrameGraphReply";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "c4af9ac907e58e906eb0b6e3c58478c0";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
\n\
";

    yarp::os::Type getType() const override
    {
        yarp::os::Type typ = yarp::os::Type::byName(typeName, typeName);
        typ.addProperty("md5sum", yarp::os::Value(typeChecksum));
        typ.addProperty("message_definition", yarp::os::Value(typeText));
        return typ;
    }
};

} // namespace tf
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_tf_FrameGraphReply_h

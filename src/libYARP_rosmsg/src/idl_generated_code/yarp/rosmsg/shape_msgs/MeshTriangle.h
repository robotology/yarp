/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "shape_msgs/MeshTriangle" msg definition:
//   # Definition of a triangle's vertices
//   uint32[3] vertex_indices
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_shape_msgs_MeshTriangle_h
#define YARP_ROSMSG_shape_msgs_MeshTriangle_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>

namespace yarp {
namespace rosmsg {
namespace shape_msgs {

class MeshTriangle : public yarp::os::idl::WirePortable
{
public:
    std::vector<std::uint32_t> vertex_indices;

    MeshTriangle() :
            vertex_indices()
    {
        vertex_indices.resize(3, 0);
    }

    void clear()
    {
        // *** vertex_indices ***
        vertex_indices.clear();
        vertex_indices.resize(3, 0);
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** vertex_indices ***
        int len = 3;
        vertex_indices.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&vertex_indices[0], sizeof(std::uint32_t)*len)) {
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

        // *** vertex_indices ***
        if (connection.expectInt32() != (BOTTLE_TAG_LIST|BOTTLE_TAG_INT32)) {
            return false;
        }
        int len = connection.expectInt32();
        vertex_indices.resize(len);
        for (int i=0; i<len; i++) {
            vertex_indices[i] = (std::uint32_t)connection.expectInt32();
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
        // *** vertex_indices ***
        if (vertex_indices.size()>0) {
            connection.appendExternalBlock((char*)&vertex_indices[0], sizeof(std::uint32_t)*vertex_indices.size());
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(1);

        // *** vertex_indices ***
        connection.appendInt32(BOTTLE_TAG_LIST|BOTTLE_TAG_INT32);
        connection.appendInt32(vertex_indices.size());
        for (size_t i=0; i<vertex_indices.size(); i++) {
            connection.appendInt32(vertex_indices[i]);
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::shape_msgs::MeshTriangle> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::shape_msgs::MeshTriangle> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "shape_msgs/MeshTriangle";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "23688b2e6d2de3d32fe8af104a903253";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
# Definition of a triangle's vertices\n\
uint32[3] vertex_indices\n\
";

    yarp::os::Type getType() const override
    {
        yarp::os::Type typ = yarp::os::Type::byName(typeName, typeName);
        typ.addProperty("md5sum", yarp::os::Value(typeChecksum));
        typ.addProperty("message_definition", yarp::os::Value(typeText));
        return typ;
    }
};

} // namespace shape_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_shape_msgs_MeshTriangle_h

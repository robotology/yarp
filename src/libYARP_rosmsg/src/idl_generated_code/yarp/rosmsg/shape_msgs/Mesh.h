/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "shape_msgs/Mesh" msg definition:
//   # Definition of a mesh
//   
//   # list of triangles; the index values refer to positions in vertices[]
//   MeshTriangle[] triangles
//   
//   # the actual vertices that make up the mesh
//   geometry_msgs/Point[] vertices
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_shape_msgs_Mesh_h
#define YARP_ROSMSG_shape_msgs_Mesh_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/shape_msgs/MeshTriangle.h>
#include <yarp/rosmsg/geometry_msgs/Point.h>

namespace yarp {
namespace rosmsg {
namespace shape_msgs {

class Mesh : public yarp::os::idl::WirePortable
{
public:
    std::vector<yarp::rosmsg::shape_msgs::MeshTriangle> triangles;
    std::vector<yarp::rosmsg::geometry_msgs::Point> vertices;

    Mesh() :
            triangles(),
            vertices()
    {
    }

    void clear()
    {
        // *** triangles ***
        triangles.clear();

        // *** vertices ***
        vertices.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** triangles ***
        int len = connection.expectInt32();
        triangles.resize(len);
        for (int i=0; i<len; i++) {
            if (!triangles[i].read(connection)) {
                return false;
            }
        }

        // *** vertices ***
        len = connection.expectInt32();
        vertices.resize(len);
        for (int i=0; i<len; i++) {
            if (!vertices[i].read(connection)) {
                return false;
            }
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

        // *** triangles ***
        if (connection.expectInt32() != BOTTLE_TAG_LIST) {
            return false;
        }
        int len = connection.expectInt32();
        triangles.resize(len);
        for (int i=0; i<len; i++) {
            if (!triangles[i].read(connection)) {
                return false;
            }
        }

        // *** vertices ***
        if (connection.expectInt32() != BOTTLE_TAG_LIST) {
            return false;
        }
        len = connection.expectInt32();
        vertices.resize(len);
        for (int i=0; i<len; i++) {
            if (!vertices[i].read(connection)) {
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

    bool writeBare(yarp::os::ConnectionWriter& connection) const override
    {
        // *** triangles ***
        connection.appendInt32(triangles.size());
        for (size_t i=0; i<triangles.size(); i++) {
            if (!triangles[i].write(connection)) {
                return false;
            }
        }

        // *** vertices ***
        connection.appendInt32(vertices.size());
        for (size_t i=0; i<vertices.size(); i++) {
            if (!vertices[i].write(connection)) {
                return false;
            }
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(2);

        // *** triangles ***
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(triangles.size());
        for (size_t i=0; i<triangles.size(); i++) {
            if (!triangles[i].write(connection)) {
                return false;
            }
        }

        // *** vertices ***
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(vertices.size());
        for (size_t i=0; i<vertices.size(); i++) {
            if (!vertices[i].write(connection)) {
                return false;
            }
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::shape_msgs::Mesh> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::shape_msgs::Mesh> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "shape_msgs/Mesh";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "1ffdae9486cd3316a121c578b47a85cc";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
# Definition of a mesh\n\
\n\
# list of triangles; the index values refer to positions in vertices[]\n\
MeshTriangle[] triangles\n\
\n\
# the actual vertices that make up the mesh\n\
geometry_msgs/Point[] vertices\n\
\n\
================================================================================\n\
MSG: shape_msgs/MeshTriangle\n\
# Definition of a triangle's vertices\n\
uint32[3] vertex_indices\n\
\n\
================================================================================\n\
MSG: geometry_msgs/Point\n\
# This contains the position of a point in free space\n\
float64 x\n\
float64 y\n\
float64 z\n\
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

#endif // YARP_ROSMSG_shape_msgs_Mesh_h

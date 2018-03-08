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
        int len = connection.expectInt();
        triangles.resize(len);
        for (int i=0; i<len; i++) {
            if (!triangles[i].read(connection)) {
                return false;
            }
        }

        // *** vertices ***
        len = connection.expectInt();
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
        if (connection.expectInt() != BOTTLE_TAG_LIST) {
            return false;
        }
        int len = connection.expectInt();
        triangles.resize(len);
        for (int i=0; i<len; i++) {
            if (!triangles[i].read(connection)) {
                return false;
            }
        }

        // *** vertices ***
        if (connection.expectInt() != BOTTLE_TAG_LIST) {
            return false;
        }
        len = connection.expectInt();
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

    bool writeBare(yarp::os::ConnectionWriter& connection) override
    {
        // *** triangles ***
        connection.appendInt(triangles.size());
        for (size_t i=0; i<triangles.size(); i++) {
            if (!triangles[i].write(connection)) {
                return false;
            }
        }

        // *** vertices ***
        connection.appendInt(vertices.size());
        for (size_t i=0; i<vertices.size(); i++) {
            if (!vertices[i].write(connection)) {
                return false;
            }
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(2);

        // *** triangles ***
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(triangles.size());
        for (size_t i=0; i<triangles.size(); i++) {
            if (!triangles[i].write(connection)) {
                return false;
            }
        }

        // *** vertices ***
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(vertices.size());
        for (size_t i=0; i<vertices.size(); i++) {
            if (!vertices[i].write(connection)) {
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::shape_msgs::Mesh> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::shape_msgs::Mesh> bottleStyle;

    // Give source text for class, ROS will need this
    yarp::os::ConstString getTypeText()
    {
        return "# Definition of a mesh\n\
\n\
# list of triangles; the index values refer to positions in vertices[]\n\
MeshTriangle[] triangles\n\
\n\
# the actual vertices that make up the mesh\n\
geometry_msgs/Point[] vertices\n================================================================================\n\
MSG: shape_msgs/MeshTriangle\n\
\n================================================================================\n\
MSG: geometry_msgs/Point\n\
";
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("shape_msgs/Mesh", "shape_msgs/Mesh");
        typ.addProperty("md5sum", yarp::os::Value("d41d8cd98f00b204e9800998ecf8427e"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace shape_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_shape_msgs_Mesh_h

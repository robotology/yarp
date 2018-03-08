// This is an automatically generated file.

// Generated from the following "geometry_msgs/Inertia" msg definition:
//   # Mass [kg]
//   float64 m
//   
//   # Center of mass [m]
//   geometry_msgs/Vector3 com
//   
//   # Inertia Tensor [kg-m^2]
//   #     | ixx ixy ixz |
//   # I = | ixy iyy iyz |
//   #     | ixz iyz izz |
//   float64 ixx
//   float64 ixy
//   float64 ixz
//   float64 iyy
//   float64 iyz
//   float64 izz
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_geometry_msgs_Inertia_h
#define YARP_ROSMSG_geometry_msgs_Inertia_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/geometry_msgs/Vector3.h>

namespace yarp {
namespace rosmsg {
namespace geometry_msgs {

class Inertia : public yarp::os::idl::WirePortable
{
public:
    yarp::os::NetFloat64 m;
    yarp::rosmsg::geometry_msgs::Vector3 com;
    yarp::os::NetFloat64 ixx;
    yarp::os::NetFloat64 ixy;
    yarp::os::NetFloat64 ixz;
    yarp::os::NetFloat64 iyy;
    yarp::os::NetFloat64 iyz;
    yarp::os::NetFloat64 izz;

    Inertia() :
            m(0.0),
            com(),
            ixx(0.0),
            ixy(0.0),
            ixz(0.0),
            iyy(0.0),
            iyz(0.0),
            izz(0.0)
    {
    }

    void clear()
    {
        // *** m ***
        m = 0.0;

        // *** com ***
        com.clear();

        // *** ixx ***
        ixx = 0.0;

        // *** ixy ***
        ixy = 0.0;

        // *** ixz ***
        ixz = 0.0;

        // *** iyy ***
        iyy = 0.0;

        // *** iyz ***
        iyz = 0.0;

        // *** izz ***
        izz = 0.0;
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** m ***
        m = connection.expectDouble();

        // *** com ***
        if (!com.read(connection)) {
            return false;
        }

        // *** ixx ***
        ixx = connection.expectDouble();

        // *** ixy ***
        ixy = connection.expectDouble();

        // *** ixz ***
        ixz = connection.expectDouble();

        // *** iyy ***
        iyy = connection.expectDouble();

        // *** iyz ***
        iyz = connection.expectDouble();

        // *** izz ***
        izz = connection.expectDouble();

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(8)) {
            return false;
        }

        // *** m ***
        m = reader.expectDouble();

        // *** com ***
        if (!com.read(connection)) {
            return false;
        }

        // *** ixx ***
        ixx = reader.expectDouble();

        // *** ixy ***
        ixy = reader.expectDouble();

        // *** ixz ***
        ixz = reader.expectDouble();

        // *** iyy ***
        iyy = reader.expectDouble();

        // *** iyz ***
        iyz = reader.expectDouble();

        // *** izz ***
        izz = reader.expectDouble();

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
        // *** m ***
        connection.appendDouble(m);

        // *** com ***
        if (!com.write(connection)) {
            return false;
        }

        // *** ixx ***
        connection.appendDouble(ixx);

        // *** ixy ***
        connection.appendDouble(ixy);

        // *** ixz ***
        connection.appendDouble(ixz);

        // *** iyy ***
        connection.appendDouble(iyy);

        // *** iyz ***
        connection.appendDouble(iyz);

        // *** izz ***
        connection.appendDouble(izz);

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(8);

        // *** m ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)m);

        // *** com ***
        if (!com.write(connection)) {
            return false;
        }

        // *** ixx ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)ixx);

        // *** ixy ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)ixy);

        // *** ixz ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)ixz);

        // *** iyy ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)iyy);

        // *** iyz ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)iyz);

        // *** izz ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)izz);

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::geometry_msgs::Inertia> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::geometry_msgs::Inertia> bottleStyle;

    // Give source text for class, ROS will need this
    yarp::os::ConstString getTypeText()
    {
        return "# Mass [kg]\n\
float64 m\n\
\n\
# Center of mass [m]\n\
geometry_msgs/Vector3 com\n\
\n\
# Inertia Tensor [kg-m^2]\n\
#     | ixx ixy ixz |\n\
# I = | ixy iyy iyz |\n\
#     | ixz iyz izz |\n\
float64 ixx\n\
float64 ixy\n\
float64 ixz\n\
float64 iyy\n\
float64 iyz\n\
float64 izz\n================================================================================\n\
MSG: geometry_msgs/Vector3\n\
";
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("geometry_msgs/Inertia", "geometry_msgs/Inertia");
        typ.addProperty("md5sum", yarp::os::Value("d41d8cd98f00b204e9800998ecf8427e"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace geometry_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_geometry_msgs_Inertia_h

// This is an automatically generated file.

// Generated from the following "geometry_msgs/Pose" msg definition:
//   # A representation of pose in free space, composed of position and orientation. 
//   Point position
//   Quaternion orientation
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_geometry_msgs_Pose_h
#define YARP_ROSMSG_geometry_msgs_Pose_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/geometry_msgs/Point.h>
#include <yarp/rosmsg/geometry_msgs/Quaternion.h>

namespace yarp {
namespace rosmsg {
namespace geometry_msgs {

class Pose : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::geometry_msgs::Point position;
    yarp::rosmsg::geometry_msgs::Quaternion orientation;

    Pose() :
            position(),
            orientation()
    {
    }

    void clear()
    {
        // *** position ***
        position.clear();

        // *** orientation ***
        orientation.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** position ***
        if (!position.read(connection)) {
            return false;
        }

        // *** orientation ***
        if (!orientation.read(connection)) {
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

        // *** position ***
        if (!position.read(connection)) {
            return false;
        }

        // *** orientation ***
        if (!orientation.read(connection)) {
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
        // *** position ***
        if (!position.write(connection)) {
            return false;
        }

        // *** orientation ***
        if (!orientation.write(connection)) {
            return false;
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(2);

        // *** position ***
        if (!position.write(connection)) {
            return false;
        }

        // *** orientation ***
        if (!orientation.write(connection)) {
            return false;
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::geometry_msgs::Pose> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::geometry_msgs::Pose> bottleStyle;

    // Give source text for class, ROS will need this
    yarp::os::ConstString getTypeText()
    {
        return "# A representation of pose in free space, composed of position and orientation. \n\
Point position\n\
Quaternion orientation\n================================================================================\n\
MSG: geometry_msgs/Point\n\
\n================================================================================\n\
MSG: geometry_msgs/Quaternion\n\
";
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("geometry_msgs/Pose", "geometry_msgs/Pose");
        typ.addProperty("md5sum", yarp::os::Value("d41d8cd98f00b204e9800998ecf8427e"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace geometry_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_geometry_msgs_Pose_h

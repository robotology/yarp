// This is an automatically generated file.

// Generated from the following "nav_msgs/Odometry" msg definition:
//   # This represents an estimate of a position and velocity in free space.  
//   # The pose in this message should be specified in the coordinate frame given by header.frame_id.
//   # The twist in this message should be specified in the coordinate frame given by the child_frame_id
//   Header header
//   string child_frame_id
//   geometry_msgs/PoseWithCovariance pose
//   geometry_msgs/TwistWithCovariance twist
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_nav_msgs_Odometry_h
#define YARP_ROSMSG_nav_msgs_Odometry_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>
#include <yarp/rosmsg/geometry_msgs/PoseWithCovariance.h>
#include <yarp/rosmsg/geometry_msgs/TwistWithCovariance.h>

namespace yarp {
namespace rosmsg {
namespace nav_msgs {

class Odometry : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::std_msgs::Header header;
    std::string child_frame_id;
    yarp::rosmsg::geometry_msgs::PoseWithCovariance pose;
    yarp::rosmsg::geometry_msgs::TwistWithCovariance twist;

    Odometry() :
            header(),
            child_frame_id(""),
            pose(),
            twist()
    {
    }

    void clear()
    {
        // *** header ***
        header.clear();

        // *** child_frame_id ***
        child_frame_id = "";

        // *** pose ***
        pose.clear();

        // *** twist ***
        twist.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** child_frame_id ***
        int len = connection.expectInt();
        child_frame_id.resize(len);
        if (!connection.expectBlock((char*)child_frame_id.c_str(), len)) {
            return false;
        }

        // *** pose ***
        if (!pose.read(connection)) {
            return false;
        }

        // *** twist ***
        if (!twist.read(connection)) {
            return false;
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(4)) {
            return false;
        }

        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** child_frame_id ***
        if (!reader.readString(child_frame_id)) {
            return false;
        }

        // *** pose ***
        if (!pose.read(connection)) {
            return false;
        }

        // *** twist ***
        if (!twist.read(connection)) {
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
        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** child_frame_id ***
        connection.appendInt(child_frame_id.length());
        connection.appendExternalBlock((char*)child_frame_id.c_str(), child_frame_id.length());

        // *** pose ***
        if (!pose.write(connection)) {
            return false;
        }

        // *** twist ***
        if (!twist.write(connection)) {
            return false;
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(4);

        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** child_frame_id ***
        connection.appendInt(BOTTLE_TAG_STRING);
        connection.appendInt(child_frame_id.length());
        connection.appendExternalBlock((char*)child_frame_id.c_str(), child_frame_id.length());

        // *** pose ***
        if (!pose.write(connection)) {
            return false;
        }

        // *** twist ***
        if (!twist.write(connection)) {
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::nav_msgs::Odometry> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::nav_msgs::Odometry> bottleStyle;

    // Give source text for class, ROS will need this
    yarp::os::ConstString getTypeText()
    {
        return "# This represents an estimate of a position and velocity in free space.  \n\
# The pose in this message should be specified in the coordinate frame given by header.frame_id.\n\
# The twist in this message should be specified in the coordinate frame given by the child_frame_id\n\
Header header\n\
string child_frame_id\n\
geometry_msgs/PoseWithCovariance pose\n\
geometry_msgs/TwistWithCovariance twist\n================================================================================\n\
MSG: std_msgs/Header\n\
\n================================================================================\n\
MSG: geometry_msgs/PoseWithCovariance\n\
\n================================================================================\n\
MSG: geometry_msgs/TwistWithCovariance\n\
";
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("nav_msgs/Odometry", "nav_msgs/Odometry");
        typ.addProperty("md5sum", yarp::os::Value("d41d8cd98f00b204e9800998ecf8427e"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace nav_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_nav_msgs_Odometry_h

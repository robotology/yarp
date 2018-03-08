// This is an automatically generated file.

// Generated from the following "trajectory_msgs/MultiDOFJointTrajectory" msg definition:
//   # The header is used to specify the coordinate frame and the reference time for the trajectory durations
//   Header header
//   
//   # A representation of a multi-dof joint trajectory (each point is a transformation)
//   # Each point along the trajectory will include an array of positions/velocities/accelerations
//   # that has the same length as the array of joint names, and has the same order of joints as 
//   # the joint names array.
//   
//   string[] joint_names
//   MultiDOFJointTrajectoryPoint[] points
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_trajectory_msgs_MultiDOFJointTrajectory_h
#define YARP_ROSMSG_trajectory_msgs_MultiDOFJointTrajectory_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>
#include <yarp/rosmsg/trajectory_msgs/MultiDOFJointTrajectoryPoint.h>

namespace yarp {
namespace rosmsg {
namespace trajectory_msgs {

class MultiDOFJointTrajectory : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::std_msgs::Header header;
    std::vector<std::string> joint_names;
    std::vector<yarp::rosmsg::trajectory_msgs::MultiDOFJointTrajectoryPoint> points;

    MultiDOFJointTrajectory() :
            header(),
            joint_names(),
            points()
    {
    }

    void clear()
    {
        // *** header ***
        header.clear();

        // *** joint_names ***
        joint_names.clear();

        // *** points ***
        points.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** joint_names ***
        int len = connection.expectInt();
        joint_names.resize(len);
        for (int i=0; i<len; i++) {
            int len2 = connection.expectInt();
            joint_names[i].resize(len2);
            if (!connection.expectBlock((char*)joint_names[i].c_str(), len2)) {
                return false;
            }
        }

        // *** points ***
        len = connection.expectInt();
        points.resize(len);
        for (int i=0; i<len; i++) {
            if (!points[i].read(connection)) {
                return false;
            }
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(3)) {
            return false;
        }

        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** joint_names ***
        if (connection.expectInt() != (BOTTLE_TAG_LIST|BOTTLE_TAG_STRING)) {
            return false;
        }
        int len = connection.expectInt();
        joint_names.resize(len);
        for (int i=0; i<len; i++) {
            int len2 = connection.expectInt();
            joint_names[i].resize(len2);
            if (!connection.expectBlock((char*)joint_names[i].c_str(), len2)) {
                return false;
            }
        }

        // *** points ***
        if (connection.expectInt() != BOTTLE_TAG_LIST) {
            return false;
        }
        len = connection.expectInt();
        points.resize(len);
        for (int i=0; i<len; i++) {
            if (!points[i].read(connection)) {
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
        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** joint_names ***
        connection.appendInt(joint_names.size());
        for (size_t i=0; i<joint_names.size(); i++) {
            connection.appendInt(joint_names[i].length());
            connection.appendExternalBlock((char*)joint_names[i].c_str(), joint_names[i].length());
        }

        // *** points ***
        connection.appendInt(points.size());
        for (size_t i=0; i<points.size(); i++) {
            if (!points[i].write(connection)) {
                return false;
            }
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(3);

        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** joint_names ***
        connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_STRING);
        connection.appendInt(joint_names.size());
        for (size_t i=0; i<joint_names.size(); i++) {
            connection.appendInt(joint_names[i].length());
            connection.appendExternalBlock((char*)joint_names[i].c_str(), joint_names[i].length());
        }

        // *** points ***
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(points.size());
        for (size_t i=0; i<points.size(); i++) {
            if (!points[i].write(connection)) {
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::trajectory_msgs::MultiDOFJointTrajectory> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::trajectory_msgs::MultiDOFJointTrajectory> bottleStyle;

    // Give source text for class, ROS will need this
    yarp::os::ConstString getTypeText()
    {
        return "# The header is used to specify the coordinate frame and the reference time for the trajectory durations\n\
Header header\n\
\n\
# A representation of a multi-dof joint trajectory (each point is a transformation)\n\
# Each point along the trajectory will include an array of positions/velocities/accelerations\n\
# that has the same length as the array of joint names, and has the same order of joints as \n\
# the joint names array.\n\
\n\
string[] joint_names\n\
MultiDOFJointTrajectoryPoint[] points\n================================================================================\n\
MSG: std_msgs/Header\n\
\n================================================================================\n\
MSG: trajectory_msgs/MultiDOFJointTrajectoryPoint\n\
";
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("trajectory_msgs/MultiDOFJointTrajectory", "trajectory_msgs/MultiDOFJointTrajectory");
        typ.addProperty("md5sum", yarp::os::Value("d41d8cd98f00b204e9800998ecf8427e"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace trajectory_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_trajectory_msgs_MultiDOFJointTrajectory_h

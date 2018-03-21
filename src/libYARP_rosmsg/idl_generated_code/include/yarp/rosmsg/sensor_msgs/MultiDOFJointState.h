// This is an automatically generated file.

// Generated from the following "sensor_msgs/MultiDOFJointState" msg definition:
//   # Representation of state for joints with multiple degrees of freedom, 
//   # following the structure of JointState.
//   #
//   # It is assumed that a joint in a system corresponds to a transform that gets applied 
//   # along the kinematic chain. For example, a planar joint (as in URDF) is 3DOF (x, y, yaw)
//   # and those 3DOF can be expressed as a transformation matrix, and that transformation
//   # matrix can be converted back to (x, y, yaw)
//   #
//   # Each joint is uniquely identified by its name
//   # The header specifies the time at which the joint states were recorded. All the joint states
//   # in one message have to be recorded at the same time.
//   #
//   # This message consists of a multiple arrays, one for each part of the joint state. 
//   # The goal is to make each of the fields optional. When e.g. your joints have no
//   # wrench associated with them, you can leave the wrench array empty. 
//   #
//   # All arrays in this message should have the same size, or be empty.
//   # This is the only way to uniquely associate the joint name with the correct
//   # states.
//   
//   Header header
//   
//   string[] joint_names
//   geometry_msgs/Transform[] transforms
//   geometry_msgs/Twist[] twist
//   geometry_msgs/Wrench[] wrench
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_sensor_msgs_MultiDOFJointState_h
#define YARP_ROSMSG_sensor_msgs_MultiDOFJointState_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>
#include <yarp/rosmsg/geometry_msgs/Transform.h>
#include <yarp/rosmsg/geometry_msgs/Twist.h>
#include <yarp/rosmsg/geometry_msgs/Wrench.h>

namespace yarp {
namespace rosmsg {
namespace sensor_msgs {

class MultiDOFJointState : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::std_msgs::Header header;
    std::vector<std::string> joint_names;
    std::vector<yarp::rosmsg::geometry_msgs::Transform> transforms;
    std::vector<yarp::rosmsg::geometry_msgs::Twist> twist;
    std::vector<yarp::rosmsg::geometry_msgs::Wrench> wrench;

    MultiDOFJointState() :
            header(),
            joint_names(),
            transforms(),
            twist(),
            wrench()
    {
    }

    void clear()
    {
        // *** header ***
        header.clear();

        // *** joint_names ***
        joint_names.clear();

        // *** transforms ***
        transforms.clear();

        // *** twist ***
        twist.clear();

        // *** wrench ***
        wrench.clear();
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

        // *** transforms ***
        len = connection.expectInt();
        transforms.resize(len);
        for (int i=0; i<len; i++) {
            if (!transforms[i].read(connection)) {
                return false;
            }
        }

        // *** twist ***
        len = connection.expectInt();
        twist.resize(len);
        for (int i=0; i<len; i++) {
            if (!twist[i].read(connection)) {
                return false;
            }
        }

        // *** wrench ***
        len = connection.expectInt();
        wrench.resize(len);
        for (int i=0; i<len; i++) {
            if (!wrench[i].read(connection)) {
                return false;
            }
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(5)) {
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

        // *** transforms ***
        if (connection.expectInt() != BOTTLE_TAG_LIST) {
            return false;
        }
        len = connection.expectInt();
        transforms.resize(len);
        for (int i=0; i<len; i++) {
            if (!transforms[i].read(connection)) {
                return false;
            }
        }

        // *** twist ***
        if (connection.expectInt() != BOTTLE_TAG_LIST) {
            return false;
        }
        len = connection.expectInt();
        twist.resize(len);
        for (int i=0; i<len; i++) {
            if (!twist[i].read(connection)) {
                return false;
            }
        }

        // *** wrench ***
        if (connection.expectInt() != BOTTLE_TAG_LIST) {
            return false;
        }
        len = connection.expectInt();
        wrench.resize(len);
        for (int i=0; i<len; i++) {
            if (!wrench[i].read(connection)) {
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

        // *** transforms ***
        connection.appendInt(transforms.size());
        for (size_t i=0; i<transforms.size(); i++) {
            if (!transforms[i].write(connection)) {
                return false;
            }
        }

        // *** twist ***
        connection.appendInt(twist.size());
        for (size_t i=0; i<twist.size(); i++) {
            if (!twist[i].write(connection)) {
                return false;
            }
        }

        // *** wrench ***
        connection.appendInt(wrench.size());
        for (size_t i=0; i<wrench.size(); i++) {
            if (!wrench[i].write(connection)) {
                return false;
            }
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(5);

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

        // *** transforms ***
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(transforms.size());
        for (size_t i=0; i<transforms.size(); i++) {
            if (!transforms[i].write(connection)) {
                return false;
            }
        }

        // *** twist ***
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(twist.size());
        for (size_t i=0; i<twist.size(); i++) {
            if (!twist[i].write(connection)) {
                return false;
            }
        }

        // *** wrench ***
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(wrench.size());
        for (size_t i=0; i<wrench.size(); i++) {
            if (!wrench[i].write(connection)) {
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::MultiDOFJointState> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::MultiDOFJointState> bottleStyle;

    // Give source text for class, ROS will need this
    yarp::os::ConstString getTypeText()
    {
        return "# Representation of state for joints with multiple degrees of freedom, \n\
# following the structure of JointState.\n\
#\n\
# It is assumed that a joint in a system corresponds to a transform that gets applied \n\
# along the kinematic chain. For example, a planar joint (as in URDF) is 3DOF (x, y, yaw)\n\
# and those 3DOF can be expressed as a transformation matrix, and that transformation\n\
# matrix can be converted back to (x, y, yaw)\n\
#\n\
# Each joint is uniquely identified by its name\n\
# The header specifies the time at which the joint states were recorded. All the joint states\n\
# in one message have to be recorded at the same time.\n\
#\n\
# This message consists of a multiple arrays, one for each part of the joint state. \n\
# The goal is to make each of the fields optional. When e.g. your joints have no\n\
# wrench associated with them, you can leave the wrench array empty. \n\
#\n\
# All arrays in this message should have the same size, or be empty.\n\
# This is the only way to uniquely associate the joint name with the correct\n\
# states.\n\
\n\
Header header\n\
\n\
string[] joint_names\n\
geometry_msgs/Transform[] transforms\n\
geometry_msgs/Twist[] twist\n\
geometry_msgs/Wrench[] wrench\n================================================================================\n\
MSG: std_msgs/Header\n\
\n================================================================================\n\
MSG: geometry_msgs/Transform\n\
\n================================================================================\n\
MSG: geometry_msgs/Twist\n\
\n================================================================================\n\
MSG: geometry_msgs/Wrench\n\
";
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("sensor_msgs/MultiDOFJointState", "sensor_msgs/MultiDOFJointState");
        typ.addProperty("md5sum", yarp::os::Value("d41d8cd98f00b204e9800998ecf8427e"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace sensor_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_sensor_msgs_MultiDOFJointState_h

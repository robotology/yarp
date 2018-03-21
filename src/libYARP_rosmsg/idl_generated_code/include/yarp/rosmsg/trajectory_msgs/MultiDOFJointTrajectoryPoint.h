// This is an automatically generated file.

// Generated from the following "trajectory_msgs/MultiDOFJointTrajectoryPoint" msg definition:
//   # Each multi-dof joint can specify a transform (up to 6 DOF)
//   geometry_msgs/Transform[] transforms
//   
//   # There can be a velocity specified for the origin of the joint 
//   geometry_msgs/Twist[] velocities
//   
//   # There can be an acceleration specified for the origin of the joint 
//   geometry_msgs/Twist[] accelerations
//   
//   duration time_from_start
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_trajectory_msgs_MultiDOFJointTrajectoryPoint_h
#define YARP_ROSMSG_trajectory_msgs_MultiDOFJointTrajectoryPoint_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/geometry_msgs/Transform.h>
#include <yarp/rosmsg/geometry_msgs/Twist.h>
#include <yarp/rosmsg/TickDuration.h>

namespace yarp {
namespace rosmsg {
namespace trajectory_msgs {

class MultiDOFJointTrajectoryPoint : public yarp::os::idl::WirePortable
{
public:
    std::vector<yarp::rosmsg::geometry_msgs::Transform> transforms;
    std::vector<yarp::rosmsg::geometry_msgs::Twist> velocities;
    std::vector<yarp::rosmsg::geometry_msgs::Twist> accelerations;
    yarp::rosmsg::TickDuration time_from_start;

    MultiDOFJointTrajectoryPoint() :
            transforms(),
            velocities(),
            accelerations(),
            time_from_start()
    {
    }

    void clear()
    {
        // *** transforms ***
        transforms.clear();

        // *** velocities ***
        velocities.clear();

        // *** accelerations ***
        accelerations.clear();

        // *** time_from_start ***
        time_from_start.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** transforms ***
        int len = connection.expectInt();
        transforms.resize(len);
        for (int i=0; i<len; i++) {
            if (!transforms[i].read(connection)) {
                return false;
            }
        }

        // *** velocities ***
        len = connection.expectInt();
        velocities.resize(len);
        for (int i=0; i<len; i++) {
            if (!velocities[i].read(connection)) {
                return false;
            }
        }

        // *** accelerations ***
        len = connection.expectInt();
        accelerations.resize(len);
        for (int i=0; i<len; i++) {
            if (!accelerations[i].read(connection)) {
                return false;
            }
        }

        // *** time_from_start ***
        if (!time_from_start.read(connection)) {
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

        // *** transforms ***
        if (connection.expectInt() != BOTTLE_TAG_LIST) {
            return false;
        }
        int len = connection.expectInt();
        transforms.resize(len);
        for (int i=0; i<len; i++) {
            if (!transforms[i].read(connection)) {
                return false;
            }
        }

        // *** velocities ***
        if (connection.expectInt() != BOTTLE_TAG_LIST) {
            return false;
        }
        len = connection.expectInt();
        velocities.resize(len);
        for (int i=0; i<len; i++) {
            if (!velocities[i].read(connection)) {
                return false;
            }
        }

        // *** accelerations ***
        if (connection.expectInt() != BOTTLE_TAG_LIST) {
            return false;
        }
        len = connection.expectInt();
        accelerations.resize(len);
        for (int i=0; i<len; i++) {
            if (!accelerations[i].read(connection)) {
                return false;
            }
        }

        // *** time_from_start ***
        if (!time_from_start.read(connection)) {
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
        // *** transforms ***
        connection.appendInt(transforms.size());
        for (size_t i=0; i<transforms.size(); i++) {
            if (!transforms[i].write(connection)) {
                return false;
            }
        }

        // *** velocities ***
        connection.appendInt(velocities.size());
        for (size_t i=0; i<velocities.size(); i++) {
            if (!velocities[i].write(connection)) {
                return false;
            }
        }

        // *** accelerations ***
        connection.appendInt(accelerations.size());
        for (size_t i=0; i<accelerations.size(); i++) {
            if (!accelerations[i].write(connection)) {
                return false;
            }
        }

        // *** time_from_start ***
        if (!time_from_start.write(connection)) {
            return false;
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(4);

        // *** transforms ***
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(transforms.size());
        for (size_t i=0; i<transforms.size(); i++) {
            if (!transforms[i].write(connection)) {
                return false;
            }
        }

        // *** velocities ***
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(velocities.size());
        for (size_t i=0; i<velocities.size(); i++) {
            if (!velocities[i].write(connection)) {
                return false;
            }
        }

        // *** accelerations ***
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(accelerations.size());
        for (size_t i=0; i<accelerations.size(); i++) {
            if (!accelerations[i].write(connection)) {
                return false;
            }
        }

        // *** time_from_start ***
        if (!time_from_start.write(connection)) {
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::trajectory_msgs::MultiDOFJointTrajectoryPoint> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::trajectory_msgs::MultiDOFJointTrajectoryPoint> bottleStyle;

    // Give source text for class, ROS will need this
    yarp::os::ConstString getTypeText()
    {
        return "# Each multi-dof joint can specify a transform (up to 6 DOF)\n\
geometry_msgs/Transform[] transforms\n\
\n\
# There can be a velocity specified for the origin of the joint \n\
geometry_msgs/Twist[] velocities\n\
\n\
# There can be an acceleration specified for the origin of the joint \n\
geometry_msgs/Twist[] accelerations\n\
\n\
duration time_from_start\n================================================================================\n\
MSG: geometry_msgs/Transform\n\
\n================================================================================\n\
MSG: geometry_msgs/Twist\n\
\n================================================================================\n\
MSG: TickDuration\n\
";
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("trajectory_msgs/MultiDOFJointTrajectoryPoint", "trajectory_msgs/MultiDOFJointTrajectoryPoint");
        typ.addProperty("md5sum", yarp::os::Value("d41d8cd98f00b204e9800998ecf8427e"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace trajectory_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_trajectory_msgs_MultiDOFJointTrajectoryPoint_h

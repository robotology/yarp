// This is an automatically generated file.

// Generated from the following "geometry_msgs/PoseWithCovariance" msg definition:
//   # This represents a pose in free space with uncertainty.
//   
//   Pose pose
//   
//   # Row-major representation of the 6x6 covariance matrix
//   # The orientation parameters use a fixed-axis representation.
//   # In order, the parameters are:
//   # (x, y, z, rotation about X axis, rotation about Y axis, rotation about Z axis)
//   float64[36] covariance
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_geometry_msgs_PoseWithCovariance_h
#define YARP_ROSMSG_geometry_msgs_PoseWithCovariance_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/geometry_msgs/Pose.h>

namespace yarp {
namespace rosmsg {
namespace geometry_msgs {

class PoseWithCovariance : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::geometry_msgs::Pose pose;
    std::vector<yarp::os::NetFloat64> covariance;

    PoseWithCovariance() :
            pose(),
            covariance()
    {
        covariance.resize(36, 0.0);
    }

    void clear()
    {
        // *** pose ***
        pose.clear();

        // *** covariance ***
        covariance.clear();
        covariance.resize(36, 0.0);
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** pose ***
        if (!pose.read(connection)) {
            return false;
        }

        // *** covariance ***
        int len = 36;
        covariance.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&covariance[0], sizeof(yarp::os::NetFloat64)*len)) {
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

        // *** pose ***
        if (!pose.read(connection)) {
            return false;
        }

        // *** covariance ***
        if (connection.expectInt() != (BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE)) {
            return false;
        }
        int len = connection.expectInt();
        covariance.resize(len);
        for (int i=0; i<len; i++) {
            covariance[i] = (yarp::os::NetFloat64)connection.expectDouble();
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
        // *** pose ***
        if (!pose.write(connection)) {
            return false;
        }

        // *** covariance ***
        if (covariance.size()>0) {
            connection.appendExternalBlock((char*)&covariance[0], sizeof(yarp::os::NetFloat64)*covariance.size());
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(2);

        // *** pose ***
        if (!pose.write(connection)) {
            return false;
        }

        // *** covariance ***
        connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE);
        connection.appendInt(covariance.size());
        for (size_t i=0; i<covariance.size(); i++) {
            connection.appendDouble((double)covariance[i]);
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::geometry_msgs::PoseWithCovariance> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::geometry_msgs::PoseWithCovariance> bottleStyle;

    // Give source text for class, ROS will need this
    yarp::os::ConstString getTypeText()
    {
        return "# This represents a pose in free space with uncertainty.\n\
\n\
Pose pose\n\
\n\
# Row-major representation of the 6x6 covariance matrix\n\
# The orientation parameters use a fixed-axis representation.\n\
# In order, the parameters are:\n\
# (x, y, z, rotation about X axis, rotation about Y axis, rotation about Z axis)\n\
float64[36] covariance\n================================================================================\n\
MSG: geometry_msgs/Pose\n\
";
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("geometry_msgs/PoseWithCovariance", "geometry_msgs/PoseWithCovariance");
        typ.addProperty("md5sum", yarp::os::Value("d41d8cd98f00b204e9800998ecf8427e"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace geometry_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_geometry_msgs_PoseWithCovariance_h

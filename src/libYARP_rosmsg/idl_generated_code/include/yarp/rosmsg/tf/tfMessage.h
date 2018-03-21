// This is an automatically generated file.

// Generated from the following "tf/tfMessage" msg definition:
//   geometry_msgs/TransformStamped[] transforms
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_tf_tfMessage_h
#define YARP_ROSMSG_tf_tfMessage_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/geometry_msgs/TransformStamped.h>

namespace yarp {
namespace rosmsg {
namespace tf {

class tfMessage : public yarp::os::idl::WirePortable
{
public:
    std::vector<yarp::rosmsg::geometry_msgs::TransformStamped> transforms;

    tfMessage() :
            transforms()
    {
    }

    void clear()
    {
        // *** transforms ***
        transforms.clear();
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

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(1)) {
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

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(1);

        // *** transforms ***
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(transforms.size());
        for (size_t i=0; i<transforms.size(); i++) {
            if (!transforms[i].write(connection)) {
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::tf::tfMessage> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::tf::tfMessage> bottleStyle;

    // Give source text for class, ROS will need this
    yarp::os::ConstString getTypeText()
    {
        return "geometry_msgs/TransformStamped[] transforms\n================================================================================\n\
MSG: geometry_msgs/TransformStamped\n\
";
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("tf/tfMessage", "tf/tfMessage");
        typ.addProperty("md5sum", yarp::os::Value("d41d8cd98f00b204e9800998ecf8427e"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace tf
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_tf_tfMessage_h

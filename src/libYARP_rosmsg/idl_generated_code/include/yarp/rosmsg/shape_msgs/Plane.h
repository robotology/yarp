// This is an automatically generated file.

// Generated from the following "shape_msgs/Plane" msg definition:
//   # Representation of a plane, using the plane equation ax + by + cz + d = 0
//   
//   # a := coef[0]
//   # b := coef[1]
//   # c := coef[2]
//   # d := coef[3]
//   
//   float64[4] coef
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_shape_msgs_Plane_h
#define YARP_ROSMSG_shape_msgs_Plane_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>

namespace yarp {
namespace rosmsg {
namespace shape_msgs {

class Plane : public yarp::os::idl::WirePortable
{
public:
    std::vector<yarp::os::NetFloat64> coef;

    Plane() :
            coef()
    {
        coef.resize(4, 0.0);
    }

    void clear()
    {
        // *** coef ***
        coef.clear();
        coef.resize(4, 0.0);
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** coef ***
        int len = 4;
        coef.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&coef[0], sizeof(yarp::os::NetFloat64)*len)) {
            return false;
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

        // *** coef ***
        if (connection.expectInt() != (BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE)) {
            return false;
        }
        int len = connection.expectInt();
        coef.resize(len);
        for (int i=0; i<len; i++) {
            coef[i] = (yarp::os::NetFloat64)connection.expectDouble();
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
        // *** coef ***
        if (coef.size()>0) {
            connection.appendExternalBlock((char*)&coef[0], sizeof(yarp::os::NetFloat64)*coef.size());
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(1);

        // *** coef ***
        connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE);
        connection.appendInt(coef.size());
        for (size_t i=0; i<coef.size(); i++) {
            connection.appendDouble((double)coef[i]);
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::shape_msgs::Plane> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::shape_msgs::Plane> bottleStyle;

    // Give source text for class, ROS will need this
    yarp::os::ConstString getTypeText()
    {
        return "# Representation of a plane, using the plane equation ax + by + cz + d = 0\n\
\n\
# a := coef[0]\n\
# b := coef[1]\n\
# c := coef[2]\n\
# d := coef[3]\n\
\n\
float64[4] coef";
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("shape_msgs/Plane", "shape_msgs/Plane");
        typ.addProperty("md5sum", yarp::os::Value("d41d8cd98f00b204e9800998ecf8427e"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace shape_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_shape_msgs_Plane_h

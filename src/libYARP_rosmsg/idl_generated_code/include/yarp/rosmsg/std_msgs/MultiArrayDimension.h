// This is an automatically generated file.

// Generated from the following "std_msgs/MultiArrayDimension" msg definition:
//   string label   # label of given dimension
//   uint32 size    # size of given dimension (in type units)
//   uint32 stride  # stride of given dimension// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_std_msgs_MultiArrayDimension_h
#define YARP_ROSMSG_std_msgs_MultiArrayDimension_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>

namespace yarp {
namespace rosmsg {
namespace std_msgs {

class MultiArrayDimension : public yarp::os::idl::WirePortable
{
public:
    std::string label;
    yarp::os::NetUint32 size;
    yarp::os::NetUint32 stride;

    MultiArrayDimension() :
            label(""),
            size(0),
            stride(0)
    {
    }

    void clear()
    {
        // *** label ***
        label = "";

        // *** size ***
        size = 0;

        // *** stride ***
        stride = 0;
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** label ***
        int len = connection.expectInt();
        label.resize(len);
        if (!connection.expectBlock((char*)label.c_str(), len)) {
            return false;
        }

        // *** size ***
        size = connection.expectInt();

        // *** stride ***
        stride = connection.expectInt();

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(3)) {
            return false;
        }

        // *** label ***
        if (!reader.readString(label)) {
            return false;
        }

        // *** size ***
        size = reader.expectInt();

        // *** stride ***
        stride = reader.expectInt();

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
        // *** label ***
        connection.appendInt(label.length());
        connection.appendExternalBlock((char*)label.c_str(), label.length());

        // *** size ***
        connection.appendInt(size);

        // *** stride ***
        connection.appendInt(stride);

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(3);

        // *** label ***
        connection.appendInt(BOTTLE_TAG_STRING);
        connection.appendInt(label.length());
        connection.appendExternalBlock((char*)label.c_str(), label.length());

        // *** size ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)size);

        // *** stride ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)stride);

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::std_msgs::MultiArrayDimension> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::std_msgs::MultiArrayDimension> bottleStyle;

    // Give source text for class, ROS will need this
    static yarp::os::ConstString typeText()
    {
        return yarp::os::ConstString("\
string label   # label of given dimension\n\
uint32 size    # size of given dimension (in type units)\n\
uint32 stride  # stride of given dimension");
    }

    yarp::os::ConstString getTypeText() const
    {
        return yarp::rosmsg::std_msgs::MultiArrayDimension::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("std_msgs/MultiArrayDimension", "std_msgs/MultiArrayDimension");
        typ.addProperty("md5sum", yarp::os::Value("4cd0c83a8683deae40ecdac60e53bfa8"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace std_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_std_msgs_MultiArrayDimension_h

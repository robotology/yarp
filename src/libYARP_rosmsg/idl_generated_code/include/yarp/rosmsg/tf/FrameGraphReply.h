// This is an automatically generated file.

// Generated from the following "tf/FrameGraphReply" msg definition:
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_tf_FrameGraphReply_h
#define YARP_ROSMSG_tf_FrameGraphReply_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>

namespace yarp {
namespace rosmsg {
namespace tf {

class FrameGraphReply : public yarp::os::idl::WirePortable
{
public:
    std::string dot_graph;

    FrameGraphReply() :
            dot_graph("")
    {
    }

    void clear()
    {
        // *** dot_graph ***
        dot_graph = "";
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** dot_graph ***
        int len = connection.expectInt();
        dot_graph.resize(len);
        if (!connection.expectBlock((char*)dot_graph.c_str(), len)) {
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

        // *** dot_graph ***
        if (!reader.readString(dot_graph)) {
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
        // *** dot_graph ***
        connection.appendInt(dot_graph.length());
        connection.appendExternalBlock((char*)dot_graph.c_str(), dot_graph.length());

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(1);

        // *** dot_graph ***
        connection.appendInt(BOTTLE_TAG_STRING);
        connection.appendInt(dot_graph.length());
        connection.appendExternalBlock((char*)dot_graph.c_str(), dot_graph.length());

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::tf::FrameGraphReply> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::tf::FrameGraphReply> bottleStyle;

    // Give source text for class, ROS will need this
    yarp::os::ConstString getTypeText()
    {
        return "";
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("tf/FrameGraphReply", "tf/FrameGraphReply");
        typ.addProperty("md5sum", yarp::os::Value("d41d8cd98f00b204e9800998ecf8427e"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace tf
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_tf_FrameGraphReply_h

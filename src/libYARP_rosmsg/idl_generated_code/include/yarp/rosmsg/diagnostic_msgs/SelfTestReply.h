// This is an automatically generated file.

// Generated from the following "diagnostic_msgs/SelfTestReply" msg definition:
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_diagnostic_msgs_SelfTestReply_h
#define YARP_ROSMSG_diagnostic_msgs_SelfTestReply_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/diagnostic_msgs/DiagnosticStatus.h>

namespace yarp {
namespace rosmsg {
namespace diagnostic_msgs {

class SelfTestReply : public yarp::os::idl::WirePortable
{
public:
    std::string id;
    unsigned char passed;
    std::vector<yarp::rosmsg::diagnostic_msgs::DiagnosticStatus> status;

    SelfTestReply() :
            id(""),
            passed(0),
            status()
    {
    }

    void clear()
    {
        // *** id ***
        id = "";

        // *** passed ***
        passed = 0;

        // *** status ***
        status.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** id ***
        int len = connection.expectInt();
        id.resize(len);
        if (!connection.expectBlock((char*)id.c_str(), len)) {
            return false;
        }

        // *** passed ***
        if (!connection.expectBlock((char*)&passed, 1)) {
            return false;
        }

        // *** status ***
        len = connection.expectInt();
        status.resize(len);
        for (int i=0; i<len; i++) {
            if (!status[i].read(connection)) {
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

        // *** id ***
        if (!reader.readString(id)) {
            return false;
        }

        // *** passed ***
        passed = reader.expectInt();

        // *** status ***
        if (connection.expectInt() != BOTTLE_TAG_LIST) {
            return false;
        }
        int len = connection.expectInt();
        status.resize(len);
        for (int i=0; i<len; i++) {
            if (!status[i].read(connection)) {
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
        // *** id ***
        connection.appendInt(id.length());
        connection.appendExternalBlock((char*)id.c_str(), id.length());

        // *** passed ***
        connection.appendBlock((char*)&passed, 1);

        // *** status ***
        connection.appendInt(status.size());
        for (size_t i=0; i<status.size(); i++) {
            if (!status[i].write(connection)) {
                return false;
            }
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(3);

        // *** id ***
        connection.appendInt(BOTTLE_TAG_STRING);
        connection.appendInt(id.length());
        connection.appendExternalBlock((char*)id.c_str(), id.length());

        // *** passed ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)passed);

        // *** status ***
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(status.size());
        for (size_t i=0; i<status.size(); i++) {
            if (!status[i].write(connection)) {
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::diagnostic_msgs::SelfTestReply> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::diagnostic_msgs::SelfTestReply> bottleStyle;

    // Give source text for class, ROS will need this
    yarp::os::ConstString getTypeText()
    {
        return "\n================================================================================\n\
MSG: diagnostic_msgs/DiagnosticStatus\n\
";
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("diagnostic_msgs/SelfTestReply", "diagnostic_msgs/SelfTestReply");
        typ.addProperty("md5sum", yarp::os::Value("d41d8cd98f00b204e9800998ecf8427e"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace diagnostic_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_diagnostic_msgs_SelfTestReply_h

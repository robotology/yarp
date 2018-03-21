// This is an automatically generated file.

// Generated from the following "diagnostic_msgs/DiagnosticArray" msg definition:
//   # This message is used to send diagnostic information about the state of the robot
//   Header header #for timestamp
//   DiagnosticStatus[] status # an array of components being reported on// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_diagnostic_msgs_DiagnosticArray_h
#define YARP_ROSMSG_diagnostic_msgs_DiagnosticArray_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>
#include <yarp/rosmsg/diagnostic_msgs/DiagnosticStatus.h>

namespace yarp {
namespace rosmsg {
namespace diagnostic_msgs {

class DiagnosticArray : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::std_msgs::Header header;
    std::vector<yarp::rosmsg::diagnostic_msgs::DiagnosticStatus> status;

    DiagnosticArray() :
            header(),
            status()
    {
    }

    void clear()
    {
        // *** header ***
        header.clear();

        // *** status ***
        status.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** status ***
        int len = connection.expectInt();
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
        if (!reader.readListHeader(2)) {
            return false;
        }

        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

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
        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

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
        connection.appendInt(2);

        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::diagnostic_msgs::DiagnosticArray> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::diagnostic_msgs::DiagnosticArray> bottleStyle;

    // Give source text for class, ROS will need this
    yarp::os::ConstString getTypeText()
    {
        return "# This message is used to send diagnostic information about the state of the robot\n\
Header header #for timestamp\n\
DiagnosticStatus[] status # an array of components being reported on\n================================================================================\n\
MSG: std_msgs/Header\n\
\n================================================================================\n\
MSG: diagnostic_msgs/DiagnosticStatus\n\
";
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("diagnostic_msgs/DiagnosticArray", "diagnostic_msgs/DiagnosticArray");
        typ.addProperty("md5sum", yarp::os::Value("d41d8cd98f00b204e9800998ecf8427e"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace diagnostic_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_diagnostic_msgs_DiagnosticArray_h

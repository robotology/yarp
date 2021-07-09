/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "diagnostic_msgs/AddDiagnostics" msg definition:
//   # This service is used as part of the process for loading analyzers at runtime,
//   # and should be used by a loader script or program, not as a standalone service.
//   # Information about dynamic addition of analyzers can be found at
//   # http://wiki.ros.org/diagnostics/Tutorials/Adding%20Analyzers%20at%20Runtime
//   
//   # The load_namespace parameter defines the namespace where parameters for the
//   # initialization of analyzers in the diagnostic aggregator have been loaded. The
//   # value should be a global name (i.e. /my/name/space), not a relative
//   # (my/name/space) or private (~my/name/space) name. Analyzers will not be added
//   # if a non-global name is used. The call will also fail if the namespace
//   # contains parameters that follow a namespace structure that does not conform to
//   # that expected by the analyzer definitions. See
//   # http://wiki.ros.org/diagnostics/Tutorials/Configuring%20Diagnostic%20Aggregators
//   # and http://wiki.ros.org/diagnostics/Tutorials/Using%20the%20GenericAnalyzer
//   # for examples of the structure of yaml files which are expected to have been
//   # loaded into the namespace.
//   string load_namespace
//   ---
//   
//   # True if diagnostic aggregator was updated with new diagnostics, False
//   # otherwise. A false return value means that either there is a bond in the
//   # aggregator which already used the requested namespace, or the initialization
//   # of analyzers failed.
//   bool success
//   
//   # Message with additional information about the success or failure
//   string message
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_diagnostic_msgs_AddDiagnostics_h
#define YARP_ROSMSG_diagnostic_msgs_AddDiagnostics_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>

namespace yarp {
namespace rosmsg {
namespace diagnostic_msgs {

class AddDiagnostics : public yarp::os::idl::WirePortable
{
public:
    std::string load_namespace;

    AddDiagnostics() :
            load_namespace("")
    {
    }

    void clear()
    {
        // *** load_namespace ***
        load_namespace = "";
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** load_namespace ***
        int len = connection.expectInt32();
        load_namespace.resize(len);
        if (!connection.expectBlock((char*)load_namespace.c_str(), len)) {
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

        // *** load_namespace ***
        if (!reader.readString(load_namespace)) {
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

    bool writeBare(yarp::os::ConnectionWriter& connection) const override
    {
        // *** load_namespace ***
        connection.appendInt32(load_namespace.length());
        connection.appendExternalBlock((char*)load_namespace.c_str(), load_namespace.length());

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(1);

        // *** load_namespace ***
        connection.appendInt32(BOTTLE_TAG_STRING);
        connection.appendInt32(load_namespace.length());
        connection.appendExternalBlock((char*)load_namespace.c_str(), load_namespace.length());

        connection.convertTextMode();
        return !connection.isError();
    }

    using yarp::os::idl::WirePortable::write;
    bool write(yarp::os::ConnectionWriter& connection) const override
    {
        return (connection.isBareMode() ? writeBare(connection)
                                        : writeBottle(connection));
    }

    // This class will serialize ROS style or YARP style depending on protocol.
    // If you need to force a serialization style, use one of these classes:
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::diagnostic_msgs::AddDiagnostics> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::diagnostic_msgs::AddDiagnostics> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "diagnostic_msgs/AddDiagnostics";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "c26cf6e164288fbc6050d74f838bcdf0";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
# This service is used as part of the process for loading analyzers at runtime,\n\
# and should be used by a loader script or program, not as a standalone service.\n\
# Information about dynamic addition of analyzers can be found at\n\
# http://wiki.ros.org/diagnostics/Tutorials/Adding%20Analyzers%20at%20Runtime\n\
\n\
# The load_namespace parameter defines the namespace where parameters for the\n\
# initialization of analyzers in the diagnostic aggregator have been loaded. The\n\
# value should be a global name (i.e. /my/name/space), not a relative\n\
# (my/name/space) or private (~my/name/space) name. Analyzers will not be added\n\
# if a non-global name is used. The call will also fail if the namespace\n\
# contains parameters that follow a namespace structure that does not conform to\n\
# that expected by the analyzer definitions. See\n\
# http://wiki.ros.org/diagnostics/Tutorials/Configuring%20Diagnostic%20Aggregators\n\
# and http://wiki.ros.org/diagnostics/Tutorials/Using%20the%20GenericAnalyzer\n\
# for examples of the structure of yaml files which are expected to have been\n\
# loaded into the namespace.\n\
string load_namespace\n\
---\n\
\n\
# True if diagnostic aggregator was updated with new diagnostics, False\n\
# otherwise. A false return value means that either there is a bond in the\n\
# aggregator which already used the requested namespace, or the initialization\n\
# of analyzers failed.\n\
bool success\n\
\n\
# Message with additional information about the success or failure\n\
string message\n\
";

    yarp::os::Type getType() const override
    {
        yarp::os::Type typ = yarp::os::Type::byName(typeName, typeName);
        typ.addProperty("md5sum", yarp::os::Value(typeChecksum));
        typ.addProperty("message_definition", yarp::os::Value(typeText));
        return typ;
    }
};

} // namespace diagnostic_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_diagnostic_msgs_AddDiagnostics_h

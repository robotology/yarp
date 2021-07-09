/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "sensor_msgs/NavSatStatus" msg definition:
//   # Navigation Satellite fix status for any Global Navigation Satellite System
//   
//   # Whether to output an augmented fix is determined by both the fix
//   # type and the last time differential corrections were received.  A
//   # fix is valid when status >= STATUS_FIX.
//   
//   int8 STATUS_NO_FIX =  -1        # unable to fix position
//   int8 STATUS_FIX =      0        # unaugmented fix
//   int8 STATUS_SBAS_FIX = 1        # with satellite-based augmentation
//   int8 STATUS_GBAS_FIX = 2        # with ground-based augmentation
//   
//   int8 status
//   
//   # Bits defining which Global Navigation Satellite System signals were
//   # used by the receiver.
//   
//   uint16 SERVICE_GPS =     1
//   uint16 SERVICE_GLONASS = 2
//   uint16 SERVICE_COMPASS = 4      # includes BeiDou.
//   uint16 SERVICE_GALILEO = 8
//   
//   uint16 service
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_sensor_msgs_NavSatStatus_h
#define YARP_ROSMSG_sensor_msgs_NavSatStatus_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>

namespace yarp {
namespace rosmsg {
namespace sensor_msgs {

class NavSatStatus : public yarp::os::idl::WirePortable
{
public:
    static const std::int8_t STATUS_NO_FIX = -1;
    static const std::int8_t STATUS_FIX = 0;
    static const std::int8_t STATUS_SBAS_FIX = 1;
    static const std::int8_t STATUS_GBAS_FIX = 2;
    std::int8_t status;
    static const std::uint16_t SERVICE_GPS = 1;
    static const std::uint16_t SERVICE_GLONASS = 2;
    static const std::uint16_t SERVICE_COMPASS = 4;
    static const std::uint16_t SERVICE_GALILEO = 8;
    std::uint16_t service;

    NavSatStatus() :
            status(0),
            service(0)
    {
    }

    void clear()
    {
        // *** STATUS_NO_FIX ***

        // *** STATUS_FIX ***

        // *** STATUS_SBAS_FIX ***

        // *** STATUS_GBAS_FIX ***

        // *** status ***
        status = 0;

        // *** SERVICE_GPS ***

        // *** SERVICE_GLONASS ***

        // *** SERVICE_COMPASS ***

        // *** SERVICE_GALILEO ***

        // *** service ***
        service = 0;
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** status ***
        status = connection.expectInt8();

        // *** service ***
        service = connection.expectInt16();

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(10)) {
            return false;
        }

        // *** status ***
        status = reader.expectInt8();

        // *** service ***
        service = reader.expectInt16();

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
        // *** status ***
        connection.appendInt8(status);

        // *** service ***
        connection.appendInt16(service);

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(10);

        // *** status ***
        connection.appendInt32(BOTTLE_TAG_INT8);
        connection.appendInt8(status);

        // *** service ***
        connection.appendInt32(BOTTLE_TAG_INT16);
        connection.appendInt16(service);

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::NavSatStatus> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::NavSatStatus> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "sensor_msgs/NavSatStatus";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "331cdbddfa4bc96ffc3b9ad98900a54c";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
# Navigation Satellite fix status for any Global Navigation Satellite System\n\
\n\
# Whether to output an augmented fix is determined by both the fix\n\
# type and the last time differential corrections were received.  A\n\
# fix is valid when status >= STATUS_FIX.\n\
\n\
int8 STATUS_NO_FIX =  -1        # unable to fix position\n\
int8 STATUS_FIX =      0        # unaugmented fix\n\
int8 STATUS_SBAS_FIX = 1        # with satellite-based augmentation\n\
int8 STATUS_GBAS_FIX = 2        # with ground-based augmentation\n\
\n\
int8 status\n\
\n\
# Bits defining which Global Navigation Satellite System signals were\n\
# used by the receiver.\n\
\n\
uint16 SERVICE_GPS =     1\n\
uint16 SERVICE_GLONASS = 2\n\
uint16 SERVICE_COMPASS = 4      # includes BeiDou.\n\
uint16 SERVICE_GALILEO = 8\n\
\n\
uint16 service\n\
";

    yarp::os::Type getType() const override
    {
        yarp::os::Type typ = yarp::os::Type::byName(typeName, typeName);
        typ.addProperty("md5sum", yarp::os::Value(typeChecksum));
        typ.addProperty("message_definition", yarp::os::Value(typeText));
        return typ;
    }
};

} // namespace sensor_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_sensor_msgs_NavSatStatus_h

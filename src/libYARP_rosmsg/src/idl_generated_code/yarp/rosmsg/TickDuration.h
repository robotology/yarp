/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "duration" native type definition:
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_TickDuration_h
#define YARP_ROSMSG_TickDuration_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <climits>
#include <cstdint>

namespace yarp {
namespace rosmsg {

class TickDuration : public yarp::os::idl::WirePortable
{
public:
    std::uint32_t sec;
    std::uint32_t nsec;

    TickDuration() :
            sec(0),
            nsec(0)
    {
    }

    TickDuration(double timestamp) :
            sec(0),
            nsec(0)
    {
        uint64_t time = (uint64_t) (timestamp * 1000000000UL);
        uint64_t sec_part = (time / 1000000000UL);
        uint64_t nsec_part = (time % 1000000000UL);
        if (sec > UINT32_MAX) {
            yWarning("TickDuration::TickDuration(): Timestamp exceeded the 32 bit representation, resetting it to 0");
            sec = 0;
        }
        sec = static_cast<yarp::os::NetUint32>(sec_part);
        nsec = static_cast<yarp::os::NetUint32>(nsec_part);
    }

    TickDuration& operator=(const double timestamp)
    {
        uint64_t time = (uint64_t) (timestamp * 1000000000UL);
        uint64_t sec_part = (time / 1000000000UL);
        uint64_t nsec_part = (time % 1000000000UL);
        if (sec > UINT32_MAX) {
            yWarning("TickDuration::operator=(): Timestamp exceeded the 32 bit representation, resetting it to 0");
            sec = 0;
        }
        sec = static_cast<yarp::os::NetUint32>(sec_part);
        nsec = static_cast<yarp::os::NetUint32>(nsec_part);
        return *this;
    }

    operator double()
    {
        if (nsec > 1000000000UL) {
            yWarning("TickDuration::operator double(): Check on nsec > 1000000000UL failed");
        }
        return sec + nsec * 1000000000.0;
    }

    void clear()
    {
        // *** sec ***
        sec = 0;

        // *** nsec ***
        nsec = 0;
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** sec ***
        sec = connection.expectInt32();

        // *** nsec ***
        nsec = connection.expectInt32();

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(2)) {
            return false;
        }

        // *** sec ***
        sec = reader.expectInt32();

        // *** nsec ***
        nsec = reader.expectInt32();

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
        // *** sec ***
        connection.appendInt32(sec);

        // *** nsec ***
        connection.appendInt32(nsec);

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(2);

        // *** sec ***
        connection.appendInt32(BOTTLE_TAG_INT32);
        connection.appendInt32(sec);

        // *** nsec ***
        connection.appendInt32(BOTTLE_TAG_INT32);
        connection.appendInt32(nsec);

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::TickDuration> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::TickDuration> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "TickDuration";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "4771ad66fef816d2e4bead2f45a1cde6";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
\n\
";

    yarp::os::Type getType() const override
    {
        yarp::os::Type typ = yarp::os::Type::byName(typeName, typeName);
        typ.addProperty("md5sum", yarp::os::Value(typeChecksum));
        typ.addProperty("message_definition", yarp::os::Value(typeText));
        return typ;
    }
};

} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_TickDuration_h

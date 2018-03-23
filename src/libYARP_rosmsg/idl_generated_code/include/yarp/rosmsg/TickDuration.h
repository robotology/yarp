/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.

// Generated from the following "duration" native type definition:
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_TickDuration_h
#define YARP_ROSMSG_TickDuration_h

#include <yarp/os/Wire.h>
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
    yarp::os::NetUint32 sec;
    yarp::os::NetUint32 nsec;

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
        sec = connection.expectInt();

        // *** nsec ***
        nsec = connection.expectInt();

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
        sec = reader.expectInt();

        // *** nsec ***
        nsec = reader.expectInt();

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
        // *** sec ***
        connection.appendInt(sec);

        // *** nsec ***
        connection.appendInt(nsec);

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(2);

        // *** sec ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)sec);

        // *** nsec ***
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt((int)nsec);

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::TickDuration> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::TickDuration> bottleStyle;

    // Give source text for class, ROS will need this
    static yarp::os::ConstString typeText()
    {
        return yarp::os::ConstString("\
");
    }

    yarp::os::ConstString getTypeText() const
    {
        return yarp::rosmsg::TickDuration::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("TickDuration", "TickDuration");
        typ.addProperty("md5sum", yarp::os::Value("4f8dc7710c22b42c7b09295dcda33fa0"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_TickDuration_h

// This is an automatically generated file.

// Generated from the following "time" native type definition:
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_TickTime_h
#define YARP_ROSMSG_TickTime_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <climits>
#include <cstdint>

namespace yarp {
namespace rosmsg {

class TickTime : public yarp::os::idl::WirePortable
{
public:
    yarp::os::NetUint32 sec;
    yarp::os::NetUint32 nsec;

    TickTime() :
            sec(0),
            nsec(0)
    {
    }

    TickTime(double timestamp) :
            sec(0),
            nsec(0)
    {
        uint64_t time = (uint64_t) (timestamp * 1000000000UL);
        uint64_t sec_part = (time / 1000000000UL);
        uint64_t nsec_part = (time % 1000000000UL);
        if (sec > UINT32_MAX) {
            yWarning("TickTime::TickTime(): Timestamp exceeded the 32 bit representation, resetting it to 0");
            sec = 0;
        }
        sec = static_cast<yarp::os::NetUint32>(sec_part);
        nsec = static_cast<yarp::os::NetUint32>(nsec_part);
    }

    TickTime& operator=(const double timestamp)
    {
        uint64_t time = (uint64_t) (timestamp * 1000000000UL);
        uint64_t sec_part = (time / 1000000000UL);
        uint64_t nsec_part = (time % 1000000000UL);
        if (sec > UINT32_MAX) {
            yWarning("TickTime::operator=(): Timestamp exceeded the 32 bit representation, resetting it to 0");
            sec = 0;
        }
        sec = static_cast<yarp::os::NetUint32>(sec_part);
        nsec = static_cast<yarp::os::NetUint32>(nsec_part);
        return *this;
    }

    operator double()
    {
        if (nsec > 1000000000UL) {
            yWarning("TickTime::operator double(): Check on nsec > 1000000000UL failed");
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::TickTime> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::TickTime> bottleStyle;

    // Give source text for class, ROS will need this
    yarp::os::ConstString getTypeText()
    {
        return "";
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("TickTime", "TickTime");
        typ.addProperty("md5sum", yarp::os::Value("d41d8cd98f00b204e9800998ecf8427e"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_TickTime_h

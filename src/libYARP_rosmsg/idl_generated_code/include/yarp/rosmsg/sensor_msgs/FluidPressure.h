/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.

// Generated from the following "sensor_msgs/FluidPressure" msg definition:
//    # Single pressure reading.  This message is appropriate for measuring the
//    # pressure inside of a fluid (air, water, etc).  This also includes
//    # atmospheric or barometric pressure.
//   
//    # This message is not appropriate for force/pressure contact sensors.
//   
//    Header header           # timestamp of the measurement
//                            # frame_id is the location of the pressure sensor
//   
//    float64 fluid_pressure  # Absolute pressure reading in Pascals.
//   
//    float64 variance        # 0 is interpreted as variance unknown// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_sensor_msgs_FluidPressure_h
#define YARP_ROSMSG_sensor_msgs_FluidPressure_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>

namespace yarp {
namespace rosmsg {
namespace sensor_msgs {

class FluidPressure : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::std_msgs::Header header;
    yarp::conf::float64_t fluid_pressure;
    yarp::conf::float64_t variance;

    FluidPressure() :
            header(),
            fluid_pressure(0.0),
            variance(0.0)
    {
    }

    void clear()
    {
        // *** header ***
        header.clear();

        // *** fluid_pressure ***
        fluid_pressure = 0.0;

        // *** variance ***
        variance = 0.0;
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** fluid_pressure ***
        fluid_pressure = connection.expectFloat64();

        // *** variance ***
        variance = connection.expectFloat64();

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(3)) {
            return false;
        }

        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** fluid_pressure ***
        fluid_pressure = reader.expectFloat64();

        // *** variance ***
        variance = reader.expectFloat64();

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

        // *** fluid_pressure ***
        connection.appendFloat64(fluid_pressure);

        // *** variance ***
        connection.appendFloat64(variance);

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(3);

        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** fluid_pressure ***
        connection.appendInt32(BOTTLE_TAG_FLOAT64);
        connection.appendFloat64(fluid_pressure);

        // *** variance ***
        connection.appendInt32(BOTTLE_TAG_FLOAT64);
        connection.appendFloat64(variance);

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::FluidPressure> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::FluidPressure> bottleStyle;

    // Give source text for class, ROS will need this
    static std::string typeText()
    {
        return std::string("\
 # Single pressure reading.  This message is appropriate for measuring the\n\
 # pressure inside of a fluid (air, water, etc).  This also includes\n\
 # atmospheric or barometric pressure.\n\
\n\
 # This message is not appropriate for force/pressure contact sensors.\n\
\n\
 Header header           # timestamp of the measurement\n\
                         # frame_id is the location of the pressure sensor\n\
\n\
 float64 fluid_pressure  # Absolute pressure reading in Pascals.\n\
\n\
 float64 variance        # 0 is interpreted as variance unknown") + std::string("\n\
================================================================================\n\
MSG: std_msgs/Header\n\
") + yarp::rosmsg::std_msgs::Header::typeText();
    }

    std::string getTypeText() const
    {
        return yarp::rosmsg::sensor_msgs::FluidPressure::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() const override
    {
        yarp::os::Type typ = yarp::os::Type::byName("sensor_msgs/FluidPressure", "sensor_msgs/FluidPressure");
        typ.addProperty("md5sum", yarp::os::Value("804dc5cea1c5306d6a2eb80b9833befe"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace sensor_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_sensor_msgs_FluidPressure_h

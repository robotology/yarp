/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.

// Generated from the following "sensor_msgs/MagneticField" msg definition:
//    # Measurement of the Magnetic Field vector at a specific location.
//   
//    # If the covariance of the measurement is known, it should be filled in
//    # (if all you know is the variance of each measurement, e.g. from the datasheet,
//    #just put those along the diagonal)
//    # A covariance matrix of all zeros will be interpreted as "covariance unknown",
//    # and to use the data a covariance will have to be assumed or gotten from some
//    # other source
//   
//   
//    Header header                        # timestamp is the time the
//                                         # field was measured
//                                         # frame_id is the location and orientation
//                                         # of the field measurement
//   
//    geometry_msgs/Vector3 magnetic_field # x, y, and z components of the
//                                         # field vector in Tesla
//                                         # If your sensor does not output 3 axes,
//                                         # put NaNs in the components not reported.
//   
//    float64[9] magnetic_field_covariance # Row major about x, y, z axes
//                                         # 0 is interpreted as variance unknown// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_sensor_msgs_MagneticField_h
#define YARP_ROSMSG_sensor_msgs_MagneticField_h

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>
#include <yarp/rosmsg/geometry_msgs/Vector3.h>

namespace yarp {
namespace rosmsg {
namespace sensor_msgs {

class MagneticField : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::std_msgs::Header header;
    yarp::rosmsg::geometry_msgs::Vector3 magnetic_field;
    std::vector<yarp::conf::float64_t> magnetic_field_covariance;

    MagneticField() :
            header(),
            magnetic_field(),
            magnetic_field_covariance()
    {
        magnetic_field_covariance.resize(9, 0.0);
    }

    void clear()
    {
        // *** header ***
        header.clear();

        // *** magnetic_field ***
        magnetic_field.clear();

        // *** magnetic_field_covariance ***
        magnetic_field_covariance.clear();
        magnetic_field_covariance.resize(9, 0.0);
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** magnetic_field ***
        if (!magnetic_field.read(connection)) {
            return false;
        }

        // *** magnetic_field_covariance ***
        int len = 9;
        magnetic_field_covariance.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&magnetic_field_covariance[0], sizeof(yarp::conf::float64_t)*len)) {
            return false;
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

        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** magnetic_field ***
        if (!magnetic_field.read(connection)) {
            return false;
        }

        // *** magnetic_field_covariance ***
        if (connection.expectInt32() != (BOTTLE_TAG_LIST|BOTTLE_TAG_FLOAT64)) {
            return false;
        }
        int len = connection.expectInt32();
        magnetic_field_covariance.resize(len);
        for (int i=0; i<len; i++) {
            magnetic_field_covariance[i] = (yarp::conf::float64_t)connection.expectFloat64();
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

        // *** magnetic_field ***
        if (!magnetic_field.write(connection)) {
            return false;
        }

        // *** magnetic_field_covariance ***
        if (magnetic_field_covariance.size()>0) {
            connection.appendExternalBlock((char*)&magnetic_field_covariance[0], sizeof(yarp::conf::float64_t)*magnetic_field_covariance.size());
        }

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

        // *** magnetic_field ***
        if (!magnetic_field.write(connection)) {
            return false;
        }

        // *** magnetic_field_covariance ***
        connection.appendInt32(BOTTLE_TAG_LIST|BOTTLE_TAG_FLOAT64);
        connection.appendInt32(magnetic_field_covariance.size());
        for (size_t i=0; i<magnetic_field_covariance.size(); i++) {
            connection.appendFloat64(magnetic_field_covariance[i]);
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::MagneticField> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::MagneticField> bottleStyle;

    // Give source text for class, ROS will need this
    static std::string typeText()
    {
        return std::string("\
 # Measurement of the Magnetic Field vector at a specific location.\n\
\n\
 # If the covariance of the measurement is known, it should be filled in\n\
 # (if all you know is the variance of each measurement, e.g. from the datasheet,\n\
 #just put those along the diagonal)\n\
 # A covariance matrix of all zeros will be interpreted as \"covariance unknown\",\n\
 # and to use the data a covariance will have to be assumed or gotten from some\n\
 # other source\n\
\n\
\n\
 Header header                        # timestamp is the time the\n\
                                      # field was measured\n\
                                      # frame_id is the location and orientation\n\
                                      # of the field measurement\n\
\n\
 geometry_msgs/Vector3 magnetic_field # x, y, and z components of the\n\
                                      # field vector in Tesla\n\
                                      # If your sensor does not output 3 axes,\n\
                                      # put NaNs in the components not reported.\n\
\n\
 float64[9] magnetic_field_covariance # Row major about x, y, z axes\n\
                                      # 0 is interpreted as variance unknown") + std::string("\n\
================================================================================\n\
MSG: std_msgs/Header\n\
") + yarp::rosmsg::std_msgs::Header::typeText() + std::string("\n\
================================================================================\n\
MSG: geometry_msgs/Vector3\n\
") + yarp::rosmsg::geometry_msgs::Vector3::typeText();
    }

    std::string getTypeText() const
    {
        return yarp::rosmsg::sensor_msgs::MagneticField::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() const override
    {
        yarp::os::Type typ = yarp::os::Type::byName("sensor_msgs/MagneticField", "sensor_msgs/MagneticField");
        typ.addProperty("md5sum", yarp::os::Value("2f3b0b43eed0c9501de0fa3ff89a45aa"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace sensor_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_sensor_msgs_MagneticField_h

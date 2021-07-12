/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
#include <yarp/os/Type.h>
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

    bool writeBare(yarp::os::ConnectionWriter& connection) const override
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

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
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
    bool write(yarp::os::ConnectionWriter& connection) const override
    {
        return (connection.isBareMode() ? writeBare(connection)
                                        : writeBottle(connection));
    }

    // This class will serialize ROS style or YARP style depending on protocol.
    // If you need to force a serialization style, use one of these classes:
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::sensor_msgs::MagneticField> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::sensor_msgs::MagneticField> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "sensor_msgs/MagneticField";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "2f3b0b43eed0c9501de0fa3ff89a45aa";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
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
                                      # 0 is interpreted as variance unknown\n\
================================================================================\n\
MSG: std_msgs/Header\n\
# Standard metadata for higher-level stamped data types.\n\
# This is generally used to communicate timestamped data \n\
# in a particular coordinate frame.\n\
# \n\
# sequence ID: consecutively increasing ID \n\
uint32 seq\n\
#Two-integer timestamp that is expressed as:\n\
# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')\n\
# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')\n\
# time-handling sugar is provided by the client library\n\
time stamp\n\
#Frame this data is associated with\n\
# 0: no frame\n\
# 1: global frame\n\
string frame_id\n\
\n\
================================================================================\n\
MSG: geometry_msgs/Vector3\n\
# This represents a vector in free space. \n\
# It is only meant to represent a direction. Therefore, it does not\n\
# make sense to apply a translation to it (e.g., when applying a \n\
# generic rigid transformation to a Vector3, tf2 will only apply the\n\
# rotation). If you want your data to be translatable too, use the\n\
# geometry_msgs/Point message instead.\n\
\n\
float64 x\n\
float64 y\n\
float64 z\n\
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

#endif // YARP_ROSMSG_sensor_msgs_MagneticField_h

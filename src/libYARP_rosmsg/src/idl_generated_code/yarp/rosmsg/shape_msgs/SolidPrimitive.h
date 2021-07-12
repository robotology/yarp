/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "shape_msgs/SolidPrimitive" msg definition:
//   # Define box, sphere, cylinder, cone 
//   # All shapes are defined to have their bounding boxes centered around 0,0,0.
//   
//   uint8 BOX=1
//   uint8 SPHERE=2
//   uint8 CYLINDER=3
//   uint8 CONE=4
//   
//   # The type of the shape
//   uint8 type
//   
//   
//   # The dimensions of the shape
//   float64[] dimensions
//   
//   # The meaning of the shape dimensions: each constant defines the index in the 'dimensions' array
//   
//   # For the BOX type, the X, Y, and Z dimensions are the length of the corresponding
//   # sides of the box.
//   uint8 BOX_X=0
//   uint8 BOX_Y=1
//   uint8 BOX_Z=2
//   
//   
//   # For the SPHERE type, only one component is used, and it gives the radius of
//   # the sphere.
//   uint8 SPHERE_RADIUS=0
//   
//   
//   # For the CYLINDER and CONE types, the center line is oriented along
//   # the Z axis.  Therefore the CYLINDER_HEIGHT (CONE_HEIGHT) component
//   # of dimensions gives the height of the cylinder (cone).  The
//   # CYLINDER_RADIUS (CONE_RADIUS) component of dimensions gives the
//   # radius of the base of the cylinder (cone).  Cone and cylinder
//   # primitives are defined to be circular. The tip of the cone is
//   # pointing up, along +Z axis.
//   
//   uint8 CYLINDER_HEIGHT=0
//   uint8 CYLINDER_RADIUS=1
//   
//   uint8 CONE_HEIGHT=0
//   uint8 CONE_RADIUS=1
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_shape_msgs_SolidPrimitive_h
#define YARP_ROSMSG_shape_msgs_SolidPrimitive_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>

namespace yarp {
namespace rosmsg {
namespace shape_msgs {

class SolidPrimitive : public yarp::os::idl::WirePortable
{
public:
    static const std::uint8_t BOX = 1;
    static const std::uint8_t SPHERE = 2;
    static const std::uint8_t CYLINDER = 3;
    static const std::uint8_t CONE = 4;
    std::uint8_t type;
    std::vector<yarp::conf::float64_t> dimensions;
    static const std::uint8_t BOX_X = 0;
    static const std::uint8_t BOX_Y = 1;
    static const std::uint8_t BOX_Z = 2;
    static const std::uint8_t SPHERE_RADIUS = 0;
    static const std::uint8_t CYLINDER_HEIGHT = 0;
    static const std::uint8_t CYLINDER_RADIUS = 1;
    static const std::uint8_t CONE_HEIGHT = 0;
    static const std::uint8_t CONE_RADIUS = 1;

    SolidPrimitive() :
            type(0),
            dimensions()
    {
    }

    void clear()
    {
        // *** BOX ***

        // *** SPHERE ***

        // *** CYLINDER ***

        // *** CONE ***

        // *** type ***
        type = 0;

        // *** dimensions ***
        dimensions.clear();

        // *** BOX_X ***

        // *** BOX_Y ***

        // *** BOX_Z ***

        // *** SPHERE_RADIUS ***

        // *** CYLINDER_HEIGHT ***

        // *** CYLINDER_RADIUS ***

        // *** CONE_HEIGHT ***

        // *** CONE_RADIUS ***
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** type ***
        type = connection.expectInt8();

        // *** dimensions ***
        int len = connection.expectInt32();
        dimensions.resize(len);
        if (len > 0 && !connection.expectBlock((char*)&dimensions[0], sizeof(yarp::conf::float64_t)*len)) {
            return false;
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(14)) {
            return false;
        }

        // *** type ***
        type = reader.expectInt8();

        // *** dimensions ***
        if (connection.expectInt32() != (BOTTLE_TAG_LIST|BOTTLE_TAG_FLOAT64)) {
            return false;
        }
        int len = connection.expectInt32();
        dimensions.resize(len);
        for (int i=0; i<len; i++) {
            dimensions[i] = (yarp::conf::float64_t)connection.expectFloat64();
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
        // *** type ***
        connection.appendInt8(type);

        // *** dimensions ***
        connection.appendInt32(dimensions.size());
        if (dimensions.size()>0) {
            connection.appendExternalBlock((char*)&dimensions[0], sizeof(yarp::conf::float64_t)*dimensions.size());
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(14);

        // *** type ***
        connection.appendInt32(BOTTLE_TAG_INT8);
        connection.appendInt8(type);

        // *** dimensions ***
        connection.appendInt32(BOTTLE_TAG_LIST|BOTTLE_TAG_FLOAT64);
        connection.appendInt32(dimensions.size());
        for (size_t i=0; i<dimensions.size(); i++) {
            connection.appendFloat64(dimensions[i]);
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::shape_msgs::SolidPrimitive> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::shape_msgs::SolidPrimitive> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "shape_msgs/SolidPrimitive";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "d8f8cbc74c5ff283fca29569ccefb45d";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
# Define box, sphere, cylinder, cone \n\
# All shapes are defined to have their bounding boxes centered around 0,0,0.\n\
\n\
uint8 BOX=1\n\
uint8 SPHERE=2\n\
uint8 CYLINDER=3\n\
uint8 CONE=4\n\
\n\
# The type of the shape\n\
uint8 type\n\
\n\
\n\
# The dimensions of the shape\n\
float64[] dimensions\n\
\n\
# The meaning of the shape dimensions: each constant defines the index in the 'dimensions' array\n\
\n\
# For the BOX type, the X, Y, and Z dimensions are the length of the corresponding\n\
# sides of the box.\n\
uint8 BOX_X=0\n\
uint8 BOX_Y=1\n\
uint8 BOX_Z=2\n\
\n\
\n\
# For the SPHERE type, only one component is used, and it gives the radius of\n\
# the sphere.\n\
uint8 SPHERE_RADIUS=0\n\
\n\
\n\
# For the CYLINDER and CONE types, the center line is oriented along\n\
# the Z axis.  Therefore the CYLINDER_HEIGHT (CONE_HEIGHT) component\n\
# of dimensions gives the height of the cylinder (cone).  The\n\
# CYLINDER_RADIUS (CONE_RADIUS) component of dimensions gives the\n\
# radius of the base of the cylinder (cone).  Cone and cylinder\n\
# primitives are defined to be circular. The tip of the cone is\n\
# pointing up, along +Z axis.\n\
\n\
uint8 CYLINDER_HEIGHT=0\n\
uint8 CYLINDER_RADIUS=1\n\
\n\
uint8 CONE_HEIGHT=0\n\
uint8 CONE_RADIUS=1\n\
";

    yarp::os::Type getType() const override
    {
        yarp::os::Type typ = yarp::os::Type::byName(typeName, typeName);
        typ.addProperty("md5sum", yarp::os::Value(typeChecksum));
        typ.addProperty("message_definition", yarp::os::Value(typeText));
        return typ;
    }
};

} // namespace shape_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_shape_msgs_SolidPrimitive_h

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "std_msgs/MultiArrayLayout" msg definition:
//   # The multiarray declares a generic multi-dimensional array of a
//   # particular data type.  Dimensions are ordered from outer most
//   # to inner most.
//   
//   MultiArrayDimension[] dim # Array of dimension properties
//   uint32 data_offset        # padding elements at front of data
//   
//   # Accessors should ALWAYS be written in terms of dimension stride
//   # and specified outer-most dimension first.
//   # 
//   # multiarray(i,j,k) = data[data_offset + dim_stride[1]*i + dim_stride[2]*j + k]
//   #
//   # A standard, 3-channel 640x480 image with interleaved color channels
//   # would be specified as:
//   #
//   # dim[0].label  = "height"
//   # dim[0].size   = 480
//   # dim[0].stride = 3*640*480 = 921600  (note dim[0] stride is just size of image)
//   # dim[1].label  = "width"
//   # dim[1].size   = 640
//   # dim[1].stride = 3*640 = 1920
//   # dim[2].label  = "channel"
//   # dim[2].size   = 3
//   # dim[2].stride = 3
//   #
//   # multiarray(i,j,k) refers to the ith row, jth column, and kth channel.
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_std_msgs_MultiArrayLayout_h
#define YARP_ROSMSG_std_msgs_MultiArrayLayout_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/MultiArrayDimension.h>

namespace yarp {
namespace rosmsg {
namespace std_msgs {

class MultiArrayLayout : public yarp::os::idl::WirePortable
{
public:
    std::vector<yarp::rosmsg::std_msgs::MultiArrayDimension> dim;
    std::uint32_t data_offset;

    MultiArrayLayout() :
            dim(),
            data_offset(0)
    {
    }

    void clear()
    {
        // *** dim ***
        dim.clear();

        // *** data_offset ***
        data_offset = 0;
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** dim ***
        int len = connection.expectInt32();
        dim.resize(len);
        for (int i=0; i<len; i++) {
            if (!dim[i].read(connection)) {
                return false;
            }
        }

        // *** data_offset ***
        data_offset = connection.expectInt32();

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(2)) {
            return false;
        }

        // *** dim ***
        if (connection.expectInt32() != BOTTLE_TAG_LIST) {
            return false;
        }
        int len = connection.expectInt32();
        dim.resize(len);
        for (int i=0; i<len; i++) {
            if (!dim[i].read(connection)) {
                return false;
            }
        }

        // *** data_offset ***
        data_offset = reader.expectInt32();

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
        // *** dim ***
        connection.appendInt32(dim.size());
        for (size_t i=0; i<dim.size(); i++) {
            if (!dim[i].write(connection)) {
                return false;
            }
        }

        // *** data_offset ***
        connection.appendInt32(data_offset);

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(2);

        // *** dim ***
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(dim.size());
        for (size_t i=0; i<dim.size(); i++) {
            if (!dim[i].write(connection)) {
                return false;
            }
        }

        // *** data_offset ***
        connection.appendInt32(BOTTLE_TAG_INT32);
        connection.appendInt32(data_offset);

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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::std_msgs::MultiArrayLayout> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::std_msgs::MultiArrayLayout> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "std_msgs/MultiArrayLayout";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "0fed2a11c13e11c5571b4e2a995a91a3";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
# The multiarray declares a generic multi-dimensional array of a\n\
# particular data type.  Dimensions are ordered from outer most\n\
# to inner most.\n\
\n\
MultiArrayDimension[] dim # Array of dimension properties\n\
uint32 data_offset        # padding elements at front of data\n\
\n\
# Accessors should ALWAYS be written in terms of dimension stride\n\
# and specified outer-most dimension first.\n\
# \n\
# multiarray(i,j,k) = data[data_offset + dim_stride[1]*i + dim_stride[2]*j + k]\n\
#\n\
# A standard, 3-channel 640x480 image with interleaved color channels\n\
# would be specified as:\n\
#\n\
# dim[0].label  = \"height\"\n\
# dim[0].size   = 480\n\
# dim[0].stride = 3*640*480 = 921600  (note dim[0] stride is just size of image)\n\
# dim[1].label  = \"width\"\n\
# dim[1].size   = 640\n\
# dim[1].stride = 3*640 = 1920\n\
# dim[2].label  = \"channel\"\n\
# dim[2].size   = 3\n\
# dim[2].stride = 3\n\
#\n\
# multiarray(i,j,k) refers to the ith row, jth column, and kth channel.\n\
\n\
================================================================================\n\
MSG: std_msgs/MultiArrayDimension\n\
string label   # label of given dimension\n\
uint32 size    # size of given dimension (in type units)\n\
uint32 stride  # stride of given dimension\n\
";

    yarp::os::Type getType() const override
    {
        yarp::os::Type typ = yarp::os::Type::byName(typeName, typeName);
        typ.addProperty("md5sum", yarp::os::Value(typeChecksum));
        typ.addProperty("message_definition", yarp::os::Value(typeText));
        return typ;
    }
};

} // namespace std_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_std_msgs_MultiArrayLayout_h

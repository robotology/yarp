/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is an automatically generated file.

// Generated from the following "nav_msgs/GridCells" msg definition:
//   #an array of cells in a 2D grid
//   Header header
//   float32 cell_width
//   float32 cell_height
//   geometry_msgs/Point[] cells
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARP_ROSMSG_nav_msgs_GridCells_h
#define YARP_ROSMSG_nav_msgs_GridCells_h

#include <yarp/os/Wire.h>
#include <yarp/os/Type.h>
#include <yarp/os/idl/WireTypes.h>
#include <string>
#include <vector>
#include <yarp/rosmsg/std_msgs/Header.h>
#include <yarp/rosmsg/geometry_msgs/Point.h>

namespace yarp {
namespace rosmsg {
namespace nav_msgs {

class GridCells : public yarp::os::idl::WirePortable
{
public:
    yarp::rosmsg::std_msgs::Header header;
    yarp::conf::float32_t cell_width;
    yarp::conf::float32_t cell_height;
    std::vector<yarp::rosmsg::geometry_msgs::Point> cells;

    GridCells() :
            header(),
            cell_width(0.0f),
            cell_height(0.0f),
            cells()
    {
    }

    void clear()
    {
        // *** header ***
        header.clear();

        // *** cell_width ***
        cell_width = 0.0f;

        // *** cell_height ***
        cell_height = 0.0f;

        // *** cells ***
        cells.clear();
    }

    bool readBare(yarp::os::ConnectionReader& connection) override
    {
        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** cell_width ***
        cell_width = connection.expectFloat32();

        // *** cell_height ***
        cell_height = connection.expectFloat32();

        // *** cells ***
        int len = connection.expectInt32();
        cells.resize(len);
        for (int i=0; i<len; i++) {
            if (!cells[i].read(connection)) {
                return false;
            }
        }

        return !connection.isError();
    }

    bool readBottle(yarp::os::ConnectionReader& connection) override
    {
        connection.convertTextMode();
        yarp::os::idl::WireReader reader(connection);
        if (!reader.readListHeader(4)) {
            return false;
        }

        // *** header ***
        if (!header.read(connection)) {
            return false;
        }

        // *** cell_width ***
        cell_width = reader.expectFloat32();

        // *** cell_height ***
        cell_height = reader.expectFloat32();

        // *** cells ***
        if (connection.expectInt32() != BOTTLE_TAG_LIST) {
            return false;
        }
        int len = connection.expectInt32();
        cells.resize(len);
        for (int i=0; i<len; i++) {
            if (!cells[i].read(connection)) {
                return false;
            }
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

        // *** cell_width ***
        connection.appendFloat32(cell_width);

        // *** cell_height ***
        connection.appendFloat32(cell_height);

        // *** cells ***
        connection.appendInt32(cells.size());
        for (size_t i=0; i<cells.size(); i++) {
            if (!cells[i].write(connection)) {
                return false;
            }
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(4);

        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** cell_width ***
        connection.appendInt32(BOTTLE_TAG_FLOAT32);
        connection.appendFloat32(cell_width);

        // *** cell_height ***
        connection.appendInt32(BOTTLE_TAG_FLOAT32);
        connection.appendFloat32(cell_height);

        // *** cells ***
        connection.appendInt32(BOTTLE_TAG_LIST);
        connection.appendInt32(cells.size());
        for (size_t i=0; i<cells.size(); i++) {
            if (!cells[i].write(connection)) {
                return false;
            }
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::nav_msgs::GridCells> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::nav_msgs::GridCells> bottleStyle;

    // The name for this message, ROS will need this
    static constexpr const char* typeName = "nav_msgs/GridCells";

    // The checksum for this message, ROS will need this
    static constexpr const char* typeChecksum = "b9e4f5df6d28e272ebde00a3994830f5";

    // The source text for this message, ROS will need this
    static constexpr const char* typeText = "\
#an array of cells in a 2D grid\n\
Header header\n\
float32 cell_width\n\
float32 cell_height\n\
geometry_msgs/Point[] cells\n\
\n\
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
MSG: geometry_msgs/Point\n\
# This contains the position of a point in free space\n\
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

} // namespace nav_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_nav_msgs_GridCells_h

/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
    yarp::os::NetFloat32 cell_width;
    yarp::os::NetFloat32 cell_height;
    std::vector<yarp::rosmsg::geometry_msgs::Point> cells;

    GridCells() :
            header(),
            cell_width(0.0),
            cell_height(0.0),
            cells()
    {
    }

    void clear()
    {
        // *** header ***
        header.clear();

        // *** cell_width ***
        cell_width = 0.0;

        // *** cell_height ***
        cell_height = 0.0;

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
        if (!connection.expectBlock((char*)&cell_width, 4)) {
            return false;
        }

        // *** cell_height ***
        if (!connection.expectBlock((char*)&cell_height, 4)) {
            return false;
        }

        // *** cells ***
        int len = connection.expectInt();
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
        cell_width = reader.expectDouble();

        // *** cell_height ***
        cell_height = reader.expectDouble();

        // *** cells ***
        if (connection.expectInt() != BOTTLE_TAG_LIST) {
            return false;
        }
        int len = connection.expectInt();
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

    bool writeBare(yarp::os::ConnectionWriter& connection) override
    {
        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** cell_width ***
        connection.appendBlock((char*)&cell_width, 4);

        // *** cell_height ***
        connection.appendBlock((char*)&cell_height, 4);

        // *** cells ***
        connection.appendInt(cells.size());
        for (size_t i=0; i<cells.size(); i++) {
            if (!cells[i].write(connection)) {
                return false;
            }
        }

        return !connection.isError();
    }

    bool writeBottle(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(4);

        // *** header ***
        if (!header.write(connection)) {
            return false;
        }

        // *** cell_width ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)cell_width);

        // *** cell_height ***
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble((double)cell_height);

        // *** cells ***
        connection.appendInt(BOTTLE_TAG_LIST);
        connection.appendInt(cells.size());
        for (size_t i=0; i<cells.size(); i++) {
            if (!cells[i].write(connection)) {
                return false;
            }
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
    typedef yarp::os::idl::BareStyle<yarp::rosmsg::nav_msgs::GridCells> rosStyle;
    typedef yarp::os::idl::BottleStyle<yarp::rosmsg::nav_msgs::GridCells> bottleStyle;

    // Give source text for class, ROS will need this
    static yarp::os::ConstString typeText()
    {
        return yarp::os::ConstString("\
#an array of cells in a 2D grid\n\
Header header\n\
float32 cell_width\n\
float32 cell_height\n\
geometry_msgs/Point[] cells\n\
") + yarp::os::ConstString("\n\
================================================================================\n\
MSG: std_msgs/Header\n\
") + yarp::rosmsg::std_msgs::Header::typeText() + yarp::os::ConstString("\n\
================================================================================\n\
MSG: geometry_msgs/Point\n\
") + yarp::rosmsg::geometry_msgs::Point::typeText();
    }

    yarp::os::ConstString getTypeText() const
    {
        return yarp::rosmsg::nav_msgs::GridCells::typeText();
    }

    // Name the class, ROS will need this
    yarp::os::Type getType() override
    {
        yarp::os::Type typ = yarp::os::Type::byName("nav_msgs/GridCells", "nav_msgs/GridCells");
        typ.addProperty("md5sum", yarp::os::Value("303d4db2c18abdde75d4dfdd171d1d79"));
        typ.addProperty("message_definition", yarp::os::Value(getTypeText()));
        return typ;
    }
};

} // namespace nav_msgs
} // namespace rosmsg
} // namespace yarp

#endif // YARP_ROSMSG_nav_msgs_GridCells_h

/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/dev/MapGrid2DInfo.h>

using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace yarp::sig;
using namespace yarp::os;
using namespace yarp::math;
using namespace std;

MapGrid2DOrigin::MapGrid2DOrigin()
{
    x = 0;
    y = 0;
    theta = 0;
}

MapGrid2DOrigin::MapGrid2DOrigin(double x_init, double y_init, double t_init)
{
    x = x_init;
    y = y_init;
    theta = t_init;
}

MapGrid2DInfo::MapGrid2DInfo()
{
    m_map_name = "";
    m_resolution = 0;
    m_origin = MapGrid2DOrigin(0, 0, 0);
    m_width = 0;
    m_height = 0;
}

XYWorld MapGrid2DInfo::cell2World(XYCell cell) const
{
    //convert a cell (from the upper-left corner) to the map reference frame (located in m_origin, measured in meters)
    //beware: the location of m_origin is referred to the lower-left corner (ROS convention)
    XYWorld v;
    v.x = double(cell.x) * this->m_resolution;
    v.y = double(cell.y) * this->m_resolution;
    v.x = +v.x + m_origin.x + 0 * this->m_resolution;
    v.y = -v.y + m_origin.y + (m_height - 1) * this->m_resolution;
    return v;
}

XYCell MapGrid2DInfo::world2Cell(XYWorld world) const
{
    //convert a world location (wrt the map reference frame located in m_origin, measured in meters), to a cell from the upper-left corner.
    //beware: the location of m_origin is referred to the lower-left corner (ROS convention)
    XYCell c;
    int x = int((+world.x - this->m_origin.x) / this->m_resolution) + 0;
    int y = int((-world.y + this->m_origin.y) / this->m_resolution) + m_height - 1;
    c.x = (x < 0) ? 0 : x;
    c.y = (y < 0) ? 0 : y;
    c.x = (c.x >= m_width) ? m_width-1 : c.x;
    c.y = (c.y >= m_height) ? m_height-1 : c.y;
    return c;
}

XYCell MapGrid2DInfo::world2Cell_unsafeFast(XYWorld world) const
{
    //convert a world location (wrt the map reference frame located in m_origin, measured in meters), to a cell from the upper-left corner.
    //beware: the location of m_origin is referred to the lower-left corner (ROS convention)
    XYCell c;
    c.x = int((+world.x - this->m_origin.x) / this->m_resolution) + 0;
    c.y = int((-world.y + this->m_origin.y) / this->m_resolution) + m_height - 1;
    return c;
}

bool MapGrid2DInfo::isInsideMap(XYWorld world) const
{
    XYCell cell = world2Cell_unsafeFast(world);
    return isInsideMap(cell);
}

bool MapGrid2DInfo::isInsideMap(XYCell cell) const
{
    //if (cell.x < 0) return false;
    //if (cell.y < 0) return false;
    if (cell.x >= m_width)
        return false;
    if (cell.y >= m_height)
        return false;
    return true;
}

Map2DLocation MapGrid2DInfo::toLocation(XYCell cell) const
{
    XYWorld wrld = cell2World(cell);
    return Map2DLocation(this->m_map_name, wrld);
}

Map2DLocation MapGrid2DInfo::toLocation(XYWorld wrld) const
{
    return Map2DLocation(this->m_map_name, wrld);
}

XYCell MapGrid2DInfo::toXYCell(yarp::dev::Nav2D::Map2DLocation loc) const
{
    XYWorld wrld(loc.x, loc.y);
    XYCell cell = world2Cell(wrld);
    return cell;
}

XYWorld MapGrid2DInfo::toXYWorld(yarp::dev::Nav2D::Map2DLocation loc) const
{
    return XYWorld(loc.x, loc.y);
}
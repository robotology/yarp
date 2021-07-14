/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define _USE_MATH_DEFINES

#include <cmath>
#include <yarp/dev/MapGrid2DInfo.h>

using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace yarp::sig;
using namespace yarp::os;
using namespace yarp::math;
using namespace std;

#ifndef DEG2RAD
#define DEG2RAD M_PI/180.0
#endif

MapGrid2DOrigin::MapGrid2DOrigin()
{
    x = 0;
    y = 0;
    theta = 0;
}

void MapGrid2DOrigin::setOrigin(double x_init, double y_init, double t_init)
{
    x = x_init;
    y = y_init;
    theta = fmod(t_init, 360.0);
    sa = sin (theta * DEG2RAD);
    ca = cos (theta * DEG2RAD);
}

MapGrid2DOrigin::MapGrid2DOrigin(double x_init, double y_init, double t_init)
{
    setOrigin(x_init,  y_init,  t_init);
}

bool MapGrid2DOrigin::operator != (const MapGrid2DOrigin& other) const
{
    if (x != other.x) {
        return true;
    }
    if (y != other.y) {
        return true;
    }
    if (theta != other.theta) {
        return true; //should I check for 360 wrap?
    }
    return false;
}

//--------------------------------------------------------------------

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
    XYWorld v1;
    XYWorld v2;
    v1.x = double(cell.x) * this->m_resolution;
    v1.y = double(cell.y) * this->m_resolution;
    v1.x = +v1.x + m_origin.get_x() + 0 * this->m_resolution;
    v1.y = -v1.y + m_origin.get_y() + (m_height - 1) * this->m_resolution;
    //note that in the following operation, we are using -get_sa()
    //this is an optimization, since sin(-a)=-sin(a) and cos(-a)=cos(a)
    //we need -a instead of a because of we are using the inverse tranformation respect to world2cell
    v2.x = v1.x *  m_origin.get_ca() - v1.y * -m_origin.get_sa();
    v2.y = v1.x * -m_origin.get_sa() + v1.y *  m_origin.get_ca();
    return v2;
}

XYCell MapGrid2DInfo::world2Cell(XYWorld world) const
{
    //convert a world location (wrt the map reference frame located in m_origin, measured in meters), to a cell from the upper-left corner.
    //beware: the location of m_origin is referred to the lower-left corner (ROS convention)
    XYWorld world2;
    world2.x = world.x * m_origin.get_ca() - world.y * m_origin.get_sa();
    world2.y = world.x * m_origin.get_sa() + world.y * m_origin.get_ca();
    int x = int((+world2.x - this->m_origin.get_x()) / this->m_resolution) + 0;
    int y = int((-world2.y + this->m_origin.get_y()) / this->m_resolution) + m_height - 1;
    XYCell c;
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
    XYWorld world2;
    world2.x = world.x * m_origin.get_ca() - world.y * m_origin.get_sa();
    world2.y = world.x * m_origin.get_sa() + world.y * m_origin.get_ca();
    XYCell c;
    c.x = int((+world2.x - this->m_origin.get_x()) / this->m_resolution) + 0;
    c.y = int((-world2.y + this->m_origin.get_y()) / this->m_resolution) + m_height - 1;
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
    if (cell.x >= m_width) {
        return false;
    }
    if (cell.y >= m_height) {
        return false;
    }
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

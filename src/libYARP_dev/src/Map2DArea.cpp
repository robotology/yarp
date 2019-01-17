/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/dev/api.h>
#include <yarp/dev/Map2DArea.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <sstream>
#include <string>
#include <vector>

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;
using namespace yarp::math;
using namespace std;

Map2DArea::Map2DArea(const std::string& map_name, const std::vector<yarp::math::Vec2D<double>> area_points)
{
    map_id = map_name;
    points = area_points;
}

Map2DArea::Map2DArea()
{
    map_id = "";
}

bool Map2DArea::read(yarp::os::ConnectionReader& connection)
{
    // auto-convert text mode interaction
    connection.convertTextMode();

    connection.expectInt32();
    connection.expectInt32();

    connection.expectInt32();
    this->map_id = connection.expectText();

    connection.expectInt32();
    size_t siz = connection.expectInt32();

    this->points.clear();
    for (size_t i = 0; i < siz; i++)
    {
        connection.expectInt32();
        double x = connection.expectFloat64();
        connection.expectInt32();
        double y = connection.expectFloat64();
        this->points.push_back(yarp::math::Vec2D<double>(x, y));
    }

    return !connection.isError();
}

bool Map2DArea::write(yarp::os::ConnectionWriter& connection) const
{
    connection.appendInt32(BOTTLE_TAG_LIST);
    connection.appendInt32(2);

    connection.appendInt32(BOTTLE_TAG_STRING);
    connection.appendRawString(map_id);
    
    connection.appendInt32(BOTTLE_TAG_LIST);
    int siz = this->points.size();
    connection.appendInt32(siz);

    for (size_t i = 0; i < siz; i++)
    {
        connection.appendInt32(BOTTLE_TAG_FLOAT64);
        connection.appendInt32(this->points[i].x);
        connection.appendInt32(BOTTLE_TAG_FLOAT64);
        connection.appendFloat64(this->points[i].y);
    }

    connection.convertTextMode();
    return !connection.isError();
}

std::string Map2DArea::toString() const
{
    std::ostringstream stringStream;
    stringStream.precision(-1);
    stringStream.width(-1);
    stringStream << std::string("map_id:") << map_id;
    for (size_t i = 0; i<points.size(); i++)
    {
        stringStream << "point " << i << "(" << points[i].x << points[i].y << ")";
    }
    return stringStream.str();
}

bool Map2DArea::check_location_inside_area(yarp::dev::Map2DLocation loc)
{
    if (loc.map_id != this->map_id) return false;
    if (points.size() < 3) return false;
    if (pnpoly(points, loc.x, loc.y) > 0) return true;
    return false;
}
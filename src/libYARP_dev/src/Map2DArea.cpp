/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
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
#include <cassert>
#include <yarp/os/LogStream.h>
#include <random>
#include <algorithm>

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;
using namespace yarp::math;
using namespace std;

int pnpoly(std::vector<yarp::math::Vec2D<double>> points, double testx, double testy)
{
    size_t i, j;
    int c = 0;
    for (i = 0, j = points.size() - 1; i < points.size(); j = i++)
    {
        if (((points[i].y>testy) != (points[j].y>testy)) &&
            (testx < (points[j].x - points[i].x) * (testy - points[i].y) / (points[j].y - points[i].y) + points[i].x))
        {
            c = !c;
        }
    }
    return c;
}

Map2DArea::Map2DArea(const std::string& map_name, const std::vector<yarp::math::Vec2D<double>> area_points)
{
    map_id = map_name;
    points = area_points;
}

Map2DArea::Map2DArea(const std::string& map_name, const std::vector<yarp::dev::Map2DLocation> area_points)
{
    map_id = map_name;
    for (auto it = area_points.begin(); it != area_points.end(); it++)
    {
#if 0
        yAssert(it->map_id == map_name);
#else
        if (it->map_id != map_name)
        {
            map_id = "";
            points.clear();
            yError() << "all area_points must belong to the same map:" << map_name;
            return;
        }
#endif
        points.push_back(yarp::math::Vec2D<double>(it->x, it->y));
    }
}

Map2DArea::Map2DArea()
{
    map_id = "";
}

bool Map2DArea::read(yarp::os::ConnectionReader& connection)
{
    // auto-convert text mode interaction
    connection.convertTextMode();
    int32_t dummy;

    dummy = connection.expectInt32();
    assert(dummy == BOTTLE_TAG_LIST);
    dummy = connection.expectInt32();

    dummy = connection.expectInt32();
    assert(dummy == BOTTLE_TAG_STRING);
    int string_size = connection.expectInt32();
    this->map_id.resize(string_size);
    connection.expectBlock(const_cast<char*>(this->map_id.data()), string_size);

    dummy = connection.expectInt32();
    assert(dummy == BOTTLE_TAG_INT32);
    size_t siz = connection.expectInt32();

    this->points.clear();
    for (size_t i = 0; i < siz; i++)
    {
        dummy = connection.expectInt32();
        assert(dummy == BOTTLE_TAG_FLOAT64);
        double x = connection.expectFloat64();
        dummy = connection.expectInt32();
        assert(dummy == BOTTLE_TAG_FLOAT64);
        double y = connection.expectFloat64();
        this->points.push_back(yarp::math::Vec2D<double>(x, y));
    }

    return !connection.isError();
}

bool Map2DArea::write(yarp::os::ConnectionWriter& connection) const
{
    size_t siz = this->points.size();

    connection.appendInt32(BOTTLE_TAG_LIST);
    connection.appendInt32(2+siz*2);

    connection.appendInt32(BOTTLE_TAG_STRING);
    connection.appendString(map_id);

    connection.appendInt32(BOTTLE_TAG_INT32);
    connection.appendInt32(siz);

    for (size_t i = 0; i < siz; i++)
    {
        connection.appendInt32(BOTTLE_TAG_FLOAT64);
        connection.appendFloat64(this->points[i].x);
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
    stringStream << std::string("map_id:") << map_id << " ";
    for (size_t i = 0; i<points.size(); i++)
    {
        stringStream << " point " << i << "(" << points[i].x << "," << points[i].y << ")";
    }
    return stringStream.str();
}

bool Map2DArea::checkLocationInsideArea(yarp::dev::Map2DLocation loc)
{
    if (loc.map_id != this->map_id) return false;
    if (points.size() < 3) return false;
    if (pnpoly(points, loc.x, loc.y) > 0) return true;
    return false;
}

bool Map2DArea::operator!=(const Map2DArea& r) const
{
    if (
        map_id != r.map_id ||
        points != r.points
        )
    {
        return true;
    }
    return false;
}

bool Map2DArea::operator==(const Map2DArea& r) const
{
    if (
        map_id == r.map_id &&
        points == r.points
        )
    {
        return true;
    }
    return false;
}

bool Map2DArea::isValid() const
{
    if (points.size() < 3) return false;
    if (map_id == "") return false;
    return true;
}

bool  Map2DArea::findAreaBounds(yarp::dev::Map2DLocation& lt, yarp::dev::Map2DLocation& rb)
{
    lt.map_id = rb.map_id = this->map_id;
    lt.x = lt.y = std::numeric_limits<double>::max();
    rb.x = rb.y = std::numeric_limits<double>::min();
    if (isValid() == false) return false;
    for (auto it = points.begin(); it != points.end(); it++)
    {
        if (it->x > rb.x) { rb.x = it->x; }
        if (it->y > rb.y) { rb.y = it->y; }
        if (it->x < lt.x) { lt.x = it->x; }
        if (it->y < lt.y) { lt.y = it->y; }
    }
    return true;
}

bool  Map2DArea::getRandomLocation(yarp::dev::Map2DLocation& loc)
{
    yarp::dev::Map2DLocation lt;
    yarp::dev::Map2DLocation rb;
    if (findAreaBounds(lt, rb) == false)
        return false;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis_x(lt.x, rb.x);
    std::uniform_real_distribution<double> dis_y(lt.y, rb.y);

    size_t count_trials = 0;
    do
    {
        double rnd_x = dis_x(gen);
        double rnd_y = dis_y(gen);

        loc.map_id = this->map_id;
        loc.x = rnd_x;
        loc.y = rnd_y;
        loc.theta = 0;
        count_trials++;
        if (this->checkLocationInsideArea(loc)) break;
    } while (count_trials < 20);

    if (count_trials >= 20)
    {
        yError() << "Problem found in Map2DArea::getRandomLocation()";
        return false;
    }

    return true;
}

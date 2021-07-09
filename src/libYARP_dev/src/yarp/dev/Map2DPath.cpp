/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <yarp/dev/Map2DPath.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <cassert>
#include <functional>
#include <math.h>

using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace yarp::sig;
using namespace yarp::os;
using namespace yarp::math;
using namespace std;

bool Map2DPath::operator!=(const Map2DPath& r) const
{
    for (size_t i = 0; i < waypoints.size(); i++)
        if (this->waypoints[i].map_id == r.waypoints[i].map_id ||
            this->waypoints[i].x == r.waypoints[i].x ||
            this->waypoints[i].y == r.waypoints[i].y ||
            this->waypoints[i].theta == r.waypoints[i].theta) return false;
    return true;
}

bool Map2DPath::operator==(const Map2DPath& r) const
{
    for (size_t i = 0; i < waypoints.size(); i++)
        if (this->waypoints[i].map_id != r.waypoints[i].map_id ||
            this->waypoints[i].x != r.waypoints[i].x ||
            this->waypoints[i].y != r.waypoints[i].y ||
            this->waypoints[i].theta != r.waypoints[i].theta) return false;
    return true;
}

std::string Map2DPath::toString() const
{
    std::ostringstream stringStream;
    stringStream.precision(2);
    //stringStream.width(0);
    for (size_t i = 0; i < waypoints.size(); i++)
    {
        stringStream << " waypoint " << i << "(" << waypoints[i].map_id << " " << waypoints[i].x << "," << waypoints[i].y << "," << waypoints[i].theta << ")";
    }
    return stringStream.str();
}

void Map2DPath::clear()
{
    this->waypoints.clear();
}

Map2DPath::Map2DPath(const std::vector<Map2DLocation> map_waypoints)
{
    for (const auto& map_waypoint : map_waypoints)
    {
        waypoints.push_back(map_waypoint);
    }
}

Map2DPath::Map2DPath()
{
}

Map2DLocation &Map2DPath::operator[](size_t index)
{
    return waypoints[index];
}

size_t Map2DPath::size() const
{
    return this->waypoints.size();
}

double Map2DPath::getLength() const
{
    double ll = 0;
    for (auto it = waypoints.begin(); it != waypoints.end(); it++)
    {
        ll += sqrt(pow(it->x,2)+ pow(it->y,2));
    }
    return ll;
}

bool Map2DPath::isOnSingleMap() const
{
    if (waypoints.size() == 0) return true;
    string mapname = waypoints[0].map_id;
    for (auto it = waypoints.begin(); it != waypoints.end(); it++)
    {
        if (it->map_id != mapname) return false;
    }
    return true;
}

Map2DPath::iterator Map2DPath::begin() noexcept
{
    return waypoints.begin();
}


Map2DPath::iterator Map2DPath::end() noexcept
{
    return waypoints.end();
}

Map2DPath::const_iterator Map2DPath::cbegin() const noexcept
{
    return waypoints.cbegin();
}

Map2DPath::const_iterator Map2DPath::cend() const noexcept
{
    return waypoints.cend();
}

void Map2DPath::push_back(yarp::dev::Nav2D::Map2DLocation loc)
{
    waypoints.push_back(loc);
}

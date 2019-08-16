/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */


#include <yarp/dev/Map2DPath.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <cassert>
#include <functional>

using namespace yarp::dev;
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
    stringStream.precision(-1);
    stringStream.width(-1);
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

Map2DPath::Map2DPath(const std::vector<yarp::dev::Map2DLocation> map_waypoints)
{
    for (auto it = map_waypoints.begin(); it != map_waypoints.end(); it++)
    {
        waypoints.push_back(*it);
    }
}

Map2DPath::Map2DPath()
{
}

yarp::dev::Map2DLocation &Map2DPath::operator[](size_t index)
{
    if (index >= this->waypoints.size())
    {
        yError() << "Map2DPath: Array index out of bound!";
        yFatal();// return yarp::dev::Map2DLocation();
    }
    Map2DLocationData a;
    auto b = reinterpret_cast<yarp::dev::Map2DLocation&> (a);
    
    yarp::dev::Map2DLocation ret_val;

  //  yarp::dev::Map2DLocation& ret_val = dynamic_cast<yarp::dev::Map2DLocation&> (waypoints[index]);

    return ret_val;
}

size_t Map2DPath::size() const
{
    return this->waypoints.size();
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
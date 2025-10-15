/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/Map2DObject.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <math.h>

using namespace yarp::dev::Nav2D;

YARP_LOG_COMPONENT(MAP2DOBJECT, "yarp.Map2DObject")

bool Map2DObject::is_near_to(const Map2DObject& other_loc, double linear_tolerance) const
{
    if (linear_tolerance < 0) {
        return false;
    }

    yCAssert(MAP2DOBJECT, linear_tolerance >= 0);

    if (this->map_id != other_loc.map_id)
    {
        return false;
    }
    if (sqrt(pow((this->x - other_loc.x), 2) +
             pow((this->y - other_loc.y), 2) +
             pow((this->z - other_loc.z), 2)) > linear_tolerance)
    {
        return false;
    }

    return true;
}

bool Map2DObject::is_near_to(const Map2DLocation& other_loc, double linear_tolerance) const
{
    if (linear_tolerance < 0) {
        return false;
    }

    yCAssert(MAP2DOBJECT, linear_tolerance >= 0);

    if (this->map_id != other_loc.map_id)
    {
        return false;
    }
    if (sqrt(pow((this->x - other_loc.x), 2) +
             pow((this->y - other_loc.y), 2)) > linear_tolerance)
    {
        return false;
    }

    return true;
}

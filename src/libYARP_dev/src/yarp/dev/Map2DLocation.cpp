/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/Map2DLocation.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <math.h>

using namespace yarp::dev::Nav2D;

YARP_LOG_COMPONENT(MAP2DLOCATION, "yarp.Map2DLocation")

bool Map2DLocation::is_near_to(const Map2DLocation& other_loc, double linear_tolerance, double angular_tolerance) const
{
    if (linear_tolerance < 0) {
        return false;
    }
    if (angular_tolerance < 0) {
        return false;
    }
    yCAssert(MAP2DLOCATION, linear_tolerance >= 0);
    yCAssert(MAP2DLOCATION, angular_tolerance >= 0);

    if (this->map_id != other_loc.map_id)
    {
        return false;
    }
    if (sqrt(pow((this->x - other_loc.x), 2) + pow((this->y - other_loc.y), 2)) > linear_tolerance)
    {
        return false;
    }

    if (angular_tolerance != std::numeric_limits<double>::infinity())
    {
        //In the following blocks, I'm giving two possible solution to the problem of
        //determining if the difference of two angles is below a certain threshold.
        //The problem is tricky, because it must take in account the critical points 0,180,360,-180, -360 etc.
        //Both the formulas lead to the same result, however I'm not sure I they have the same performances.
        //Please do not remove the unused block, since it may be a useful reference for the future.
#if 1
        //check in the range 0-360
        double diff = other_loc.theta - this->theta + 180.0;
        diff = fmod(diff, 360.0) - 180.0;
        diff = (diff < -180.0) ? (diff + 360.0) : (diff);
        if (fabs(diff) > angular_tolerance)
#else
        //check in the range 0-180
        double angle1 = normalize_angle(this->theta);
        double angle2 = normalize_angle(other_loc.theta);
        double diff = angle1 - angle2;
        diff += (diff > 180) ? -360 : (diff < -180) ? 360 : 0;
        if (fabs(diff) > angular_tolerance)
#endif
        {
            return false;
        }
    }
    return true;
}

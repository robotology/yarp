/*
 * Copyright (C) 2017 iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_DEV_MAP2DLOCATION_H
#define YARP_DEV_MAP2DLOCATION_H

#include <yarp/os/Portable.h>
#include <yarp/math/Vec2D.h>
#include <yarp/dev/api.h>
#include <sstream>
#include <string>

/**
* \file Map2DLocation.h contains the definition of a Map2DLocation type
*/
namespace yarp
{
    namespace dev
    {
        struct Map2DLocation
        {
            Map2DLocation(const std::string& map_name, const double& inX, const double& inY, const double& inT)
            {
                map_id = map_name;
                x = inX;
                y = inY;
                theta = inT;
            }

            Map2DLocation()
            {
                map_id = "";
                x = 0;
                y = 0;
                theta = 0;
            }

            std::string toString()
            {
                std::ostringstream stringStream;
                stringStream.precision(-1);
                stringStream.width(-1);
                stringStream << std::string("map_id:") << map_id << std::string(" x:") << x << std::string(" y:") << y << std::string(" theta:") << theta;
                return stringStream.str();
            }

            inline bool operator!=(const Map2DLocation& r) const
            {
                if (
                    map_id != r.map_id ||
                    x != r.x ||
                    y != r.y ||
                    theta != r.theta
                    )
                {
                    return true;
                }
                return false;
            }

            inline bool operator==(const Map2DLocation& r) const
            {
                if (
                    map_id == r.map_id &&
                    x == r.x      &&
                    y == r.y      &&
                    theta == r.theta
                    )
                {
                    return true;
                }
                return false;
            }

            std::string map_id;
            double x;
            double y;
            double theta;
        };
    }
}

#endif // YARP_DEV_MAP2DLOCATION_H

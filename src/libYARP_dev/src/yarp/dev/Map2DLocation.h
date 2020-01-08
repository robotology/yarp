/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_MAP2DLOCATION_H
#define YARP_DEV_MAP2DLOCATION_H

#include <sstream>
#include <string>

#include <yarp/dev/api.h>

/**
* \file Map2DLocation.h contains the definition of a Map2DLocation type
*/
namespace yarp
{
    namespace dev
    {
        struct YARP_dev_API Map2DLocation
        {
            /**
            * Constructor
            * @param map_name: the name of the map the location refers to.
            * @param inX: location coordinates w.r.t. map reference frame (expressed in meters)
            * @param inY: location coordinates w.r.t. map reference frame (expressed in meters)
            * @param inT: location orientation w.r.t. map reference frame (expressed in degrees)
            */
            Map2DLocation(const std::string& map_name, const double& inX, const double& inY, const double& inT)
            {
                map_id = map_name;
                x = inX;
                y = inY;
                theta = inT;
            }

            /**
            * Default constructor: the map name is empty, coordinates are set to zero.
            */
            Map2DLocation()
            {
                map_id = "";
                x = 0;
                y = 0;
                theta = 0;
            }

            /**
            * Returns text representation of the location.
            * @return a human readable string containing the location infos.
            */
            std::string toString() const
            {
                std::ostringstream stringStream;
                stringStream.precision(-1);
                stringStream.width(-1);
                stringStream << std::string("map_id:") << map_id << std::string(" x:") << x << std::string(" y:") << y << std::string(" theta:") << theta;
                return stringStream.str();
            }

            /**
            * Compares two Map2DLocations
            * @return true if the two locations are different.
            */
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

            /**
            * Compares two Map2DLocations
            * @return true if the two locations are identical.
            */
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

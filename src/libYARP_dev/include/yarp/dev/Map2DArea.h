/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_MAP2DAREA_H
#define YARP_DEV_MAP2DAREA_H

#include <yarp/os/Portable.h>
#include <yarp/math/Vec2D.h>
#include <yarp/dev/api.h>
#include <yarp/dev/Map2Dlocation.h>
#include <sstream>
#include <string>
#include <vector>

/**
* \file Map2DArea.h contains the definition of a Map2DArea type
*/
namespace yarp
{
    namespace dev
    {
        class YARP_dev_API Map2DArea : public yarp::os::Portable
        {
            public:
            /**
            * Constructor
            * @param map_name: the name of the map the location refers to.
            * @param inX: location coordinates w.r.t. map reference frame (expressed in meters)
            * @param inY: location coordinates w.r.t. map reference frame (expressed in meters)
            * @param inT: location orientation w.r.t. map reference frame (expressed in degrees)
            */
            Map2DArea(const std::string& map_name, const std::vector<yarp::math::Vec2D<double>> area_points);

            /**
            * Default constructor: the map name is empty, coordinates are set to zero.
            */
            Map2DArea();

            /**
            * Returns text representation of the location.
            * @return a human readable string containing the location infos.
            */
            std::string toString() const;

            /**
            * Compares two Map2DAreas
            * @return true if the two areas are different.
            */
            inline bool operator!=(const Map2DArea& r) const
            {
                if (
                    map_id != r.map_id
                    )
                {
                    return true;
                }
                return false;
            }

            /**
            * Compares two Map2DArea
            * @return true if the two areas are identical.
            */
            inline bool operator==(const Map2DArea& r) const
            {
                if (
                    map_id == r.map_id
                   )
                {
                    return true;
                }
                return false;
            }

            bool check_location_inside_area(yarp::dev::Map2DLocation loc);

            private:
            int pnpoly(std::vector<yarp::math::Vec2D<double>> points, double testx, double testy)
            {
                int i, j, c = 0;
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

            public:
            std::string map_id;
            std::vector<yarp::math::Vec2D<double>> points;

            /*
            * Read vector from a connection.
            * return true iff a vector was read correctly
            */
            bool read(yarp::os::ConnectionReader& connection) override;

            /**
            * Write vector to a connection.
            * return true iff a vector was written correctly
            */
            bool write(yarp::os::ConnectionWriter& connection) const override;
        };
    }
}

#endif // YARP_DEV_MAP2DLOCATION_H

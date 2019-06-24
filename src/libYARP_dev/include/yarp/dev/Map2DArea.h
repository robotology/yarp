/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
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
#include <yarp/dev/Map2DLocation.h>
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
            * @param area_points: a set of vertexes defining the area. At least three points are required to define a valid area.
            */
            Map2DArea(const std::string& map_name, const std::vector<yarp::math::Vec2D<double>> area_points);

            /**
            * Constructor
            * @param map_name: the name of the map the location refers to.
            * @param area_points: a set of Map2DLocations defining the area. At least three points are required to define a valid area.
            */
            Map2DArea(const std::string& map_name, const std::vector<yarp::dev::Map2DLocation> area_points);

            /**
            * Default constructor: the map name is empty, coordinates are set to zero.
            */
            Map2DArea();

            /**
            * Returns text representation of the area.
            * @return a human readable string containing the area infos.
            */
            std::string toString() const;

            /**
            * Compares two Map2DAreas
            * @return true if the two areas are different.
            */
            bool operator!=(const Map2DArea& r) const;

            /**
            * Compares two Map2DArea
            * @return true if the two areas are identical.
            */
            bool operator==(const Map2DArea& r) const;

            /**
            * Checks if the Map2DArea is valid
            * return true if the Map2DArea is valid
            */
            bool isValid() const;

            /**
            * Check if a Map2DLocation is inside a Map2DArea
            * @return loc the Map2DLocation
            * @return true if Map2DLocation is inside the Map2DArea
            */
            bool checkLocationInsideArea(yarp::dev::Map2DLocation loc);

            /**
            * retrieves two Map2DLocations representing the bounding box of the Map2DArea
            * @lt the left-top vertex
            * @rb the right-bottom vertex
            * @return true if the two Locations were successfully generated (the Map2DArea must be valid)
            */
            bool findAreaBounds(yarp::dev::Map2DLocation& lt, yarp::dev::Map2DLocation& rb);

            /**
            * get a random Map2DLocation inside the Map2DArea
            * @loc the computed Map2DLocation
            * @return true if the Location was successfully generated (the Map2DArea must be valid)
            */
            bool getRandomLocation(yarp::dev::Map2DLocation& loc);

            public:
            std::string map_id;
            std::vector<yarp::math::Vec2D<double>> points;

            public:
            /*
            * Read a map2DArea from a connection.
            * return true iff a map2DArea was read correctly
            */
            bool read(yarp::os::ConnectionReader& connection) override;

            /**
            * Write a map2DArea to a connection.
            * return true iff a map2DArea was written correctly
            */
            bool write(yarp::os::ConnectionWriter& connection) const override;
        };
    }
}

#endif // YARP_DEV_MAP2DAREA_H

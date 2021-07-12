/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_MAP2DPATH_H
#define YARP_DEV_MAP2DPATH_H

#include <vector>
#include <yarp/dev/MapGrid2D.h>
#include <yarp/dev/Map2DLocation.h>
#include <yarp/dev/Map2DPathData.h>

 /**
 * \file Map2DPath.h contains the definition of a Map2DPath type
 */
namespace yarp
{
    namespace dev
    {
        namespace Nav2D
        {
            class YARP_dev_API Map2DPath : public yarp::dev::Map2DPathData
            {
            public:
                /**
                * Constructor
                * @param area_points: a set of Map2DLocations defining the path.
                */
                Map2DPath(const std::vector<yarp::dev::Nav2D::Map2DLocation> map_waypoints);

                /**
                * Default constructor: the map name is empty, coordinates are set to zero.
                */
                Map2DPath();

                /**
                * Returns text representation of the path.
                * @return a human readable string containing the path infos.
                */
                std::string toString() const;

                /**
                * Compares two Map2DPath
                * @return true if the two paths are different.
                */
                bool operator!=(const Map2DPath& r) const;

                /**
                * Compares two Map2DArea
                * @return true if the two areas are identical.
                */
                bool operator==(const Map2DPath& r) const;

                /**
                * Returns a waypoint in the path
                * @param index: the waypoint index
                * @return the waypoint
                */
                yarp::dev::Nav2D::Map2DLocation& operator[](size_t index);

                /**
                * Returns the size of the path
                * @return the size of the path
                */
                size_t size() const;

                /**
                * Returns the length of the path
                * @return the size of the path
                */
                double getLength() const;

                /**
                * Checks if all the waypoints of the path belong to the same map
                * return true if the test is successful
                */
                bool isOnSingleMap() const;

            public:
                bool read(yarp::os::idl::WireReader& reader) override { return Map2DPathData::read(reader); }
                bool write(const yarp::os::idl::WireWriter& writer) const override { return Map2DPathData::write(writer); }
                bool read(yarp::os::ConnectionReader& reader) override { return Map2DPathData::read(reader); }
                bool write(yarp::os::ConnectionWriter& writer) const override { return Map2DPathData::write(writer); }

            public:
                /**
                * Remove all elements from the path
                */
                void clear();

                using iterator = std::vector<Map2DLocation>::iterator;
                using const_iterator = std::vector<Map2DLocation>::const_iterator;

                /**
                 * Returns an iterator to the begin of the Path.
                 */
                iterator begin() noexcept;

                /**
                 * @brief Returns an iterator to the end of the Path
                 */
                iterator end() noexcept;

                /**
                 * Returns a const iterator to the begin of the Path.
                 */
                const_iterator cbegin() const noexcept;

                /**
                 * Returns a const iterator to the end of the Path.
                 */
                const_iterator cend() const noexcept;

                /**
                 * Inserts a new location into the path
                 * @loc   the location to be inserted
                */
                void push_back(yarp::dev::Nav2D::Map2DLocation loc);
            };
        }
    }
}

#endif

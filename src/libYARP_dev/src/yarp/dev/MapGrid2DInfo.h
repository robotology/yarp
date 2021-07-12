/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_MAPGRID2DINFO_H
#define YARP_DEV_MAPGRID2DINFO_H

#include <string>

#include <yarp/os/Portable.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/sig/Image.h>
#include <yarp/math/Vec2D.h>
#include <yarp/dev/api.h>
#include <yarp/dev/Map2DLocation.h>
#include <yarp/dev/NavTypes.h>

/**
* \file MapGrid2D.h contains the definition of a map type
*/
namespace yarp
{
    namespace dev
    {
        namespace Nav2D
        {
            class YARP_dev_API MapGrid2DOrigin
            {
            private:
                double sa;
                double ca;
            private:
                double x;     ///< in meters
                double y;     ///< in meters
                double theta; ///< in degrees
            public:
                MapGrid2DOrigin();
                MapGrid2DOrigin(double x_init, double y_init, double t_init);
                inline double get_x() const     { return x; }
                inline double get_y() const     { return y; }
                inline double get_theta() const { return theta; }
                void   setOrigin(double x_init, double y_init, double t_init);
                bool operator != (const MapGrid2DOrigin& other) const;
            public:
                inline double get_ca() const { return ca; }
                inline double get_sa() const { return sa; }
            };

            class YARP_dev_API MapGrid2DInfo
            {
            public:
                MapGrid2DInfo();

                YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) m_map_name;
                double          m_resolution;   ///< meters/pixel
                MapGrid2DOrigin m_origin;       ///< pose of the map frame w.r.t. the bottom left corner of the map image
                size_t          m_width;        ///< cells
                size_t          m_height;       ///< cells

                //------------------------------utility functions-------------------------------
                //convert a cell (from the upper-left corner) to the map reference frame (located in m_origin, measured in meters)
                XYWorld cell2World(XYCell cell) const;
                yarp::dev::Nav2D::Map2DLocation toLocation(XYCell cell) const;
                XYCell toXYCell(yarp::dev::Nav2D::Map2DLocation loc) const;

                //convert a world location (wrt the map reference frame located in m_origin, measured in meters), to a cell from the upper-left corner.
                XYCell world2Cell(XYWorld world) const;
                XYCell world2Cell_unsafeFast(XYWorld world) const;
                yarp::dev::Nav2D::Map2DLocation toLocation(XYWorld cell) const;
                XYWorld toXYWorld(yarp::dev::Nav2D::Map2DLocation loc) const;

                /** Checks if a cell is inside the map.
                * @param cell is the cell location, referred to the top - left corner of the map.
                * @return true if cell is inside the map, false otherwise.
                */
                bool   isInsideMap(XYCell cell) const;

                /**
                * Checks if a world coordinate is inside the map.
                * @param world is the world coordinate, expressed in meters, referred to the map origin reference frame.
                * @return true if cell is inside the map, false otherwise.
                */
                bool   isInsideMap(XYWorld world) const;
            };
        }
    }
}

#endif // YARP_DEV_MAPGRID2D_H

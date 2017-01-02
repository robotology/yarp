/*
 * Copyright (C) 2016 RobotCub Consortium
 * Authors: Marco Randazzo
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_DEV_MAPGRID2D_H
#define YARP_DEV_MAPGRID2D_H

#include <yarp/os/Portable.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/Vector.h>
#include <yarp/math/Vec2D.h>
#include <yarp/dev/api.h>

/**
* \file MapGrid2D.h contains the definition of a map type
*/
namespace yarp
{
    namespace dev
    {
        class  YARP_dev_API MapGrid2D : public yarp::os::Portable
        {
            public:
            typedef yarp::sig::PixelMono CellData;
            typedef yarp::math::Vec2D<int> XYCell;
            typedef yarp::math::Vec2D<double> XYWorld;
            enum map_flags
            {
                MAP_CELL_FREE=0,
                MAP_CELL_KEEP_OUT=1,
                MAP_CELL_TEMPORARY_OBSTACLE=2,
                MAP_CELL_ENLARGED_OBSTACLE=3,
                MAP_CELL_WALL = 4,
                MAP_CELL_UNKNOWN =5
            };
            private:
                //those two always have the same size
                yarp::sig::ImageOf<CellData> m_map_occupancy;
                yarp::sig::ImageOf<CellData> m_map_flags;

                double m_resolution;    //m/pixel
                double m_occupied_thresh;
                double m_free_thresh;
                size_t m_width;
                size_t m_height;
                std::string m_map_name;

                struct
                {
                    double x;     //in meters
                    double y;     //in meters
                    double theta; //radians
                } m_origin; //pose of the map frame w.r.t. the bottom left corner of the map image

                //std::vector<map_link> links_to_other_maps;

            private:
                void enlargeCell(XYCell cell);
                CellData PixelToCellData(const yarp::sig::PixelRgb& pixin) const;
                yarp::sig::PixelRgb CellDataToPixel(const CellData& pixin) const;

            public:
                MapGrid2D();
                virtual ~MapGrid2D();

                bool   isWall           (XYCell cell) const;
                bool   isFree           (XYCell cell) const;
                bool   isNotFree        (XYCell cell) const;
                bool   isKeepOut        (XYCell cell) const;
                bool   setMapFlag       (XYCell cell, map_flags flag);
                bool   setOccupancyData (XYCell cell, double  occupancy);
                bool   getOccupancyData (XYCell cell, double& occupancy) const;

                bool   setMapImage      (yarp::sig::ImageOf<yarp::sig::PixelRgb>& image);
                bool   getMapImage      (yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) const;
                bool   setOccupancyGrid (yarp::sig::ImageOf<yarp::sig::PixelMono>& image);
                bool   getOccupancyGrid (yarp::sig::ImageOf<yarp::sig::PixelMono>& image) const;

                bool   setOrigin        (double x, double y, double theta);
                void   getOrigin        (double& x, double& y, double& theta) const;
                bool   setResolution    (double resolution);
                void   getResolution    (double& resolution) const;
                bool   setSize_in_meters(double x, double y);
                bool   setSize_in_cells (size_t x, size_t y);
                void   getSize_in_meters(double& x, double& y) const;
                void   getSize_in_cells (size_t&x, size_t& y) const;
                size_t width            () const;
                size_t height           () const;
                bool   setMapName       (std::string map_name);
                std::string             getMapName() const;

                //utilities
                //convert a cell (from the upper-left corner) to the map reference frame (located in m_origin, measured in meters)
                XYWorld cell2World(XYCell cell) const;
                //convert a world location (wrt the map reference frame located in m_origin, measured in meters), to a cell from the upper-left corner.
                XYCell world2Cell(XYWorld world) const;
                bool   isInsideMap(XYCell cell) const;
                bool   isInsideMap(XYWorld world) const;
                bool   isIdenticalTo(const MapGrid2D& otherMap) const;
                bool   enlargeObstacles(unsigned int size);
                //file access
                bool   loadFromFile(std::string yaml_filename);
                bool   saveToFile(std::string yaml_filename) const;

                /*
                * Read vector from a connection.
                * return true iff a vector was read correctly
                */
                virtual bool read(yarp::os::ConnectionReader& connection);

                /**
                * Write vector to a connection.
                * return true iff a vector was written correctly
                */
                virtual bool write(yarp::os::ConnectionWriter& connection);
        };
    }
}

#endif // YARP_DEV_MAPGRID2D_H

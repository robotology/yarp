/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_MAPGRID2D_H
#define YARP_DEV_MAPGRID2D_H

#include <string>

#include <yarp/os/Portable.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/sig/Image.h>
#include <yarp/math/Vec2D.h>
#include <yarp/dev/api.h>
#include <yarp/dev/MapGrid2DInfo.h>

/**
* \file MapGrid2D.h contains the definition of a map type
*/
namespace yarp
{
    namespace dev
    {
        namespace Nav2D
        {
            class  YARP_dev_API MapGrid2D : public yarp::os::Portable,
                                            public yarp::dev::Nav2D::MapGrid2DInfo
            {
            public:
                typedef yarp::sig::PixelMono CellFlagData;
                typedef yarp::sig::PixelMono CellOccupancyData;
                //typedef yarp::math::Vec2D<int> XYCell;
                //typedef yarp::math::Vec2D<double> XYWorld;

                enum map_flags
                {
                    MAP_CELL_FREE = 0,
                    MAP_CELL_KEEP_OUT = 1,
                    MAP_CELL_TEMPORARY_OBSTACLE = 2,
                    MAP_CELL_ENLARGED_OBSTACLE = 3,
                    MAP_CELL_WALL = 4,
                    MAP_CELL_UNKNOWN = 5
                };

            private:
                //those two always have the same size
                yarp::sig::ImageOf<CellOccupancyData> m_map_occupancy;
                yarp::sig::ImageOf<CellFlagData> m_map_flags;

                double m_occupied_thresh;
                double m_free_thresh;

                //std::vector<map_link> links_to_other_maps;

            private:
                bool m_compressed_data_over_network;
            public:
                bool enable_map_compression_over_network (bool val);

            private:
                //performs an obstacles enlargement on the specified cell.
                void enlargeCell(XYCell cell);

                //conversion from pixel color to CellFlagData (yarp format) and viceversa
                CellFlagData PixelToCellFlagData(const yarp::sig::PixelRgb& pixin) const;
                yarp::sig::PixelRgb CellFlagDataToPixel(const CellFlagData& cellin) const;

                //conversion from pixel color to CellOccupancyData (occupancy grid, ros format) and viceversa
                CellOccupancyData PixelToCellOccupancyData(const yarp::sig::PixelMono& pixin) const;
                yarp::sig::PixelMono CellOccupancyDataToPixel(const CellOccupancyData& cellin) const;

                //internal methods to read a map from file, either in yarp or ROS format
                bool loadMapYarpOnly(std::string yarp_img_filename);
                bool loadMapROSOnly(std::string ros_yaml_filename);
                bool loadROSParams(std::string ros_yaml_filename, std::string& pgm_occ_filename, double& resolution, double& orig_x, double& orig_y, double& orig_t);
                bool loadMapYarpAndRos(std::string yarp_img_filename, std::string ros_yaml_filename);
                bool parseMapParameters(const yarp::os::Property& mapfile);

            public:
                MapGrid2D();
                virtual ~MapGrid2D();

                /**
                * Checks if a specific cell of the map contains a wall. A robot cannot pass through a cell occupied by a wall.
                * @param cell is the cell location, referred to the top-left corner of the map.
                * @return true if cell is valid cell inside the map, false otherwise.
                */
                bool   isWall(XYCell cell) const;

                /**
                * Checks if a specific cell of the map is free, i.e. the robot can freely pass through the cell
                * @param cell is the cell location, referred to the top-left corner of the map.
                * @return true if cell is valid cell inside the map, false otherwise.
                */
                bool   isFree(XYCell cell) const;

                /**
                * Checks if a specific cell of the map contains is not free. It may be occupied by a wall, an obstacle, a keep-out area etc.
                * @param cell is the cell location, referred to the top-left corner of the map.
                * @return true if cell is valid cell inside the map, false otherwise.
                */
                bool   isNotFree(XYCell cell) const;

                /**
                * Checks if a specific cell of the map is marked as keep-out.
                * User can set a cell as keep-out to prevent a robot to pass through it, even if no obstacles are present in the path.
                * @param cell is the cell location, referred to the top-left corner of the map.
                * @return true if cell is valid cell inside the map, false otherwise.
                */
                bool   isKeepOut(XYCell cell) const;

                /**
                * Get the flag of a specific cell of the map.
                * @param cell is the cell location, referred to the top-left corner of the map.
                * @return true if cell is valid cell inside the map, false otherwise.
                */
                bool   getMapFlag(XYCell cell, map_flags& flag) const;

                /**
                * Set the flag of a specific cell of the map.
                * @param cell is the cell location, referred to the top-left corner of the map.
                * @return true if cell is valid cell inside the map, false otherwise.
                */
                bool   setMapFlag(XYCell cell, map_flags flag);

                /**
                * Clear map temporary flags, such as: MAP_CELL_TEMPORARY_OBSTACLE, MAP_CELL_ENLARGED_OBSTACLE etc.
                */
                void   clearMapTemporaryFlags();

                /**
                * Set the occupancy data of a specific cell of the map.
                * @param occupancy represents the probability (0-100) of the cell of being occupied by a wall/obstacle etc.
                * @return true if cell is valid cell inside the map, false otherwise.
                */
                bool   setOccupancyData(XYCell cell, double  occupancy);

                /**
                * Retrieves the occupancy data of a specific cell of the map.
                * @param occupancy represents the probability (0-100) of the cell of being occupied by a wall/obstacle etc.
                * @return true if cell is valid cell inside the map, false otherwise.
                */
                bool   getOccupancyData(XYCell cell, double& occupancy) const;

                bool   setMapImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image);
                bool   getMapImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) const;
                bool   setOccupancyGrid(yarp::sig::ImageOf<yarp::sig::PixelMono>& image);
                bool   getOccupancyGrid(yarp::sig::ImageOf<yarp::sig::PixelMono>& image) const;

                /**
                * Sets the origin of the map reference frame (according to ROS convention)
                * @param x,y,theta is the pose of the origin, expressed in [m], [deg] and referred to the bottom-left corner of the map, pointing outwards.
                * @return true if cell is valid value inside the map, false otherwise.
                */
                bool   setOrigin(double x, double y, double theta);

                /**
                * Retrieves the origin of the map reference frame (according to ROS convention)
                * @param x,y,theta is the pose of the origin, expressed in [m], [deg] and referred to the bottom-left corner of the map, pointing outwards.
                */
                void   getOrigin(double& x, double& y, double& theta) const;

                /**
                * Sets the resolution of the map, i.e. the conversion factor which represents the metric size of a map cell.
                * @param resolution the map resolution, expressed in [m/cell]. e.g. resolution=0.05 means that each cell of the map represent 5cm of the real world.
                * @return true if resolution is valid (>=0), false otherwise.
                */
                bool   setResolution(double resolution);

                /**
                * Retrieves the resolution of the map, i.e. the conversion factor which represents the metric size of a map cell.
                * @param resolution the map resolution, expressed in [m/cell]. e.g. resolution=0.05 means that each cell of the map represent 5cm of the real world.
                */
                void   getResolution(double& resolution) const;

                /**
                * Sets the size of the map in meters, according to the current map resolution.
                * @param x,y is the map size in meters.
                * @return true if the operation was successful, false otherwise.
                */
                bool   setSize_in_meters(double x, double y);

                /**
                * Sets the size of the map in cells
                * @param x,y is the map size in cells.
                * @return true if the operation was successful, false otherwise.
                */
                bool   setSize_in_cells(size_t x, size_t y);

                /**
                * Returns the size of the map in meters, according to the current map resolution.
                * @param x,y is the map size in meters.
                */
                void   getSize_in_meters(double& x, double& y) const;

                /**
                * Returns the size of the map in cells.
                * @param x,y is the map size in cells.
                */
                void   getSize_in_cells(size_t&x, size_t& y) const;

                /**
                * Retrieves the map width, expressed in cells.
                * @return the map width.
                */
                size_t width() const;

                /**
                * Retrieves the map height, expressed in cells.
                * @return the map height.
                */
                size_t height() const;

                /**
                * Sets the map name.
                * @param map_name the map name.
                * @return true if map_name is a valid non-empty string, false otherwise.
                */
                bool   setMapName(std::string map_name);

                /**
                * Retrieves the map name.
                * @return the map_name.
                */
                std::string             getMapName() const;

                //------------------------------utility functions-------------------------------

                /**
                * Modifies the map, cropping pixels at the boundaries.
                * @param left, top, right, bottom: the corners of the map area to keep (expressed in pixel coordinates). If the value is negative, all unknown pixels are removed until a significative pixel is found.
                * @return true if the operation is performed successfully (the input parameters are valid), false otherwise.
                */
                bool   crop(int left, int top, int right, int bottom);

#if 0
                /**
                * Checks if a cell is inside the map.
                * @param cell is the cell location, referred to the top-left corner of the map.
                * @return true if cell is inside the map, false otherwise.
                */
                bool   isInsideMap(XYCell cell) const;

                /**
                * Checks if a world coordinate is inside the map.
                * @param world is the world coordinate, expressed in meters, referred to the map origin reference frame.
                * @return true if cell is inside the map, false otherwise.
                */
                bool   isInsideMap(XYWorld world) const;
#endif

                /**
                * Checks is two maps are identical.
                * @return true if all the internal data of the maps are identical, false otherwise.
                */
                bool   isIdenticalTo(const MapGrid2D& otherMap) const;

                /**
                * Performs the obstacle enlargement operation. It's useful to set size to a value equal or larger to the radius of the robot bounding box.
                * In this way a navigation algorithm can easily check obstacle collision by comparing the location of the center of the robot with cell value (free/occupied etc)
                * @param size the size of the enlargement, in meters. If size>0 the requested enlargement is performed. If the function is called multiple times, the enlargement sums up.
                If size <= 0 the enlargement stored in the map is cleaned up.
                * @return true always.
                */
                bool   enlargeObstacles(double size);

                //-------------------------------file access functions-------------------------------

                /**
                * Loads a yarp map file from disk. File must have .map extension.
                * param map_filename is the full path to the map file.
                * @return true if load was successful, false otherwise.
                */
                bool   loadFromFile(std::string map_filename);

                /**
                * Store a yarp map file to disk. File must have .map extension.
                * param map_filename is the full path to the map file.
                * @return true if load was successful, false otherwise.
                */
                bool   saveToFile(std::string map_filename) const;

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
}

#endif // YARP_DEV_MAPGRID2D_H

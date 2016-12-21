/*
 * Copyright (C) 2016 RobotCub Consortium
 * Authors: Marco Randazzo
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_SIG_MAPGRID2D_H
#define YARP_SIG_MAPGRID2D_H

#include <yarp/os/Portable.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/Vector.h>

/**
* \file MapGrid2D.h contains the definition of a map type
*/
namespace yarp {
    namespace sig
    {
        typedef yarp::sig::PixelMono MapGrid2DCell;
        class YARP_sig_API MapGrid2D : public yarp::os::Portable
        {
            private:
                //those two always have the same size
                yarp::sig::ImageOf<MapGrid2DCell> m_map_occupancy;
                yarp::sig::ImageOf<MapGrid2DCell>       m_map_flags;

                double m_resolution;    //m/pixel
                struct
                {
                    double x;     //in meters
                    double y;     //in meters
                    double theta; //radians
                } m_origin; //pose of the map frame w.r.t. the bottom left corner of the map image

                //std::vector<map_link> links_to_other_maps;

            private:
                void enlargeCell(size_t x, size_t y);

            public:
                MapGrid2D();
                virtual ~MapGrid2D();

                std::string m_map_name;
                bool   isObstacle (int x, int y);
                bool   isWall     (int x, int y);
                bool   isFree     (int x, int y);
                bool   isKeepOut  (int x, int y);
                bool   isIdenticalTo(const MapGrid2D& otherMap);
                size_t width() const;
                size_t height() const;
                bool   loadFromFile(std::string yaml_filename);
                bool   saveToFile(std::string yaml_filename);
                bool   getMapImage(yarp::sig::ImageOf<PixelRgb>& image);
                yarp::sig::Vector cell2World(yarp::sig::VectorOf<int> cell);
                yarp::sig::VectorOf<int> world2Cell(yarp::sig::Vector world);
                bool   enlargeObstacles(unsigned int size);

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

#endif // YARP_SIG_MAPGRID2D_H

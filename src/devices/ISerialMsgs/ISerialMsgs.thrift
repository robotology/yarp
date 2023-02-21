/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

struct yarp_dev_Nav2D_Map2DLocation{
} (
  yarp.name = "yarp::dev::Nav2D::Map2DLocation"
  yarp.includefile="yarp/dev/Map2DLocation.h"
)
struct yarp_dev_Nav2D_Map2DArea{
} (
  yarp.name = "yarp::dev::Nav2D::Map2DArea"
  yarp.includefile="yarp/dev/Map2DArea.h"
)
struct yarp_dev_Nav2D_Map2DPath{
} (
  yarp.name = "yarp::dev::Nav2D::Map2DPath"
  yarp.includefile="yarp/dev/Map2DPath.h"
)

struct yarp_dev_Nav2D_MapGrid2D{
} (
  yarp.name = "yarp::dev::Nav2D::MapGrid2D"
  yarp.includefile="yarp/dev/MapGrid2D.h"
)

//-------------------------------------------------

struct return_get_map {
  1: bool retval = false;
  2: yarp_dev_Nav2D_MapGrid2D themap;
}

struct return_get_map_names {
  1: bool retval = false;
  2: list<string> map_names;
}

struct return_get_location {
  1: bool retval = false;
  2: yarp_dev_Nav2D_Map2DLocation loc;
}
struct return_get_area {
  1: bool retval = false;
  2: yarp_dev_Nav2D_Map2DArea area;
}
struct return_get_path {
  1: bool retval = false;
  2: yarp_dev_Nav2D_Map2DPath path;
}
struct return_get_locations_list {
  1: bool retval = false;
  2: list<string> locations;
}
struct return_get_areas_list {
  1: bool retval = false;
  2: list<string> areas;
}
struct return_get_paths_list {
  1: bool retval = false;
  2: list<string> paths;
}
struct return_get_all_locations {
  1: bool retval = false;
  2: list<yarp_dev_Nav2D_Map2DLocation> locations;
}
struct return_get_all_areas {
  1: bool retval = false;
  2: list<yarp_dev_Nav2D_Map2DArea> areas;
}
struct return_get_all_paths {
  1: bool retval = false;
  2: list<yarp_dev_Nav2D_Map2DPath> paths;
}

//-------------------------------------------------

//    bool send(const Bottle& msg) override;
//    bool send(char *msg, size_t size) override;
//   bool receive(Bottle& msg) override;
//    int receiveChar(char& c) override;
//
//    int receiveLine(char* line, const int MaxLineLength) override;
//    int receiveBytes(unsigned char* bytes, const int size) override;

service ISerialMsgs
{
    bool setDTR (1:bool enable);
    i32 flush ();
}

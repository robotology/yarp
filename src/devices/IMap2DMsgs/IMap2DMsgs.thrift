/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
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

struct return_getMap {
  1: bool retval = false;
  2: yarp_dev_Nav2D_MapGrid2D themap;
}

struct return_getMapNames {
  1: bool retval = false;
  2: list<string> map_names;
}

struct return_getLocation {
  1: bool retval = false;
  2: yarp_dev_Nav2D_Map2DLocation loc;
}
struct return_getArea {
  1: bool retval = false;
  2: yarp_dev_Nav2D_Map2DArea area;
}
struct return_getPath {
  1: bool retval = false;
  2: yarp_dev_Nav2D_Map2DPath path;
}
struct return_getLocationsList {
  1: bool retval = false;
  2: list<string> locations;
}
struct return_getAreasList {
  1: bool retval = false;
  2: list<string> areas;
}
struct return_getPathsList {
  1: bool retval = false;
  2: list<string> paths;
}
struct return_getAllLocations {
  1: bool retval = false;
  2: list<yarp_dev_Nav2D_Map2DLocation> locations;
}
struct return_getAllAreas {
  1: bool retval = false;
  2: list<yarp_dev_Nav2D_Map2DArea> areas;
}
struct return_getAllPaths {
  1: bool retval = false;
  2: list<yarp_dev_Nav2D_Map2DPath> paths;
}

//-------------------------------------------------

service IMap2DMsgsRPC
{
    bool clearAllMapsRPC ();
    bool store_mapRPC (yarp_dev_Nav2D_MapGrid2D themap);
    return_getMap getMapRPC (string map_name);
    return_getMapNames getMapNamesRPC();
    bool remove_mapRPC (string map_name);
    bool storeLocationRPC (string location_name, yarp_dev_Nav2D_Map2DLocation loc);
    bool storeAreaRPC (string area_name, yarp_dev_Nav2D_Map2DArea area);
    bool storePathRPC (string path_name, yarp_dev_Nav2D_Map2DPath path);
    return_getLocation getLocationRPC (string location_name);
    return_getArea getAreaRPC (string area_name);
    return_getPath getPathRPC (string path_name);
    return_getLocationsList getLocationsListRPC ();
    return_getAreasList getAreasListRPC ();
    return_getPathsList getPathsListRPC ();
    return_getAllLocations getAllLocationsRPC ();
    return_getAllAreas getAllAreasRPC ();
    return_getAllPaths getAllPathsRPC ();
    bool renameLocationRPC (string original_name, string new_name);
    bool deleteLocationRPC (string location_name);
    bool deletePathRPC (string path_name);
    bool renameAreaRPC (string original_name, string new_name);
    bool renamePathRPC (string original_name, string new_name);
    bool deleteAreaRPC (string area_name);
    bool clearAllLocationsRPC ();
    bool clearAllAreasRPC ();
    bool clearAllPathsRPC ();
    bool clearAllMapsTemporaryFlagsRPC ();
    bool clearMapTemporaryFlagsRPC (string map_name);
    bool saveMapsCollectionRPC (string maps_collection_file);
    bool loadMapsCollectionRPC (string maps_collection_file);
    bool saveLocationsAndExtrasRPC (string locations_collection_file);
    bool loadLocationsAndExtrasRPC (string locations_collection_file);
    bool saveMapToDiskRPC(string map_name, string file_name);
    bool loadMapFromDiskRPC(string file_name);
    bool enableMapsCompressionRPC(bool enable_compression);
}

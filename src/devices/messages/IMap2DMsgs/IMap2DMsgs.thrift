/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

struct yarp_dev_Nav2D_Map2DLocation{
} (
  yarp.name = "yarp::dev::Nav2D::Map2DLocation"
  yarp.includefile="yarp/dev/Map2DLocation.h"
)
struct yarp_dev_Nav2D_Map2DObject{
} (
  yarp.name = "yarp::dev::Nav2D::Map2DObject"
  yarp.includefile="yarp/dev/Map2DObject.h"
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

struct yReturnValue {
} (
  yarp.name = "yarp::dev::ReturnValue"
  yarp.includefile = "yarp/dev/ReturnValue.h"
)
//-------------------------------------------------

struct return_get_map {
  1: yReturnValue retval;
  2: yarp_dev_Nav2D_MapGrid2D themap;
}

struct return_get_map_names {
  1: yReturnValue retval;
  2: list<string> map_names;
}

struct return_get_location {
  1: yReturnValue retval;
  2: yarp_dev_Nav2D_Map2DLocation loc;
}
struct return_get_object {
  1: yReturnValue retval;
  2: yarp_dev_Nav2D_Map2DObject obj;
}
struct return_get_area {
  1: yReturnValue retval;
  2: yarp_dev_Nav2D_Map2DArea area;
}
struct return_get_path {
  1: yReturnValue retval;
  2: yarp_dev_Nav2D_Map2DPath path;
}
struct return_get_locations_list {
  1: yReturnValue retval;
  2: list<string> locations;
}
struct return_get_objects_list {
  1: yReturnValue retval;
  2: list<string> objects;
}
struct return_get_areas_list {
  1: yReturnValue retval;
  2: list<string> areas;
}
struct return_get_paths_list {
  1: yReturnValue retval;
  2: list<string> paths;
}
struct return_get_all_locations {
  1: yReturnValue retval;
  2: list<yarp_dev_Nav2D_Map2DLocation> locations;
}
struct return_get_all_objects {
  1: yReturnValue retval;
  2: list<yarp_dev_Nav2D_Map2DObject> objects;
}
struct return_get_all_areas {
  1: yReturnValue retval;
  2: list<yarp_dev_Nav2D_Map2DArea> areas;
}
struct return_get_all_paths {
  1: yReturnValue retval;
  2: list<yarp_dev_Nav2D_Map2DPath> paths;
}

//-------------------------------------------------

service IMap2DMsgs
{
    yReturnValue clear_all_maps_RPC ();
    yReturnValue store_map_RPC (1:yarp_dev_Nav2D_MapGrid2D themap);
    return_get_map get_map_RPC (1:string map_name);
    return_get_map_names get_map_names_RPC();
    yReturnValue remove_map_RPC (1:string map_name);
    yReturnValue store_object_RPC (1:string object_name, 2:yarp_dev_Nav2D_Map2DObject loc);
    yReturnValue store_location_RPC (1:string location_name, 2:yarp_dev_Nav2D_Map2DLocation loc);
    yReturnValue store_area_RPC (1:string area_name, 2:yarp_dev_Nav2D_Map2DArea area);
    yReturnValue store_path_RPC (1:string path_name, 2:yarp_dev_Nav2D_Map2DPath path);
    return_get_object get_object_RPC (1:string object_name);
    return_get_location get_location_RPC (1:string location_name);
    return_get_area get_area_RPC (1:string area_name);
    return_get_path get_path_RPC (1:string path_name);
    return_get_objects_list get_objects_list_RPC ();
    return_get_locations_list get_locations_list_RPC ();
    return_get_areas_list get_areas_list_RPC ();
    return_get_paths_list get_paths_list_RPC ();
    return_get_all_objects get_all_objects_RPC ();
    return_get_all_locations get_all_locations_RPC ();
    return_get_all_areas get_all_areas_RPC ();
    return_get_all_paths get_all_paths_RPC ();
    yReturnValue rename_location_RPC (1:string original_name, 2:string new_name);
    yReturnValue delete_object_RPC (1:string object_name);
    yReturnValue delete_location_RPC (1:string location_name);
    yReturnValue delete_path_RPC (1:string path_name);
    yReturnValue rename_object_RPC (1:string original_name, 2:string new_name);
    yReturnValue rename_area_RPC (1:string original_name, 2:string new_name);
    yReturnValue rename_path_RPC (1:string original_name, 2:string new_name);
    yReturnValue delete_area_RPC (1:string area_name);
    yReturnValue clear_all_objects_RPC ();
    yReturnValue clear_all_locations_RPC ();
    yReturnValue clear_all_areas_RPC ();
    yReturnValue clear_all_paths_RPC ();
    yReturnValue clear_all_maps_temporary_flags_RPC ();
    yReturnValue clear_map_temporary_flags_RPC (1:string map_name);
    yReturnValue save_maps_collection_RPC (1:string maps_collection_file);
    yReturnValue load_maps_collection_RPC (1:string maps_collection_file);
    yReturnValue save_locations_and_extras_RPC (1:string locations_collection_file);
    yReturnValue load_locations_and_extras_RPC (1:string locations_collection_file);
    yReturnValue save_map_to_disk_RPC(1:string map_name, 2:string file_name);
    yReturnValue load_map_from_disk_RPC(1:string file_name);
    yReturnValue enable_maps_compression_RPC(1:bool enable_compression);
}

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_MAP2DSERVERIMPL_H
#define YARP_DEV_MAP2DSERVERIMPL_H

#include "IMap2DMsgs.h"
#include <yarp/dev/IMap2D.h>
#include <yarp/os/Stamp.h>
#include <yarp/dev/ReturnValue.h>
// Extra return structs for newly added object and bulk APIs (may not be included yet by generated header)
#include <return_get_object.h>
#include <return_get_objects_list.h>
#include <return_get_all_objects.h>

class IMap2DRPCd : public IMap2DMsgs
{
    private:
    yarp::dev::Nav2D::IMap2D* m_iMap = nullptr;
    std::mutex                m_mutex;

    public:
    IMap2DRPCd(yarp::dev::Nav2D::IMap2D* _imap) { m_iMap = _imap; }

    yarp::dev::ReturnValue clear_all_maps_RPC() override;
    yarp::dev::ReturnValue store_map_RPC(const yarp::dev::Nav2D::MapGrid2D& themap) override;
    return_get_map get_map_RPC(const std::string& map_name) override;
    return_get_map_names get_map_names_RPC() override;
    yarp::dev::ReturnValue remove_map_RPC(const std::string& map_name) override;
    yarp::dev::ReturnValue store_location_RPC(const std::string& location_name, const yarp::dev::Nav2D::Map2DLocation& loc) override;
    yarp::dev::ReturnValue store_area_RPC(const std::string& area_name, const yarp::dev::Nav2D::Map2DArea& area) override;
    yarp::dev::ReturnValue store_path_RPC(const std::string& path_name, const yarp::dev::Nav2D::Map2DPath& path) override;
    return_get_location get_location_RPC(const std::string& location_name) override;
    return_get_area get_area_RPC(const std::string& area_name) override;
    return_get_path get_path_RPC(const std::string& path_name) override;
    return_get_locations_list get_locations_list_RPC() override;
    return_get_areas_list get_areas_list_RPC() override;
    return_get_paths_list get_paths_list_RPC() override;
    yarp::dev::ReturnValue rename_location_RPC(const std::string& original_name, const std::string& new_name) override;
    yarp::dev::ReturnValue delete_location_RPC(const std::string& location_name) override;
    yarp::dev::ReturnValue delete_path_RPC(const std::string& path_name) override;
    yarp::dev::ReturnValue rename_area_RPC(const std::string& original_name, const std::string& new_name) override;
    yarp::dev::ReturnValue rename_path_RPC(const std::string& original_name, const std::string& new_name) override;
    yarp::dev::ReturnValue delete_area_RPC(const std::string& area_name) override;
    yarp::dev::ReturnValue clear_all_locations_RPC() override;
    yarp::dev::ReturnValue clear_all_areas_RPC() override;
    yarp::dev::ReturnValue clear_all_paths_RPC() override;
    yarp::dev::ReturnValue clear_all_maps_temporary_flags_RPC() override;
    yarp::dev::ReturnValue clear_map_temporary_flags_RPC(const std::string& map_name) override;
    yarp::dev::ReturnValue save_maps_collection_RPC(const std::string& maps_collection_file) override;
    yarp::dev::ReturnValue load_maps_collection_RPC(const std::string& maps_collection_file) override;
    yarp::dev::ReturnValue save_locations_and_extras_RPC(const std::string& locations_collection_file) override;
    yarp::dev::ReturnValue load_locations_and_extras_RPC(const std::string& locations_collection_file) override;
    // Newly introduced object-related RPCs and extended APIs
    yarp::dev::ReturnValue store_object_RPC(const std::string& object_name, const yarp::dev::Nav2D::Map2DObject& obj) override;
    return_get_object         get_object_RPC(const std::string& object_name) override;
    return_get_objects_list   get_objects_list_RPC() override;
    return_get_all_objects    get_all_objects_RPC() override;
    return_get_all_locations  get_all_locations_RPC() override;
    return_get_all_areas      get_all_areas_RPC() override;
    return_get_all_paths      get_all_paths_RPC() override;
    yarp::dev::ReturnValue rename_object_RPC(const std::string& original_name, const std::string& new_name) override;
    yarp::dev::ReturnValue delete_object_RPC(const std::string& object_name) override;
    yarp::dev::ReturnValue clear_all_objects_RPC() override;
    yarp::dev::ReturnValue save_map_to_disk_RPC(const std::string& map_name, const std::string& file_name) override;
    yarp::dev::ReturnValue load_map_from_disk_RPC(const std::string& file_name) override;
    yarp::dev::ReturnValue enable_maps_compression_RPC(const bool enable_compression) override;

    std::mutex* getMutex() {return &m_mutex;}
};

#endif // YARP_DEV_MAP2DSERVERIMPL_H

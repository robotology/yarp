/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_MAP2DSERVERIMPL_H
#define YARP_DEV_MAP2DSERVERIMPL_H

#include "IMap2DMsgs.h"
#include <yarp/dev/IMap2D.h>
#include <yarp/os/Stamp.h>

class IMap2DRPCd : public IMap2DMsgs
{
    private:
    yarp::dev::Nav2D::IMap2D* m_iMap = nullptr;
    std::mutex                m_mutex;

    public:
    void setInterface(yarp::dev::Nav2D::IMap2D* _imap) { m_iMap = _imap; }

    bool clear_all_maps_RPC() override;
    bool store_map_RPC(const yarp::dev::Nav2D::MapGrid2D& themap) override;
    return_get_map get_map_RPC(const std::string& map_name) override;
    return_get_map_names get_map_names_RPC() override;
    bool remove_map_RPC(const std::string& map_name) override;
    bool store_location_RPC(const std::string& location_name, const yarp::dev::Nav2D::Map2DLocation& loc) override;
    bool store_area_RPC(const std::string& area_name, const yarp::dev::Nav2D::Map2DArea& area) override;
    bool store_path_RPC(const std::string& path_name, const yarp::dev::Nav2D::Map2DPath& path) override;
    return_get_location get_location_RPC(const std::string& location_name) override;
    return_get_area get_area_RPC(const std::string& area_name) override;
    return_get_path get_path_RPC(const std::string& path_name) override;
    return_get_locations_list get_locations_list_RPC() override;
    return_get_areas_list get_areas_list_RPC() override;
    return_get_paths_list get_paths_list_RPC() override;
    bool rename_location_RPC(const std::string& original_name, const std::string& new_name) override;
    bool delete_location_RPC(const std::string& location_name) override;
    bool delete_path_RPC(const std::string& path_name) override;
    bool rename_area_RPC(const std::string& original_name, const std::string& new_name) override;
    bool rename_path_RPC(const std::string& original_name, const std::string& new_name) override;
    bool delete_area_RPC(const std::string& area_name) override;
    bool clear_all_locations_RPC() override;
    bool clear_all_areas_RPC() override;
    bool clear_all_paths_RPC() override;
    bool clear_all_maps_temporary_flags_RPC() override;
    bool clear_map_temporary_flags_RPC(const std::string& map_name) override;
    bool save_maps_collection_RPC(const std::string& maps_collection_file) override;
    bool load_maps_collection_RPC(const std::string& maps_collection_file) override;
    bool save_locations_and_extras_RPC(const std::string& locations_collection_file) override;
    bool load_locations_and_extras_RPC(const std::string& locations_collection_file) override;

    std::mutex* getMutex() {return &m_mutex;}
};

#endif // YARP_DEV_MAP2DSERVERIMPL_H

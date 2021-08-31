/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_MAP2DSERVERIMPL_H
#define YARP_DEV_MAP2DSERVERIMPL_H

#include "IMap2DMsgsRPC.h"
#include <yarp/dev/IMap2D.h>
#include <yarp/os/Stamp.h>

class Localization2DServer;

class IMap2DRPCd : public IMap2DMsgsRPC
{
    private:
    yarp::dev::Nav2D::IMap2D* m_iMap = nullptr;
    std::mutex                m_mutex;

    public:
    void setInterface(yarp::dev::Nav2D::IMap2D* _imap) { m_iMap = _imap; }

    virtual bool clearAllMapsRPC() override;
    virtual bool store_mapRPC(const yarp::dev::Nav2D::MapGrid2D& themap) override;
    virtual return_getMap getMapRPC(const std::string& map_name) override;
    virtual return_getMapNames getMapNamesRPC() override;
    virtual bool remove_mapRPC(const std::string& map_name) override;
    virtual bool storeLocationRPC(const std::string& location_name, const yarp::dev::Nav2D::Map2DLocation& loc) override;
    virtual bool storeAreaRPC(const std::string& area_name, const yarp::dev::Nav2D::Map2DArea& area) override;
    virtual bool storePathRPC(const std::string& path_name, const yarp::dev::Nav2D::Map2DPath& path) override;
    virtual return_getLocation getLocationRPC(const std::string& location_name) override;
    virtual return_getArea getAreaRPC(const std::string& area_name) override;
    virtual return_getPath getPathRPC(const std::string& path_name) override;
    virtual return_getLocationsList getLocationsListRPC() override;
    virtual return_getAreasList getAreasListRPC() override;
    virtual return_getPathsList getPathsListRPC() override;
    virtual bool renameLocationRPC(const std::string& original_name, const std::string& new_name) override;
    virtual bool deleteLocationRPC(const std::string& location_name) override;
    virtual bool deletePathRPC(const std::string& path_name) override;
    virtual bool renameAreaRPC(const std::string& original_name, const std::string& new_name) override;
    virtual bool renamePathRPC(const std::string& original_name, const std::string& new_name) override;
    virtual bool deleteAreaRPC(const std::string& area_name) override;
    virtual bool clearAllLocationsRPC() override;
    virtual bool clearAllAreasRPC() override;
    virtual bool clearAllPathsRPC() override;
    virtual bool clearAllMapsTemporaryFlagsRPC() override;
    virtual bool clearMapTemporaryFlagsRPC(const std::string& map_name) override;
    virtual bool saveMapsCollectionRPC(const std::string& maps_collection_file) override;
    virtual bool loadMapsCollectionRPC(const std::string& maps_collection_file) override;
    virtual bool saveLocationsAndExtrasRPC(const std::string& locations_collection_file) override;
    virtual bool loadLocationsAndExtrasRPC(const std::string& locations_collection_file) override;

    std::mutex* getMutex() {return &m_mutex;}
};

#endif // YARP_DEV_LOCALIZATION2DSERVERIMPL_H

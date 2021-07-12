/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "Map2D_nws_yarp.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Node.h>
#include <yarp/os/Publisher.h>
#include <yarp/os/Subscriber.h>

#include <yarp/dev/GenericVocabs.h>
#include <yarp/dev/IMap2D.h>
#include <yarp/dev/INavigation2D.h>

#include <yarp/math/Math.h>

#include <yarp/rosmsg/TickDuration.h>
#include <yarp/rosmsg/TickTime.h>

#include <cstdlib>
#include <fstream>
#include <limits>
#include <mutex>
#include <sstream>

using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace yarp::os;
using namespace std;

namespace {
YARP_LOG_COMPONENT(MAP2D_NWS_YARP, "yarp.device.map2D_nws_yarp")
}

/**
  * Map2D_nws_yarp
  */

Map2D_nws_yarp::Map2D_nws_yarp()
{
}

Map2D_nws_yarp::~Map2D_nws_yarp() = default;

void Map2D_nws_yarp::parse_vocab_command(yarp::os::Bottle& in, yarp::os::Bottle& out)
{
    int code = in.get(0).asVocab32();
    if (code == VOCAB_IMAP)
    {
        int cmd = in.get(1).asVocab32();
        if (cmd == VOCAB_IMAP_SET_MAP)
        {
            MapGrid2D the_map;
            Value& b = in.get(2);
            if (Property::copyPortable(b, the_map))
            {
                m_iMap2D->store_map(the_map);
                out.clear();
                out.addVocab32(VOCAB_IMAP_OK);
            }
            else
            {
                out.clear();
                out.addVocab32(VOCAB_IMAP_ERROR);
                yCError(MAP2D_NWS_YARP) << "Error in copyPortable";
            }
        }
        else if (cmd == VOCAB_IMAP_GET_MAP)
        {
            string name = in.get(2).asString();
            MapGrid2D map;
            if (m_iMap2D->get_map(name, map))
            {
                map.enable_map_compression_over_network(m_send_maps_compressed);
                out.clear();
                out.addVocab32(VOCAB_IMAP_OK);
                yarp::os::Bottle& mapbot = out.addList();
                Property::copyPortable(map, mapbot);
            }

            else
            {
                out.clear();
                out.addVocab32(VOCAB_IMAP_ERROR);
                yCError(MAP2D_NWS_YARP) << "Map" << name << "not found";
            }
        }
        else if (cmd == VOCAB_IMAP_GET_NAMES)
        {
            out.clear();
            out.addVocab32(VOCAB_IMAP_OK);
            std::vector<std::string> map_names;
            m_iMap2D->get_map_names(map_names);
            for (auto& it : map_names)
            {
                out.addString(it);
            }
        }
        else if (cmd == VOCAB_IMAP_REMOVE)
        {
            string name = in.get(2).asString();
            if (m_iMap2D->remove_map(name))
            {
                out.clear();
                out.addVocab32(VOCAB_IMAP_OK);
            }
            else
            {
                yCError(MAP2D_NWS_YARP) << "Map not found";
                out.clear();
                out.addVocab32(VOCAB_IMAP_ERROR);
            }
        }
        else if (cmd == VOCAB_IMAP_CLEAR_ALL_MAPS)
        {
            m_iMap2D->clearAllMaps();
            out.clear();
            out.addVocab32(VOCAB_IMAP_OK);
        }
        else if (cmd == VOCAB_IMAP_SAVE_X)
        {
            if (in.get(2).asVocab32() == VOCAB_IMAP_MAPS_COLLECTION)
            {
                string mapfile = in.get(3).asString();
                if (m_iMap2D->saveMapsCollection(mapfile))
                {
                    out.clear();
                    out.addVocab32(VOCAB_IMAP_OK);
                }
                else
                {
                    yCError(MAP2D_NWS_YARP, "Unable to save collection");
                    out.clear();
                    out.addVocab32(VOCAB_IMAP_ERROR);
                }
            }
            else if (in.get(2).asVocab32() == VOCAB_IMAP_LOCATIONS_COLLECTION)
            {
                string locfile = in.get(3).asString();
                if (m_iMap2D->saveLocationsAndExtras(locfile))
                {
                    out.clear();
                    out.addVocab32(VOCAB_IMAP_OK);
                }
                else
                {
                    yCError(MAP2D_NWS_YARP, "Unable to save collection");
                    out.clear();
                    out.addVocab32(VOCAB_IMAP_ERROR);
                }
            }
            else
            {
                yCError(MAP2D_NWS_YARP, "Parser error");
                out.clear();
                out.addVocab32(VOCAB_IMAP_ERROR);
            }
        }
        else if (cmd == VOCAB_IMAP_LOAD_X)
        {
            if (in.get(2).asVocab32()==VOCAB_IMAP_MAPS_COLLECTION)
            {
                string mapfile = in.get(3).asString();
                if (m_iMap2D->loadMapsCollection(mapfile))
                {
                    out.clear();
                    out.addVocab32(VOCAB_IMAP_OK);
                }
                else
                {
                    yCError(MAP2D_NWS_YARP, "Unable to load collection");
                    out.clear();
                    out.addVocab32(VOCAB_IMAP_ERROR);
                }
            }
            if (in.get(2).asVocab32() == VOCAB_IMAP_LOCATIONS_COLLECTION)
            {
                string locfile = in.get(3).asString();
                if (m_iMap2D->loadLocationsAndExtras(locfile))
                {
                    out.clear();
                    out.addVocab32(VOCAB_IMAP_OK);
                }
                else
                {
                    yCError(MAP2D_NWS_YARP, "Unable to load collection");
                    out.clear();
                    out.addVocab32(VOCAB_IMAP_ERROR);
                }
            }
            else
            {
                yCError(MAP2D_NWS_YARP, "Parser error");
                out.clear();
                out.addVocab32(VOCAB_IMAP_ERROR);
            }
        }
        else
        {
            yCError(MAP2D_NWS_YARP, "Invalid vocab received in Map2D_nws_yarp");
            out.clear();
            out.addVocab32(VOCAB_IMAP_ERROR);
        }
    }
    else if (code == VOCAB_INAVIGATION)
    {
        int cmd = in.get(1).asVocab32();
        if (cmd == VOCAB_NAV_GET_LIST_X && in.get(2).asVocab32() == VOCAB_NAV_LOCATION)
        {
            std::string info;

            out.addVocab32(VOCAB_OK);
            Bottle& l = out.addList();

            std::vector<std::string> loc_names;
            m_iMap2D->getLocationsList(loc_names);
            for (auto& it : loc_names)
            {
                out.addString(it);
            }
            yCInfo(MAP2D_NWS_YARP) << "The following locations are currently stored in the server:" << l.toString();
        }
        else if (cmd == VOCAB_NAV_GET_LIST_X && in.get(2).asVocab32() == VOCAB_NAV_AREA)
        {
            std::string info;

            out.addVocab32(VOCAB_OK);
            Bottle& l = out.addList();

            std::vector<std::string> area_names;
            m_iMap2D->getAreasList(area_names);
            for (auto& it : area_names)
            {
                out.addString(it);
            }
            yCInfo(MAP2D_NWS_YARP) << "The following areas are currently stored in the server:" << l.toString();
        }
        else if (cmd == VOCAB_NAV_GET_LIST_X && in.get(2).asVocab32() == VOCAB_NAV_PATH)
        {
            std::string info;

            out.addVocab32(VOCAB_OK);
            Bottle& l = out.addList();

            std::vector<std::string> path_names;
            m_iMap2D->getPathsList(path_names);
            for (auto& it : path_names)
            {
                out.addString(it);
            }
            yCInfo(MAP2D_NWS_YARP) << "The following paths are currently stored in the server: " << l.toString();
        }
        else if (cmd == VOCAB_NAV_CLEARALL_X && in.get(2).asVocab32() == VOCAB_NAV_LOCATION)
        {
            m_iMap2D->clearAllLocations();

            yCInfo(MAP2D_NWS_YARP) << "All locations deleted";
            out.addVocab32(VOCAB_OK);
        }
        else if (cmd == VOCAB_NAV_CLEARALL_X && in.get(2).asVocab32() == VOCAB_NAV_AREA)
        {
            m_iMap2D->clearAllAreas();

            yCInfo(MAP2D_NWS_YARP) << "All areas deleted";
            out.addVocab32(VOCAB_OK);
        }
        else if (cmd == VOCAB_NAV_CLEARALL_X && in.get(2).asVocab32() == VOCAB_NAV_PATH)
        {
            m_iMap2D->clearAllPaths();
            yCInfo(MAP2D_NWS_YARP) << "All paths deleted";
            out.addVocab32(VOCAB_OK);
        }
        else if (cmd == VOCAB_NAV_CLEARALL_X && in.get(2).asVocab32() == VOCAB_NAV_TEMPORARY_FLAGS)
        {
            m_iMap2D->clearAllMapsTemporaryFlags();
            yCInfo(MAP2D_NWS_YARP) << "Temporary flags deleted from all maps";
            out.addVocab32(VOCAB_OK);
        }
        else if (cmd == VOCAB_NAV_DELETE_X && in.get(2).asVocab32() == VOCAB_NAV_TEMPORARY_FLAGS)
        {
            std::string map_name = in.get(3).asString();
            if (m_iMap2D->clearMapTemporaryFlags(map_name))
            {
                yCInfo(MAP2D_NWS_YARP) << "Temporary flags cleaned" << map_name;
                out.addVocab32(VOCAB_OK);
            }
            else
            {
                yCError(MAP2D_NWS_YARP, "User requested an invalid map name");
                out.addVocab32(VOCAB_ERR);
            }
        }
        else if (cmd == VOCAB_NAV_DELETE_X && in.get(2).asVocab32() == VOCAB_NAV_LOCATION)
        {
            std::string location_name = in.get(3).asString();
            if (m_iMap2D->deletePath(location_name))
            {
                out.addVocab32(VOCAB_OK);
                yCInfo(MAP2D_NWS_YARP) << "Deleted location" << location_name;
            }
            else
            {
                yCError(MAP2D_NWS_YARP, "User requested an invalid location name");
                out.addVocab32(VOCAB_ERR);
            }
        }
        else if (cmd == VOCAB_NAV_DELETE_X && in.get(2).asVocab32() == VOCAB_NAV_AREA)
        {
            std::string area_name = in.get(3).asString();
            if (m_iMap2D->deleteArea(area_name))
            {
                yCInfo(MAP2D_NWS_YARP) << "Deleted area" << area_name;
                out.addVocab32(VOCAB_OK);
            }
            else
            {
                yCError(MAP2D_NWS_YARP, "User requested an invalid area name");
                out.addVocab32(VOCAB_ERR);
            }
        }
        else if (cmd == VOCAB_NAV_DELETE_X && in.get(2).asVocab32() == VOCAB_NAV_PATH)
        {
            std::string path_name = in.get(3).asString();
            if (m_iMap2D->deletePath(path_name))
            {
                yCInfo(MAP2D_NWS_YARP) << "Deleted path" << path_name;
                out.addVocab32(VOCAB_OK);
            }
            else
            {
                yCError(MAP2D_NWS_YARP, "User requested an invalid location name");
                out.addVocab32(VOCAB_ERR);
            }
        }
        else if (cmd == VOCAB_NAV_RENAME_X && in.get(2).asVocab32() == VOCAB_NAV_LOCATION)
        {
            std::string orig_name = in.get(3).asString();
            std::string new_name = in.get(4).asString();

            if (m_iMap2D->renameLocation(orig_name, new_name))
            {
                yCInfo(MAP2D_NWS_YARP) << "Location:" << orig_name << "renamed to:" << new_name;
                out.addVocab32(VOCAB_OK);
            }
            else
            {
                yCError(MAP2D_NWS_YARP, "User requested an invalid rename operation");
                out.addVocab32(VOCAB_ERR);
            }
        }
        else if (cmd == VOCAB_NAV_RENAME_X && in.get(2).asVocab32() == VOCAB_NAV_AREA)
        {
            std::string orig_name = in.get(3).asString();
            std::string new_name = in.get(4).asString();

            if (m_iMap2D->renameArea(orig_name, new_name))
            {
                yCInfo(MAP2D_NWS_YARP) << "Area:" << orig_name << "renamed to:" << new_name;
                out.addVocab32(VOCAB_OK);
            }
            else
            {
                yCError(MAP2D_NWS_YARP, "User requested an invalid rename operation");
                out.addVocab32(VOCAB_ERR);
            }
        }
        else if (cmd == VOCAB_NAV_RENAME_X && in.get(2).asVocab32() == VOCAB_NAV_PATH)
        {
            std::string orig_name = in.get(3).asString();
            std::string new_name = in.get(4).asString();

            if (m_iMap2D->renamePath(orig_name, new_name))
            {
                yCInfo(MAP2D_NWS_YARP) << "Path:" << orig_name << "renamed to:" << new_name;
                out.addVocab32(VOCAB_OK);
            }
            else
            {
                yCError(MAP2D_NWS_YARP, "User requested an invalid rename operation");
                out.addVocab32(VOCAB_ERR);
            }
        }
        else if (cmd == VOCAB_NAV_GET_X && in.get(2).asVocab32() == VOCAB_NAV_LOCATION)
        {
            std::string loc_name = in.get(3).asString();
            Map2DLocation loc;
            if (m_iMap2D->getLocation(loc_name, loc))
            {
                out.addVocab32(VOCAB_OK);
                yCInfo(MAP2D_NWS_YARP) << "Retrieved location" << loc_name << "at" << loc.toString();
                out.addString(loc.map_id);
                out.addFloat64(loc.x);
                out.addFloat64(loc.y);
                out.addFloat64(loc.theta);
            }
            else
            {
                out.addVocab32(VOCAB_ERR);
                yCError(MAP2D_NWS_YARP, "User requested an invalid location name");
            }
        }
        else if (cmd == VOCAB_NAV_GET_X && in.get(2).asVocab32() == VOCAB_NAV_AREA)
        {
            std::string area_name = in.get(3).asString();
            Map2DArea area;
            if (m_iMap2D->getArea(area_name, area))
            {
                yarp::os::Bottle areabot;
                Map2DArea areatemp = area;
                if (Property::copyPortable(areatemp, areabot) == false)
                {
                    yCError(MAP2D_NWS_YARP) << "VOCAB_NAV_GET_X VOCAB_NAV_AREA failed copyPortable()";
                    out.addVocab32(VOCAB_ERR);
                }
                else
                {
                    yCInfo(MAP2D_NWS_YARP) << "Retrieved area" << area_name << "at" << area.toString();
                    out.addVocab32(VOCAB_OK);

                    yarp::os::Bottle& areabot = out.addList();
                    Property::copyPortable(areatemp, areabot);
                }
            }
            else
            {
                out.addVocab32(VOCAB_ERR);
                yCError(MAP2D_NWS_YARP, "User requested an invalid area name");
            }
        }
        else if (cmd == VOCAB_NAV_GET_X && in.get(2).asVocab32() == VOCAB_NAV_PATH)
        {
            std::string path_name = in.get(3).asString();
            Map2DPath path;
            if (m_iMap2D->getPath(path_name, path))
            {
                yarp::os::Bottle pathbot;
                if (Property::copyPortable(path, pathbot) == false)
                {
                    yCError(MAP2D_NWS_YARP) << "VOCAB_NAV_GET_X VOCAB_NAV_PATH failed copyPortable()";
                    out.addVocab32(VOCAB_ERR);
                }
                else
                {
                    yCInfo(MAP2D_NWS_YARP) << "Retrieved path" << path_name << "at" << path.toString();
                    out.addVocab32(VOCAB_OK);

                    yarp::os::Bottle& pathbot = out.addList();
                    Property::copyPortable(path, pathbot);
                }
            }
            else
            {
                out.addVocab32(VOCAB_ERR);
                yCError(MAP2D_NWS_YARP, "User requested an invalid path name");
            }
        }
        else if (cmd == VOCAB_NAV_STORE_X && in.get(2).asVocab32() == VOCAB_NAV_LOCATION)
        {
            Map2DLocation         location;
            std::string name = in.get(3).asString();

            location.map_id = in.get(4).asString();
            location.x      = in.get(5).asFloat64();
            location.y      = in.get(6).asFloat64();
            location.theta  = in.get(7).asFloat64();

            m_iMap2D->storeLocation(name, location);
            yCInfo(MAP2D_NWS_YARP) << "Added location" << name << "at" << location.toString();
            out.addVocab32(VOCAB_OK);
        }
        else if (cmd == VOCAB_NAV_STORE_X && in.get(2).asVocab32() == VOCAB_NAV_AREA)
        {
            Map2DArea         area;
            std::string area_name = in.get(3).asString();

            Value& b = in.get(4);
            if (Property::copyPortable(b, area))
            {
                m_iMap2D->storeArea(area_name, area);
                yCInfo(MAP2D_NWS_YARP) << "Added area" << area_name << "at" << area.toString();
                out.addVocab32(VOCAB_OK);
            }
            else
            {
                yCError(MAP2D_NWS_YARP) << "VOCAB_NAV_STORE_X VOCAB_NAV_AREA failed copyPortable()";
                out.addVocab32(VOCAB_ERR);
            }
        }
        else if (cmd == VOCAB_NAV_STORE_X && in.get(2).asVocab32() == VOCAB_NAV_PATH)
        {
            Map2DPath         path;
            std::string path_name = in.get(3).asString();

            Value& b = in.get(4);
            if (Property::copyPortable(b, path))
            {
                m_iMap2D->storePath(path_name, path);
                yCInfo(MAP2D_NWS_YARP) << "Added path" << path_name << "at" << path.toString();
                out.addVocab32(VOCAB_OK);
            }
            else
            {
                yCError(MAP2D_NWS_YARP) << "VOCAB_NAV_STORE_X VOCAB_NAV_PATH failed copyPortable()";
                out.addVocab32(VOCAB_ERR);
            }
        }
        else
        {
            yCError(MAP2D_NWS_YARP) << "Invalid vocab received:" << in.toString();
            out.clear();
            out.addVocab32(VOCAB_ERR);
        }
    }
    else
    {
        yCError(MAP2D_NWS_YARP) << "Invalid vocab received:" << in.toString();
        out.clear();
        out.addVocab32(VOCAB_IMAP_ERROR);
    }
}

void Map2D_nws_yarp::parse_string_command(yarp::os::Bottle& in, yarp::os::Bottle& out)
{
    if (in.get(0).asString() == "save_locations&areas" && in.get(1).isString())
    {
        if(m_iMap2D->saveLocationsAndExtras(in.get(1).asString()))
        {
            out.addString(in.get(1).asString() + " successfully saved");
        }
        else
        {
            out.addString("save_locations&areas failed");
        }
    }
    else if (in.get(0).asString() == "load_locations&areas" && in.get(1).isString())
    {
        if(m_iMap2D->loadLocationsAndExtras(in.get(1).asString()))
        {
            out.addString(in.get(1).asString() + " successfully loaded");
        }
        else
        {
            out.addString("load_locations&areas failed");
        }
    }
    else if(in.get(0).asString() == "list_locations")
    {
        std::vector<std::string> vec;
        m_iMap2D->getLocationsList(vec);
        for (auto it = vec.begin(); it != vec.end(); ++it)
        {
            out.addString(*it);
        }
    }
    else if (in.get(0).asString() == "list_areas")
    {
        std::vector<std::string> vec;
        m_iMap2D->getAreasList(vec);
        for (auto it = vec.begin(); it != vec.end(); ++it)
        {
            out.addString(*it);
        }
    }
    else if (in.get(0).asString() == "list_paths")
    {
        std::vector<std::string> vec;
        m_iMap2D->getPathsList(vec);
        for (auto it = vec.begin(); it != vec.end(); ++it)
        {
            out.addString(*it);
        }
    }
    else if (in.get(0).asString() == "save_maps" && in.get(1).isString())
    {
        if(m_iMap2D->saveMapsCollection(in.get(1).asString()))
        {
            out.addString(in.get(1).asString() + " successfully saved");
        }
        else
        {
            out.addString("save_maps failed");
        }
    }
    else if (in.get(0).asString() == "load_maps" && in.get(1).isString())
    {
        if(m_iMap2D->loadMapsCollection(in.get(1).asString()))
        {
            out.addString(in.get(1).asString() + " successfully loaded");
        }
        else
        {
            out.addString("load_maps failed");
        }
    }
    else if (in.get(0).asString() == "save_map" && in.get(1).isString() && in.get(2).isString())
    {
        std::string map_name = in.get(1).asString();
        std::string map_file = in.get(2).asString() + ".map";

        MapGrid2D map;
        if (m_iMap2D->get_map(map_name, map)==false)
        {
            out.addString("save_map failed: map " + map_name + " not found");
        }
        else
        {
            bool b = map.saveToFile(map_file);
            if (b)
            {
                out.addString(map_file + " successfully saved");
            }
            else
            {
                out.addString("save_map failed: unable to save " + map_name + " to "+ map_file);
            }
        }
    }
    else if (in.get(0).asString() == "load_map" && in.get(1).isString())
    {
        MapGrid2D map;
        bool r = map.loadFromFile(in.get(1).asString());
        if(r)
        {
            if (m_iMap2D->store_map(map))
            {
                out.addString(in.get(1).asString() + " successfully loaded.");
            }
            else
            {
                out.addString(in.get(1).asString() + " already exists, skipping.");
            }
        }
        else
        {
            out.addString("load_map failed. Unable to load " + in.get(1).asString());
        }
    }
    else if(in.get(0).asString() == "list_maps")
    {
        std::vector<std::string> vec;
        m_iMap2D->get_map_names(vec);
        for (auto it = vec.begin(); it != vec.end(); ++it)
        {
            out.addString(*it);
        }
    }
    else if(in.get(0).asString() == "clear_all_locations")
    {
        m_iMap2D->clearAllLocations();
        out.addString("all locations cleared");
    }
    else if (in.get(0).asString() == "clear_all_areas")
    {
        m_iMap2D->clearAllAreas();
        out.addString("all areas cleared");
    }
    else if (in.get(0).asString() == "clear_all_paths")
    {
        m_iMap2D->clearAllPaths();
        out.addString("all paths cleared");
    }
    else if(in.get(0).asString() == "clear_all_maps")
    {
        m_iMap2D->clearAllMaps();
        out.addString("all maps cleared");
    }
    else if (in.get(0).asString() == "send_maps_compressed")
    {
        m_send_maps_compressed  = in.get(1).asBool();
        out.addString("compression mode of all maps set to:"+ in.get(1).asString());
    }
    else if(in.get(0).asString() == "help")
    {
        out.addVocab32("many");
        out.addString("'save_locations&areas <full path filename>' to save locations and areas on a file");
        out.addString("'load_locations&areas <full path filename>' to load locations and areas from a file");
        out.addString("'list_locations' to view a list of all stored locations");
        out.addString("'list_areas' to view a list of all stored areas");
        out.addString("'list_paths' to view a list of all stored paths");
        out.addString("'clear_all_locations' to clear all stored locations");
        out.addString("'clear_all_areas' to clear all stored areas");
        out.addString("'clear_all_paths' to clear all stored paths");
        out.addString("'save_maps <full path>' to save a map collection to a folder");
        out.addString("'load_maps <full path>' to load a map collection from a folder");
        out.addString("'save_map <map_name> <full path>' to save a single map");
        out.addString("'load_map <full path>' to load a single map");
        out.addString("'list_maps' to view a list of all stored maps");
        out.addString("'clear_all_maps' to clear all stored maps");
        out.addString("'send_maps_compressed <0/1>' to set the map transmission mode");
    }
    else
    {
        out.addString("request not understood, call 'help' to see a list of available commands");
    }
}


bool Map2D_nws_yarp::read(yarp::os::ConnectionReader& connection)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    yarp::os::Bottle in;
    yarp::os::Bottle out;
    bool ok = in.read(connection);
    if (!ok) return false;

    //parse string command
    if(in.get(0).isString())
    {
        parse_string_command(in, out);
    }
    // parse vocab command
    else if(in.get(0).isVocab32())
    {
        parse_vocab_command(in, out);
    }

    yarp::os::ConnectionWriter *returnToSender = connection.getWriter();
    if (returnToSender != nullptr)
    {
        out.write(*returnToSender);
    }
    else
    {
        yCError(MAP2D_NWS_YARP) << "Invalid return to sender";
    }
    return true;
}

bool Map2D_nws_yarp::open(yarp::os::Searchable &config)
{
    Property params;
    params.fromString(config.toString());

    if (!config.check("name"))
    {
        m_rpcPortName = "/map2D_nws_yarp/rpc";
    }
    else
    {
        m_rpcPortName = config.find("name").asString();
    }

    //open rpc port
    if (!m_rpcPort.open(m_rpcPortName))
    {
        yCError(MAP2D_NWS_YARP, "Failed to open port %s", m_rpcPortName.c_str());
        return false;
    }
    m_rpcPort.setReader(*this);

    //subdevice handling
    if (config.check("subdevice"))
    {
        Property       p;
        p.fromString(config.toString(), false);
        p.put("device", config.find("subdevice").asString());

        if (!m_drv.open(p) || !m_drv.isValid())
        {
            yCError(MAP2D_NWS_YARP) << "Failed to open subdevice.. check params";
            return false;
        }

        if (!attach(&m_drv))
        {
            yCError(MAP2D_NWS_YARP) << "Failed to open subdevice.. check params";
            return false;
        }
    }
    else
    {
        yCInfo(MAP2D_NWS_YARP) << "Waiting for device to attach";
    }

    return true;
}

bool Map2D_nws_yarp::close()
{
    yCTrace(MAP2D_NWS_YARP, "Close");
    return true;
}

bool Map2D_nws_yarp::detach()
{
    m_iMap2D = nullptr;
    return true;
}

bool Map2D_nws_yarp::attach(PolyDriver* driver)
{
    if (driver->isValid())
    {
        driver->view(m_iMap2D);
    }

    if (nullptr == m_iMap2D)
    {
        yCError(MAP2D_NWS_YARP, "Subdevice passed to attach method is invalid");
        return false;
    }

    return true;
}

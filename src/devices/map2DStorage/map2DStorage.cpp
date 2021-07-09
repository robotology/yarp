/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <sstream>
#include <limits>
#include "map2DStorage.h"
#include <yarp/dev/IMap2D.h>
#include <yarp/dev/INavigation2D.h>
#include <yarp/dev/GenericVocabs.h>
#include <yarp/math/Math.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <mutex>
#include <cstdlib>
#include <fstream>

using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace yarp::os;
using namespace std;

namespace {
YARP_LOG_COMPONENT(MAP2DSTORAGE, "yarp.device.map2DStorage")
}

/**
  * Map2DStorage
  */

Map2DStorage::Map2DStorage()
{
}

Map2DStorage::~Map2DStorage() = default;

bool Map2DStorage::saveMapsCollection(std::string mapsfile)
{
    //check if the storage is not empty
    if (m_maps_storage.size() == 0)
    {
        yCError(MAP2DSTORAGE) << "Map storage is empty";
        return false;
    }

    //open the map collection file
    std::ofstream file;
    file.open(mapsfile.c_str());
    if (!file.is_open())
    {
        yCError(MAP2DSTORAGE) << "Sorry unable to open" << mapsfile;
        return false;
    }

    bool ret = true;
    //for each map...
    for (auto& it : m_maps_storage)
    {
        //add an entry to the map collection file
        string map_filename = it.first + ".map";
        file << "mapfile: ";
        file << map_filename;
        file << endl;

        //save each individual map to a file
        ret &= it.second.saveToFile(map_filename);
    }

    file.close();
    return ret;
}

bool Map2DStorage::loadMapsCollection(std::string mapsfile)
{
    bool ret = true;
    std::ifstream file;
    file.open(mapsfile.c_str());
    if (!file.is_open())
    {
        yCError(MAP2DSTORAGE) << "loadMaps() Unable to open:" << mapsfile;
        return false;
    }
    while (!file.eof())
    {
        string dummy;
        string buffer;
        std::getline(file, buffer);
        std::istringstream iss(buffer);
        iss >> dummy;
        if (dummy == "") break;
        if (dummy == "mapfile:")
        {
            string mapfilename;
            iss >> mapfilename;
            string option;
            iss >> option;
            string mapfilenameWithPath = m_rf_mapCollection.findFile(mapfilename);

            //open the individual map file
            MapGrid2D map;
            bool r = map.loadFromFile(mapfilenameWithPath);
            if (r)
            {
                string map_name= map.getMapName();
                auto p = m_maps_storage.find(map_name);
                if (p == m_maps_storage.end())
                {
                    if (option == "crop")
                        map.crop(-1,-1,-1,-1);
                    m_maps_storage[map_name] = map;
                }
                else
                {
                    yCError(MAP2DSTORAGE) << "A map with the same name '" << map_name << "'was found, skipping...";
                    ret = false;
                }
            }
            else
            {
                yCError(MAP2DSTORAGE) << "Problems opening map file" << mapfilenameWithPath;
                ret = false;
            }
        }
        else
        {
            yCError(MAP2DSTORAGE) << "Invalid syntax, missing mapfile tag";
            ret = false;
        }
    }
    file.close();
    return ret;
}

bool Map2DStorage::open(yarp::os::Searchable &config)
{
    Property params;
    params.fromString(config.toString());

    string collection_file_name="maps_collection.ini";
    string locations_file_name="locations.ini";
    if (config.check("mapCollectionFile"))
    {
        collection_file_name= config.find("mapCollectionFile").asString();
    }

    if (config.check("mapCollectionContext"))
    {
        string collection_context_name= config.find("mapCollectionContext").asString();
        m_rf_mapCollection.setDefaultContext(collection_context_name.c_str());
        string collection_file_with_path = m_rf_mapCollection.findFile(collection_file_name);
        string locations_file_with_path = m_rf_mapCollection.findFile(locations_file_name);

        if (collection_file_with_path=="")
        {
            yCInfo(MAP2DSTORAGE) << "No locations loaded";
        }
        else
        {
            bool ret  = loadLocationsAndExtras(locations_file_with_path);
            if (ret) { yCInfo(MAP2DSTORAGE) << "Location file" << locations_file_with_path << "successfully loaded."; }
            else { yCError(MAP2DSTORAGE) << "Problems opening file" << locations_file_with_path; }
        }

        if (collection_file_with_path=="")
        {
            yCError(MAP2DSTORAGE) << "Unable to find file" << collection_file_name << "within the specified context:" << collection_context_name;
            return false;
        }
        if (loadMapsCollection(collection_file_with_path))
        {
            yCInfo(MAP2DSTORAGE) << "Map collection file:" << collection_file_with_path << "successfully loaded.";
            if (m_maps_storage.size() > 0)
            {
                yCInfo(MAP2DSTORAGE) << "Available maps are:";
                for (auto& it : m_maps_storage)
                {
                    yCInfo(MAP2DSTORAGE) << it.first;
                }
            }
            else
            {
                yCInfo(MAP2DSTORAGE) << "No maps available";
            }
            if (m_locations_storage.size() > 0)
            {
                yCInfo(MAP2DSTORAGE) << "Available Locations are:";
                for (auto& it : m_locations_storage)
                {
                    yCInfo(MAP2DSTORAGE) << it.first;
                }
            }
            else
            {
                yCInfo(MAP2DSTORAGE) << "No locations available";
            }

            if (m_areas_storage.size() > 0)
            {
                yCInfo(MAP2DSTORAGE) << "Available areas are:";
                for (auto& it : m_areas_storage)
                {
                    yCInfo(MAP2DSTORAGE) << it.first;
                }
            }
            else
            {
                yCInfo(MAP2DSTORAGE) << "No areas available";
            }
        }
        else
        {
            yCError(MAP2DSTORAGE) << "Unable to load map collection file:" << collection_file_with_path;
            return false;
        }
    }

    if (!config.check("name"))
    {
        m_rpcPortName = "/map2DStorage/rpc";
    }
    else
    {
        m_rpcPortName = config.find("name").asString();
    }

    //open rpc port
    if (!m_rpcPort.open(m_rpcPortName))
    {
        yCError(MAP2DSTORAGE, "Failed to open port %s", m_rpcPortName.c_str());
        return false;
    }
    m_rpcPort.setReader(*this);

    return true;
}

bool Map2DStorage::close()
{
    yCTrace(MAP2DSTORAGE, "Close");
    return true;
}

bool Map2DStorage::priv_load_locations_and_areas_v1(std::ifstream& file)
{
    std::string     buffer;
    std::getline(file, buffer);
    if (buffer != "Locations:")
    {
        yCError(MAP2DSTORAGE) << "Unable to parse Locations section!";
        return false;
    }

    while (1)
    {
        std::getline(file, buffer);
        if (buffer == "Areas:") break;
        if (file.eof())
        {
            yCError(MAP2DSTORAGE) << "Unexpected End Of File";
            return false;
        }
        Bottle b;
        b.fromString(buffer);
        size_t bot_size = b.size();
        if (bot_size != 5)
        {
            yCError(MAP2DSTORAGE) << "Unable to parse contents of Areas section!";
            return false;
        }
        Map2DLocation   location;
        std::string name = b.get(0).asString();
        location.map_id = b.get(1).asString();
        location.x = b.get(2).asFloat64();
        location.y = b.get(3).asFloat64();
        location.theta = b.get(4).asFloat64();
        m_locations_storage[name] = location;
    }

    if (buffer != "Areas:")
    {
        yCError(MAP2DSTORAGE) << "Unable to parse Areas section!";
        return false;
    }

    while (1)
    {
        Map2DArea       area;
        std::getline(file, buffer);
        if (file.eof()) break;

        Bottle b;
        b.fromString(buffer);
        size_t bot_size = b.size();
        std::string name = b.get(0).asString();
        area.map_id = b.get(1).asString();
        size_t area_size = b.get(2).asInt32();
        if (area_size <= 0 || bot_size != area_size * 2 + 3)
        {
            yCError(MAP2DSTORAGE) << "Unable to parse contents of Areas section!";
            return false;
        }
        for (size_t ai = 3; ai < bot_size; ai += 2)
        {
            double xpos = b.get(ai).asFloat64();
            double ypos = b.get(ai + 1).asFloat64();
            area.points.push_back(yarp::math::Vec2D<double>(xpos, ypos));
        }
        m_areas_storage[name] = area;
    }
    return true;
}

bool Map2DStorage::priv_load_locations_and_areas_v2(std::ifstream& file)
{
    std::string     buffer;
    std::getline(file, buffer);
    if (buffer != "Locations:")
    {
        yCError(MAP2DSTORAGE) << "Unable to parse Locations section!";
        return false;
    }

    while (1)
    {
        std::getline(file, buffer);
        if (buffer == "Areas:") break;
        if (file.eof())
        {
            yCError(MAP2DSTORAGE) << "Unexpected End Of File";
            return false;
        }
        Bottle b;
        b.fromString(buffer);
        size_t bot_size = b.size();
        if (bot_size != 5)
        {
            yCError(MAP2DSTORAGE) << "Unable to parse contents of Areas section!";
            return false;
        }
        Map2DLocation   location;
        std::string name = b.get(0).asString();
        location.map_id = b.get(1).asString();
        location.x = b.get(2).asFloat64();
        location.y = b.get(3).asFloat64();
        location.theta = b.get(4).asFloat64();
        m_locations_storage[name] = location;
    }

    if (buffer != "Areas:")
    {
        yCError(MAP2DSTORAGE) << "Unable to parse Areas section!";
        return false;
    }

    while (1)
    {
        Map2DArea       area;
        std::getline(file, buffer);
        if (buffer == "Paths:") break;
        if (file.eof()) break;

        Bottle b;
        b.fromString(buffer);
        size_t bot_size = b.size();
        std::string name = b.get(0).asString();
        area.map_id = b.get(1).asString();
        size_t area_size = b.get(2).asInt32();
        if (area_size <= 0 || bot_size != area_size * 2 + 3)
        {
            yCError(MAP2DSTORAGE) << "Unable to parse contents of Areas section!";
            return false;
        }
        for (size_t ai = 3; ai < bot_size; ai += 2)
        {
            double xpos = b.get(ai).asFloat64();
            double ypos = b.get(ai + 1).asFloat64();
            area.points.push_back(yarp::math::Vec2D<double>(xpos, ypos));
        }
        m_areas_storage[name] = area;
    }

    if (buffer != "Paths:")
    {
        yCError(MAP2DSTORAGE) << "Unable to parse Paths section!";
        return false;
    }

    while (1)
    {
        Map2DPath       path;
        std::getline(file, buffer);
        if (file.eof()) break;

        Bottle b;
        b.fromString(buffer);
        size_t bot_size = b.size();
        YARP_UNUSED(bot_size);
        std::string name = b.get(0).asString();
        size_t i = 1;
        do
        {
            if (b.get(i).isList())
            {
                Bottle* ll = b.get(i).asList();
                if (ll && ll->size() == 4)
                {
                    Map2DLocation loc;
                    loc.map_id = ll->get(0).asString();
                    loc.x = ll->get(1).asFloat64();
                    loc.y = ll->get(2).asFloat64();
                    loc.theta = ll->get(3).asFloat64();
                    path.push_back(loc);
                }
                else
                {
                    yCError(MAP2DSTORAGE) << "Unable to parse contents of Paths section!";
                    return false;
                }
            }
            else
            {
                break;
            }
            i++;
        } while (1);
        m_paths_storage[name] = path;
    }

    return true;
}

bool Map2DStorage::loadLocationsAndExtras(std::string locations_file)
{
    std::ifstream file;
    file.open (locations_file.c_str());

    if(!file.is_open())
    {
        yCError(MAP2DSTORAGE) << "Unable to open" << locations_file << "locations file.";
        return false;
    }

    std::string     buffer;

    std::getline(file, buffer);
    if (buffer != "Version:")
    {
        yCError(MAP2DSTORAGE) << "Unable to parse Version section!";
        file.close();
        return false;
    }
    std::getline(file, buffer);
    int version = atoi(buffer.c_str());

    if (version == 1)
    {
        if (!priv_load_locations_and_areas_v1(file))
        {
            yCError(MAP2DSTORAGE) << "Call to load_locations_and_areas_v1 failed";
            file.close();
            return false;
        }
    }
    else if (version == 2)
    {
        if (!priv_load_locations_and_areas_v2(file))
        {
            yCError(MAP2DSTORAGE) << "Call to load_locations_and_areas_v2 failed";
            file.close();
            return false;
        }
    }
    else
    {
        yCError(MAP2DSTORAGE) << "Only versions 1,2 supported!";
        file.close();
        return false;
    }

    //on success
    file.close();
    yCDebug(MAP2DSTORAGE) << "Locations file" << locations_file << "loaded, "
                          << m_locations_storage.size() << "locations and "
                          << m_areas_storage.size() << " areas and "
                          << m_paths_storage.size() << " paths available";
    return true;
}

bool Map2DStorage::saveLocationsAndExtras(std::string locations_file)
{
    std::ofstream file;
    file.open (locations_file.c_str());

    if(!file.is_open())
    {
        yCError(MAP2DSTORAGE) << "Unable to open" << locations_file << "locations file.";
        return false;
    }

    std::string     s;
    Map2DLocation   loc;
    Map2DArea       area;
    s = " ";

    file << "Version:\n";
    file << "2\n";

    {
        file << "Locations:\n";
        std::map<std::string, Map2DLocation>::iterator it;
        for (it = m_locations_storage.begin(); it != m_locations_storage.end(); ++it)
        {
            loc = it->second;
            file << it->first << s << loc.map_id << s << loc.x << s << loc.y << s << loc.theta << "\n";
        }
    }

    {
        file << "Areas:\n";
        std::map<std::string, Map2DArea>::iterator it2;
        for (it2 = m_areas_storage.begin(); it2 != m_areas_storage.end(); ++it2)
        {
            area = it2->second;
            file << it2->first << s << area.map_id << s << area.points.size() << s;
            for (size_t i = 0; i < area.points.size(); i++)
            {
                file << area.points[i].x << s;
                file << area.points[i].y << s;
            }
            file << "\n";
        }
    }

    {
        file << "Paths:\n";
        std::map<std::string, Map2DPath>::iterator it3;
        for (it3 = m_paths_storage.begin(); it3 != m_paths_storage.end(); ++it3)
        {
            file << it3->first << " "; // the name of the path
            for (size_t i=0; i<it3->second.size(); i++)
            {
                loc = it3->second[i];
                file << "( " <<loc.map_id << s << loc.x << s << loc.y << s << loc.theta << ") ";
            }
            file << "\n";
        }
    }

    file.close();
    yCDebug(MAP2DSTORAGE) << "Locations file" << locations_file << "saved.";
    return true;
}


bool Map2DStorage::clearAllMaps()
{
    m_maps_storage.clear();
    return true;
}

bool Map2DStorage::store_map(const yarp::dev::Nav2D::MapGrid2D& map)
{
    string map_name = map.getMapName();
    auto it = m_maps_storage.find(map_name);
    if (it == m_maps_storage.end())
    {
        //add a new map
        m_maps_storage[map_name] = map;
    }
    else
    {
        //the map already exists
        m_maps_storage[map_name] = map;
    }
    return true;
}

bool Map2DStorage::get_map(std::string map_name, yarp::dev::Nav2D::MapGrid2D& map)
{
    auto it = m_maps_storage.find(map_name);
    if (it != m_maps_storage.end())
    {
        map=it->second;
        return true;
    }
    return false;
}

bool Map2DStorage::get_map_names(std::vector<std::string>& map_names)
{
    map_names.clear();
    for (auto& it : m_maps_storage)
    {
        map_names.push_back(it.first);
    }
    return true;
}

bool Map2DStorage::remove_map(std::string map_name)
{
    size_t rem = m_maps_storage.erase(map_name);
    if (rem == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool Map2DStorage::storeLocation(std::string location_name, yarp::dev::Nav2D::Map2DLocation loc)
{
    m_locations_storage.insert(std::pair<std::string, Map2DLocation>(location_name, loc));
    return true;
}

bool Map2DStorage::storeArea(std::string area_name, yarp::dev::Nav2D::Map2DArea area)
{
    m_areas_storage.insert(std::pair<std::string, Map2DArea>(area_name, area));
    return true;
}

bool Map2DStorage::storePath(std::string path_name, yarp::dev::Nav2D::Map2DPath path)
{
    m_paths_storage.insert(std::pair<std::string, Map2DPath>(path_name, path));
    return true;
}

bool Map2DStorage::getLocation(std::string location_name, yarp::dev::Nav2D::Map2DLocation& loc)
{
    auto it = m_locations_storage.find(location_name);
    if (it != m_locations_storage.end())
    {
        loc = it->second;
        return true;
    }
    return false;
}

bool Map2DStorage::getArea(std::string area_name, yarp::dev::Nav2D::Map2DArea& area)
{
    auto it = m_areas_storage.find(area_name);
    if (it != m_areas_storage.end())
    {
        area = it->second;
        return true;
    }
    return false;
}

bool Map2DStorage::getPath(std::string path_name, yarp::dev::Nav2D::Map2DPath& path)
{
    auto it = m_paths_storage.find(path_name);
    if (it != m_paths_storage.end())
    {
        path = it->second;
        return true;
    }
    return false;
}

bool Map2DStorage::getLocationsList(std::vector<std::string>& locations)
{
    locations.clear();
    for (auto& it : m_locations_storage)
    {
        locations.push_back(it.first);
    }
    return true;
}

bool Map2DStorage::getAreasList(std::vector<std::string>& areas)
{
    areas.clear();
    for (auto& it : m_areas_storage)
    {
        areas.push_back(it.first);
    }
    return true;
}

bool Map2DStorage::getPathsList(std::vector<std::string>& paths)
{
    paths.clear();
    for (auto& it : m_paths_storage)
    {
        paths.push_back(it.first);
    }
    return true;
}

bool Map2DStorage::renameLocation(std::string original_name, std::string new_name)
{
    std::map<std::string, Map2DLocation>::iterator orig_it;
    orig_it = m_locations_storage.find(original_name);
    std::map<std::string, Map2DLocation>::iterator new_it;
    new_it = m_locations_storage.find(new_name);

    if (orig_it != m_locations_storage.end() &&
        new_it == m_locations_storage.end())
    {
        auto loc = orig_it->second;
        m_locations_storage.erase(orig_it);
        m_locations_storage.insert(std::pair<std::string, Map2DLocation>(new_name, loc));
        return true;
    }
    return false;
}

bool Map2DStorage::deleteLocation(std::string location_name)
{
    std::map<std::string, Map2DLocation>::iterator it;
    it = m_locations_storage.find(location_name);
    if (it != m_locations_storage.end())
    {
        m_locations_storage.erase(it);
        return true;
    }
    return false;
}

bool Map2DStorage::deleteArea(std::string area_name)
{
    std::map<std::string, Map2DArea>::iterator it;
    it = m_areas_storage.find(area_name);
    if (it != m_areas_storage.end())
    {
        m_areas_storage.erase(it);
        return true;
    }
    return false;
}

bool Map2DStorage::deletePath(std::string path_name)
{
    std::map<std::string, Map2DPath>::iterator it;
    it = m_paths_storage.find(path_name);
    if (it != m_paths_storage.end())
    {
        m_paths_storage.erase(it);
        return true;
    }
    return false;
}

bool Map2DStorage::renameArea(std::string original_name, std::string new_name)
{

    std::map<std::string, Map2DArea>::iterator orig_it;
    orig_it = m_areas_storage.find(original_name);
    std::map<std::string, Map2DArea>::iterator new_it;
    new_it = m_areas_storage.find(new_name);

    if (orig_it != m_areas_storage.end() &&
        new_it == m_areas_storage.end())
    {
        auto area = orig_it->second;
        m_areas_storage.erase(orig_it);
        m_areas_storage.insert(std::pair<std::string, Map2DArea>(new_name, area));
        return true;
    }
    return false;
}

bool Map2DStorage::renamePath(std::string original_name, std::string new_name)
{

    std::map<std::string, Map2DPath>::iterator orig_it;
    orig_it = m_paths_storage.find(original_name);
    std::map<std::string, Map2DPath>::iterator new_it;
    new_it = m_paths_storage.find(new_name);

    if (orig_it != m_paths_storage.end() &&
        new_it == m_paths_storage.end())
    {
        auto area = orig_it->second;
        m_paths_storage.erase(orig_it);
        m_paths_storage.insert(std::pair<std::string, Map2DPath>(new_name, area));
        return true;
    }
    return false;
}



bool Map2DStorage::clearAllLocations()
{
    m_locations_storage.clear();
    return true;
}

bool Map2DStorage::clearAllAreas()
{
    m_areas_storage.clear();
    return true;
}

bool Map2DStorage::clearAllPaths()
{
    m_paths_storage.clear();
    return true;
}

bool Map2DStorage::clearAllMapsTemporaryFlags()
{
    for (auto it = m_maps_storage.begin(); it != m_maps_storage.end(); it++)
    {
        it->second.clearMapTemporaryFlags();
    }
    return true;
}

bool Map2DStorage::clearMapTemporaryFlags(std::string map_name)
{
    auto it = m_maps_storage.find(map_name);
    if (it != m_maps_storage.end())
    {
        it->second.clearMapTemporaryFlags();
        return true;
    }
    else
    {
        return false;
    }
}

bool Map2DStorage::read(yarp::os::ConnectionReader& connection)
{
    yCWarning(MAP2DSTORAGE) << "not yet implemented";

    std::lock_guard<std::mutex> lock(m_mutex);
    yarp::os::Bottle in;
    yarp::os::Bottle out;
    bool ok = in.read(connection);
    if (!ok) return false;

    //parse string command
    if (in.get(0).isString())
    {
        //parse_string_command(in, out);
    }
    // parse vocab command
    else if (in.get(0).isVocab32())
    {
        //parse_vocab_command(in, out);
    }

    yarp::os::ConnectionWriter* returnToSender = connection.getWriter();
    if (returnToSender != nullptr)
    {
        out.write(*returnToSender);
    }
    else
    {
        yCError(MAP2DSTORAGE) << "Invalid return to sender";
    }
    return true;
}

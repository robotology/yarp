/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <sstream>
#include <limits>
#include "Map2DServer.h"
#include <yarp/dev/IMap2D.h>
#include <yarp/dev/INavigation2D.h>
#include <yarp/dev/GenericVocabs.h>
#include <yarp/math/Math.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/LockGuard.h>
#include <cstdlib>
#include <fstream>
#include <yarp/os/Publisher.h>
#include <yarp/os/Subscriber.h>
#include <yarp/os/Node.h>
#include <yarp/rosmsg/TickDuration.h>
#include <yarp/rosmsg/TickTime.h>

using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::os;
using namespace std;

// needed for the driver factory.
yarp::dev::DriverCreator *createMap2DServer() {
    return new DriverCreatorOf<yarp::dev::Map2DServer>("map2DServer",
        "map2DServer",
        "yarp::dev::map2DServer");
}

/**
  * Map2DServer
  */

Map2DServer::Map2DServer()
{
    m_enable_publish_ros_map = false;
    m_enable_subscribe_ros_map = false;
    m_rosNode = nullptr;
}

Map2DServer::~Map2DServer() = default;

void Map2DServer::parse_vocab_command(yarp::os::Bottle& in, yarp::os::Bottle& out)
{
    int code = in.get(0).asVocab();
//     bool ret = false;
    if (code == VOCAB_IMAP)
    {
        int cmd = in.get(1).asVocab();
        if (cmd == VOCAB_IMAP_SET_MAP)
        {
            MapGrid2D the_map;
            Value& b = in.get(2);
            if (Property::copyPortable(b, the_map))
            {
                string map_name = the_map.getMapName();
                auto it = m_maps_storage.find(map_name);
                if (it == m_maps_storage.end())
                {
                    //add a new map
                    m_maps_storage[map_name] = the_map;
                    out.clear();
                    out.addVocab(VOCAB_IMAP_OK);
                }
                else
                {
                    //the map already exists
                    m_maps_storage[map_name] = the_map;
                    out.clear();
                    out.addVocab(VOCAB_IMAP_OK);
                }
            }
            else
            {
                out.clear();
                out.addVocab(VOCAB_IMAP_ERROR);
                yError() << "Error in copyPortable";
            }
        }
        else if (cmd == VOCAB_IMAP_GET_MAP)
        {
            string name = in.get(2).asString();
            auto it = m_maps_storage.find(name);
            if (it != m_maps_storage.end())
            {
                out.clear();
                out.addVocab(VOCAB_IMAP_OK);
                yarp::os::Bottle& mapbot = out.addList();
                Property::copyPortable(it->second, mapbot);
            }
            else
            {
                out.clear();
                out.addVocab(VOCAB_IMAP_ERROR);
                yError() << "Map" << name << "not found";
            }
        }
        else if (cmd == VOCAB_IMAP_GET_NAMES)
        {
            out.clear();
            out.addVocab(VOCAB_IMAP_OK);

            for (auto& it : m_maps_storage)
            {
                out.addString(it.first);
            }
        }
        else if (cmd == VOCAB_IMAP_REMOVE)
        {
            string name = in.get(2).asString();
            size_t rem = m_maps_storage.erase(name);
            if (rem == 0)
            {
                yError() << "Map not found";
                out.clear();
                out.addVocab(VOCAB_IMAP_ERROR);
            }
            else
            {
                out.clear();
                out.addVocab(VOCAB_IMAP_OK);
            }
        }
        else if (cmd == VOCAB_IMAP_CLEAR)
        {
            m_maps_storage.clear();
            out.clear();
            out.addVocab(VOCAB_IMAP_OK);
        }
        else if (cmd == VOCAB_IMAP_SAVE_COLLECTION)
        {
            string mapfile = in.get(2).asString();
            if (saveMaps(mapfile))
            {
                out.clear();
                out.addVocab(VOCAB_IMAP_OK);
            }
            else
            {
                yError("Map2DServer: Unable to save collection");
                out.clear();
                out.addVocab(VOCAB_IMAP_ERROR);
            }
        }
        else if (cmd == VOCAB_IMAP_LOAD_COLLECTION)
        {
            string mapfile = in.get(2).asString();
            if (loadMaps(mapfile))
            {
                out.clear();
                out.addVocab(VOCAB_IMAP_OK);
            }
            else
            {
                yError("Map2DServer: Unable to load collection");
                out.clear();
                out.addVocab(VOCAB_IMAP_ERROR);
            }
        }
        else
        {
            yError("Invalid vocab received in Map2DServer");
            out.clear();
            out.addVocab(VOCAB_IMAP_ERROR);
        }
    }
    else if (code == VOCAB_INAVIGATION)
    {
        int cmd = in.get(1).asVocab();
        if (cmd == VOCAB_NAV_GET_LOCATION_LIST)
        {
            std::string info;

            out.addVocab(VOCAB_OK);
            Bottle& l = out.addList();

            std::map<std::string, Map2DLocation>::iterator it;
            for (it = m_locations_storage.begin(); it != m_locations_storage.end(); ++it)
            {
                l.addString(it->first);
            }
            yInfo() << "The following locations are currently stored in the server: "<<l.toString();
//             ret = true;
        }
        else if (cmd == VOCAB_NAV_CLEAR)
        {
            m_locations_storage.clear();
            yInfo() << "All locations deleted ";
            out.addVocab(VOCAB_OK);
//             ret = true;
        }
        else if (cmd == VOCAB_NAV_DELETE)
        {
            std::string name = in.get(2).asString();

            std::map<std::string, Map2DLocation>::iterator it;
            it = m_locations_storage.find(name);
            if (it != m_locations_storage.end())
            {
                yInfo() << "Deleted location " << name;
                m_locations_storage.erase(it);
                out.addVocab(VOCAB_OK);
            }
            else
            {
                yError("User requested an invalid location name");
                out.addVocab(VOCAB_ERR);
            }

//             ret = true;
        }
        else if (cmd == VOCAB_NAV_GET_LOCATION)
        {
            std::string name = in.get(2).asString();

            std::map<std::string, Map2DLocation>::iterator it;
            it = m_locations_storage.find(name);
            if (it != m_locations_storage.end())
            {
                out.addVocab(VOCAB_OK);
                Map2DLocation loc = it->second;
                yInfo() << "Retrieved location " << name << "at " << loc.toString();
                out.addString(loc.map_id);
                out.addFloat64(loc.x);
                out.addFloat64(loc.y);
                out.addFloat64(loc.theta);
            }
            else
            {
                out.addVocab(VOCAB_ERR);
                yError("User requested an invalid location name");
            }

//             ret = true;
        }
        else if (cmd == VOCAB_NAV_STORE_ABS)
        {
            Map2DLocation         location;
            std::string name = in.get(2).asString();

            location.map_id = in.get(3).asString();
            location.x      = in.get(4).asFloat64();
            location.y      = in.get(5).asFloat64();
            location.theta  = in.get(6).asFloat64();

            m_locations_storage.insert(std::pair<std::string, Map2DLocation>(name, location));
            yInfo() << "Added location " << name << "at " << location.toString();
            out.addVocab(VOCAB_OK);
//             ret = true;
        }
        else
        {
            yError("Invalid vocab received in LocationsServer");
        }
    }
    else
    {
        yError("Invalid vocab received in Map2DServer");
        out.clear();
        out.addVocab(VOCAB_IMAP_ERROR);
    }
}

void Map2DServer::parse_string_command(yarp::os::Bottle& in, yarp::os::Bottle& out)
{
    if (in.get(0).asString() == "save_locations" && in.get(1).isString())
    {
        if(save_locations(in.get(1).asString()))
        {
            out.addString(in.get(1).asString() + " successfully saved");
        }
        else
        {
            out.addString("save_locations failed");
        }
    }
    else if (in.get(0).asString() == "load_locations" && in.get(1).isString())
    {
        if(load_locations(in.get(1).asString()))
        {
            out.addString(in.get(1).asString() + " successfully loaded");
        }
        else
        {
            out.addString("load_locations failed");
        }
    }
    else if(in.get(0).asString() == "list_locations")
    {
        std::map<std::string, Map2DLocation>::iterator it;
        for (it = m_locations_storage.begin(); it != m_locations_storage.end(); ++it)
        {
            out.addString(it->first);
        }
    }
    else if (in.get(0).asString() == "save_maps" && in.get(1).isString())
    {
        if(saveMaps(in.get(1).asString()))
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
        if(loadMaps(in.get(1).asString()))
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
        auto p = m_maps_storage.find(map_name);
        if (p == m_maps_storage.end())
        {
            out.addString("save_map failed: map " + map_name + " not found");
        }
        else
        {
            bool b = p->second.saveToFile(map_file);
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
        yarp::dev::MapGrid2D map;
        bool r = map.loadFromFile(in.get(1).asString());
        if(r)
        {
            string map_name= map.getMapName();
            auto p = m_maps_storage.find(map_name);
            if (p == m_maps_storage.end())
            {
                m_maps_storage[map_name] = map;
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
        std::map<std::string, MapGrid2D>::iterator it;
        for (it = m_maps_storage.begin(); it != m_maps_storage.end(); ++it)
        {
            out.addString(it->first);
        }
    }
    else if(in.get(0).asString() == "clear_all_locations")
    {
        m_locations_storage.clear();
        out.addString("all locations cleared");
    }
    else if(in.get(0).asString() == "clear_all_maps")
    {
        m_maps_storage.clear();
        out.addString("all maps cleared");
    }
    else if(in.get(0).asString() == "help")
    {
        out.addVocab(Vocab::encode("many"));
        out.addString("'save_locations <full path filename>' to save locations on a file");
        out.addString("'load_locations <full path filename>' to load locations from a file");
        out.addString("'list_locations' to view a list of all stored locations");
        out.addString("'clear_all_locations' to clear all stored locations");
        out.addString("'save_maps <full path>' to save a map collection to a folder");
        out.addString("'load_maps <full path>' to load a map collection from a folder");
        out.addString("'save_map <map_name> <full path>' to save a single map");
        out.addString("'load_map <full path>' to load a single map");
        out.addString("'list_maps' to view a list of all stored maps");
        out.addString("'clear_all_maps' to clear all stored maps");
    }
    else
    {
        out.addString("request not understood, call 'help' to see a list of available commands");
    }

    //updateVizMarkers();
}

bool Map2DServer::read(yarp::os::ConnectionReader& connection)
{
    LockGuard lock(m_mutex);
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
    else if(in.get(0).isVocab())
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
        yError() << "Map2DServer: invalid return to sender";
    }
    return true;
}

bool Map2DServer::saveMaps(std::string mapsfile)
{
    if (m_maps_storage.size() == 0)
    {
        yError() << "map storage is empty";
        return false;
    }
    std::ofstream file;
    file.open(mapsfile.c_str());
    if (!file.is_open())
    {
        yError() << "sorry unable to open" << mapsfile;
        return false;
    }
    bool ret = true;
    for (auto& it : m_maps_storage)
    {
        string map_filename = it.first + ".map";
        file << "mapfile: ";
        file << map_filename;
        file << endl;
        ret &= it.second.saveToFile(map_filename);
    }
    file.close();
    return ret;
}

bool Map2DServer::loadMaps(std::string mapsfile)
{
    bool ret = true;
    std::ifstream file;
    file.open(mapsfile.c_str());
    if (!file.is_open())
    {
        yError() << "Map2DServer::loadMaps() Unable to open:" << mapsfile;
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
            yarp::dev::MapGrid2D map;
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
                    yError() << "A map with the same name '" << map_name << "'was found, skipping...";
                    ret = false;
                }
            }
            else
            {
                yError() << "Problems opening map file" << mapfilenameWithPath;
                ret = false;
            }
        }
        else
        {
            yError() << "Invalid syntax, missing mapfile tag";
            ret = false;
        }
    }
    file.close();
    return ret;
}

bool Map2DServer::open(yarp::os::Searchable &config)
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
            yInfo() << "No locations loaded";
        }
        else
        {
            bool ret  = load_locations(locations_file_with_path);
            if (ret) { yInfo() << "Location file" << locations_file_with_path << "successfully loaded."; }
            else { yError() << "Problems opening file" << locations_file_with_path; }
        }

        if (collection_file_with_path=="")
        {
            yError() << "Unable to find file "<< collection_file_name << " within the specified context: " << collection_context_name;
            return false;
        }
        if (loadMaps(collection_file_with_path))
        {
            yInfo() << "Map collection file:" << collection_file_with_path << "successfully loaded.";
            yInfo() << "Available maps are:";
            for (auto& it : m_maps_storage)
            {
                yInfo() << it.first;
            }
        }
        else
        {
            yError() << "Unable to load map collection file:" << collection_file_with_path;
            return false;
        }
    }

    if (!config.check("name"))
    {
        m_rpcPortName = "/mapServer/rpc";
    }
    else
    {
        m_rpcPortName = config.find("name").asString();
    }

    //open rpc port
    if (!m_rpcPort.open(m_rpcPortName))
    {
        yError("Map2DServer: failed to open port %s", m_rpcPortName.c_str());
        return false;
    }
    m_rpcPort.setReader(*this);

    //ROS configuration
    if (config.check("ROS"))
    {
        yInfo ("Configuring ROS params");
        Bottle ROS_config = config.findGroup("ROS");
        if (ROS_config.check("enable_ros_publisher") == false)
        {
            yError() << "Map2DServer: Missing 'enable_ros_publisher' in ROS group";
            return false;
        }
        if (ROS_config.find("enable_ros_publisher").asInt32() == 1 || ROS_config.find("enable_ros_publisher").asString() == "true")
        {
            m_enable_publish_ros_map = true;
            yInfo() << "Map2DServer: Enabled ROS publisher";
        }
        if (ROS_config.check("enable_ros_subscriber") == false)
        {
            yError() << "Map2DServer: Missing 'enable_ros_subscriber' in ROS group";
            return false;
        }
        if (ROS_config.find("enable_ros_subscriber").asInt32() == 1 || ROS_config.find("enable_ros_subscriber").asString() == "true")
        {
            m_enable_subscribe_ros_map = true;
            yInfo() << "Map2DServer: Enabled ROS subscriber";
        }

        if (m_enable_subscribe_ros_map || m_enable_publish_ros_map)
        {
            if (m_rosNode == nullptr)
            {
                m_rosNode = new yarp::os::Node(ROSNODENAME);
            }
            if (m_enable_publish_ros_map && !m_rosPublisherPort_map.topic(ROSTOPICNAME_MAP))
            {
                yError() << "Map2DServer: unable to publish to " << ROSTOPICNAME_MAP << " topic, check your yarp-ROS network configuration";
                return false;
            }
            if (m_enable_publish_ros_map && !m_rosPublisherPort_metamap.topic(ROSTOPICNAME_MAPMETADATA))
            {
                yError() << "Map2DServer: unable to publish to " << ROSTOPICNAME_MAPMETADATA << " topic, check your yarp-ROS network configuration";
                return false;
            }

            if (m_enable_subscribe_ros_map && !m_rosSubscriberPort_map.topic(ROSTOPICNAME_MAP))
            {
                yError() << "Map2DServer: unable to subscribe to " << ROSTOPICNAME_MAP << " topic, check your yarp-ROS network configuration";
                return false;
            }
            if (m_enable_subscribe_ros_map && !m_rosSubscriberPort_metamap.topic(ROSTOPICNAME_MAPMETADATA))
            {
                yError() << "Map2DServer: unable to subscribe to " << ROSTOPICNAME_MAPMETADATA << " topic, check your yarp-ROS network configuration";
                return false;
            }
            m_rosSubscriberPort_map.setStrict();
            m_rosSubscriberPort_metamap.setStrict();

        }
        // m_rosPublisherPort_markers.topic("/locationServerMarkers");
    }
    else
    {
        //no ROS options
    }
    //yarp::os::Time::delay(5);
    yarp::rosmsg::nav_msgs::OccupancyGrid*   map_ros = nullptr;
    yarp::rosmsg::nav_msgs::MapMetaData*     metamap_ros = nullptr;

    map_ros = m_rosSubscriberPort_map.read(true);
    metamap_ros = m_rosSubscriberPort_metamap.read(true);
    if (map_ros!=nullptr && metamap_ros!=nullptr)
    {
        yInfo() << "Received map for ROS";
        string map_name = "ros_map";
        yarp::dev::MapGrid2D map;
        map.setSize_in_cells(map_ros->info.width,map_ros->info.height);
        map.setResolution( map_ros->info.resolution);
        map.setMapName(map_name);
        yarp::math::Quaternion quat(map_ros->info.origin.orientation.x,
                                    map_ros->info.origin.orientation.y,
                                    map_ros->info.origin.orientation.z,
                                    map_ros->info.origin.orientation.w);
        yarp::sig::Matrix mat=quat.toRotationMatrix4x4();
        yarp::sig::Vector vec=yarp::math::dcm2rpy(mat);
        double orig_angle = vec[2];
        map.setOrigin(map_ros->info.origin.position.x,map_ros->info.origin.position.y,orig_angle);
        for (size_t y=0; y< map_ros->info.height; y++)
            for (size_t x=0; x< map_ros->info.width; x++)
            {
               MapGrid2D::XYCell cell(x,map_ros->info.height-1-y);
               double occ = map_ros->data[x+y*map_ros->info.width];
               map.setOccupancyData(cell,occ);
             
               if      (occ >= 0   && occ <= 70)  map.setMapFlag(cell, MapGrid2D::MAP_CELL_FREE);
               else if (occ >= 71 && occ <= 100)  map.setMapFlag(cell, MapGrid2D::MAP_CELL_WALL);
               else                               map.setMapFlag(cell, MapGrid2D::MAP_CELL_UNKNOWN);
            }
        auto p = m_maps_storage.find(map_name);
        if (p == m_maps_storage.end())
        {
            yInfo() << "Added map "<< map_name <<" to mapServer";
            m_maps_storage[map_name] = map;
        }
    }
    return true;
}

bool Map2DServer::close()
{
    if (m_enable_publish_ros_map)
    {
        m_rosPublisherPort_map.interrupt();
        m_rosPublisherPort_metamap.interrupt();
        m_rosPublisherPort_map.close();
        m_rosPublisherPort_metamap.close();
    }
    if (m_enable_subscribe_ros_map)
    {
        m_rosSubscriberPort_map.interrupt();
        m_rosSubscriberPort_metamap.interrupt();
        m_rosSubscriberPort_map.close();
        m_rosSubscriberPort_metamap.close();
    }
    yTrace("Map2DServer::Close");
    return true;
}

bool Map2DServer::load_locations(std::string locations_file)
{
    std::ifstream file;
    file.open (locations_file.c_str());

    if(!file.is_open())
    {
        yError() << "Unable to open" << locations_file << "locations file.";
        return false;
    }

    std::string     buffer, name, mapId, xpos, ypos, theta;
    Map2DLocation   location;

    while(!file.eof())
    {
        std::getline(file, buffer);
        std::istringstream iss(buffer);

        iss >> name >> mapId >> xpos >> ypos >> theta;

        location.map_id  = mapId;
        location.x       = std::atof(xpos.c_str());
        location.y       = std::atof(ypos.c_str());
        location.theta   = std::atof(theta.c_str());
        m_locations_storage[name] = location;
    }

    file.close();
    yDebug() << "Locations file" << locations_file << "loaded.";
    return true;
}

bool Map2DServer::save_locations(std::string locations_file)
{
    std::ofstream file;
    file.open (locations_file.c_str());

    if(!file.is_open())
    {
        yError() << "Unable to open" << locations_file << "locations file.";
        return false;
    }

    std::string     s;
    Map2DLocation   l;
    s = " ";

    std::map<std::string, Map2DLocation>::iterator it;
    for (it = m_locations_storage.begin(); it != m_locations_storage.end(); ++it)
    {
        l = it->second;
        file << it->first << s << l.map_id << s << l.x << s << l.y << s << l.theta << "\n";
    }

    file.close();
    yDebug() << "Locations file" << locations_file << "saved.";
    return true;
}

bool Map2DServer::updateVizMarkers()
{
    yarp::rosmsg::TickDuration dur; dur.sec = 0xFFFFFFFF;
    double yarpTimeStamp = yarp::os::Time::now();
    uint64_t time;
    uint64_t nsec_part;
    uint64_t sec_part;
    yarp::rosmsg::TickTime ret;
    time = (uint64_t)(yarpTimeStamp * 1000000000UL);
    nsec_part = (time % 1000000000UL);
    sec_part = (time / 1000000000UL);
    if (sec_part > std::numeric_limits<unsigned int>::max())
    {
        yWarning() << "Timestamp exceeded the 64 bit representation, resetting it to 0";
        sec_part = 0;
    }

    yarp::rosmsg::visualization_msgs::Marker marker;
    yarp::rosmsg::TickTime    tt;
    yarp::sig::Vector         rpy(3);
    yarp::math::Quaternion    q;

    yarp::rosmsg::visualization_msgs::MarkerArray& markers = m_rosPublisherPort_markers.prepare();
    markers.markers.clear();

    std::map<std::string, Map2DLocation>::iterator it;
    int count = 1;
    for (it = m_locations_storage.begin(); it != m_locations_storage.end(); ++it, ++count)
    {
        rpy[0] = 0; //x
        rpy[1] = 0; //y
        rpy[2] = it->second.theta / 180 * 3.14159265359; //z
        yarp::sig::Matrix m = yarp::math::rpy2dcm(rpy);
        q.fromRotationMatrix(m);

        marker.header.frame_id    = "map";
        tt.sec                    = (yarp::os::NetUint32) sec_part;;
        tt.nsec                   = (yarp::os::NetUint32) nsec_part;;
        marker.header.stamp       = tt;
        marker.ns                 = "my_namespace";
        marker.id                 = count;
        marker.type               = yarp::rosmsg::visualization_msgs::Marker::ARROW;
        marker.action             = yarp::rosmsg::visualization_msgs::Marker::ADD;
        marker.pose.position.x    = it->second.x;
        marker.pose.position.y    = it->second.y;
        marker.pose.position.z    = 0;
        marker.pose.orientation.x = q.x();
        marker.pose.orientation.y = q.y();
        marker.pose.orientation.z = q.z();
        marker.pose.orientation.w = q.w();
        marker.scale.x            = 1;
        marker.scale.y            = 0.1;
        marker.scale.z            = 0.1;
        marker.color.a            = 1.0;
        marker.color.r            = 0.0;
        marker.color.g            = 1.0;
        marker.color.b            = 0.0;
        marker.lifetime           = dur;
        marker.text               = it->first;
        markers.markers.push_back(marker);
    }

    m_rosPublisherPort_markers.write();
    return true;
}

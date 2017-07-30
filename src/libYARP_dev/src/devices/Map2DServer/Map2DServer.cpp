/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <sstream>
#include <limits>
#include "Map2DServer.h"
#include <yarp/dev/IMap2D.h>
#include <yarp/dev/INavigation2D.h>
#include <yarp/math/Math.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/LockGuard.h>
#include <stdlib.h>
#include <fstream>
#include <yarp/os/Publisher.h>
#include <yarp/os/Subscriber.h>
#include <yarp/os/Node.h>

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
    m_enable_publish_ros_tf = false;
    m_enable_subscribe_ros_tf = false;
    m_rosNode = 0;
}

Map2DServer::~Map2DServer()
{

}

void Map2DServer::parse_vocab_command(yarp::os::Bottle& in, yarp::os::Bottle& out)
{
    int code = in.get(0).asVocab();
    bool ret = false;
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
                    //the map alreay exists
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

            for (auto it = m_maps_storage.begin(); it != m_maps_storage.end(); ++it)
            {
                out.addString(it->first);
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
            yarp::os::ConstString info;

            out.addVocab(VOCAB_OK);
            Bottle& l = out.addList();

            std::map<std::string, Map2DLocation>::iterator it;
            for (it = m_locations.begin(); it != m_locations.end(); ++it)
            {
                l.addString(it->first);
            }
            yInfo() << "The following locations are currently stored in the server: "<<l.toString();
            ret = true;
        }
        else if (cmd == VOCAB_NAV_CLEAR)
        {
            m_locations.clear();
            yInfo() << "All locations deleted ";
            out.addVocab(VOCAB_OK);
            ret = true;
        }
        else if (cmd == VOCAB_NAV_DELETE)
        {
            std::string name = in.get(2).asString();

            std::map<std::string, Map2DLocation>::iterator it;
            it = m_locations.find(name);
            if (it != m_locations.end())
            {
                yInfo() << "Deleted location " << name;
                m_locations.erase(it);
                out.addVocab(VOCAB_OK);
            }
            else
            {
                yError("User requested an invalid location name");
                out.addVocab(VOCAB_ERR);
            }

            ret = true;
        }
        else if (cmd == VOCAB_NAV_GET_LOCATION)
        {
            std::string name = in.get(2).asString();

            std::map<std::string, Map2DLocation>::iterator it;
            it = m_locations.find(name);
            if (it != m_locations.end())
            {
                out.addVocab(VOCAB_OK);
                Map2DLocation loc = it->second;
                yInfo() << "Retrieved location " << name << "at " << loc.toString();
                out.addString(loc.map_id);
                out.addDouble(loc.x);
                out.addDouble(loc.y);
                out.addDouble(loc.theta);
            }
            else
            {
                out.addVocab(VOCAB_ERR);
                yError("User requested an invalid location name");
            }

            ret = true;
        }
        else if (cmd == VOCAB_NAV_STORE_ABS)
        {
            Map2DLocation         location;
            yarp::os::ConstString name = in.get(2).asString();

            location.map_id = in.get(3).asString();
            location.x      = in.get(4).asDouble();
            location.y      = in.get(5).asDouble();
            location.theta  = in.get(6).asDouble();

            m_locations.insert(std::pair<std::string, Map2DLocation>(name, location));
            yInfo() << "Added location " << name << "at " << location.toString();
            out.addVocab(VOCAB_OK);
            ret = true;
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
    }
    else if (in.get(0).asString() == "load_locations" && in.get(1).isString())
    {
        if(load_locations(in.get(1).asString()))
        {
            out.addString(in.get(1).asString() + " successfully loaded");
        }
    }
    else if(in.get(0).asString() == "list_locations")
    {
        std::map<std::string, Map2DLocation>::iterator it;
        for (it = m_locations.begin(); it != m_locations.end(); ++it)
        {
            out.addString(it->first);
        }
    }
    else if(in.get(0).asString() == "help")
    {
        out.addString("'save_locations <full path filename>' to save locations on a file");
        out.addString("'load_locations <full path filename>' to load locations from a file");
        out.addString("'list_locations' to view a list of all stored locations");
        out.addString("'save_maps <full path>' to save maps on in a folder");
        out.addString("'load_maps <full path>' to load maps from a folder");
        out.addString("'list_maps' to view a list of all stored maps");

    }
    else
    {
        out.addString("request not undestood, call 'help' to see a list of avaiable commands");
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
    if (returnToSender != NULL)
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
    for (auto it = m_maps_storage.begin(); it != m_maps_storage.end(); ++it)
    {
        string map_filename = it->first + ".yaml";
        file << "mapfile: ";
        file << it->first + ".yaml";
        ret &= it->second.saveToFile(map_filename);
    }
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
            string mapfilenameWithPath = m_rf_mapCollection.findFile(mapfilename.c_str());
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

    return true;
}

bool Map2DServer::open(yarp::os::Searchable &config)
{
    Property params;
    params.fromString(config.toString().c_str());

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
            yInfo() << "Map collection file:" << collection_file_with_path << "succesfully loaded.";
            yInfo() << "Available maps are:";
            for (auto it = m_maps_storage.begin(); it != m_maps_storage.end(); ++it)
            {
                yInfo() << it->first;
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
        m_rpcPortName = config.find("name").asString().c_str();
    }

    //open rpc port
    if (!m_rpcPort.open(m_rpcPortName.c_str()))
    {
        yError("Map2DServer: failed to open port %s", m_rpcPortName.c_str());
        return false;
    }
    m_rpcPort.setReader(*this);

    //ROS configuration
#if 0
    if (!config.check("ROS"))
    {
        yError() << "Map2DServer: Missing ROS group";
        return false;
    }
    Bottle ROS_config = config.findGroup("ROS");
    if (ROS_config.check("enable_ros_publisher") == false)
    {
        yError() << "Map2DServer: Missing 'enable_ros_publisher' in ROS group";
        return false;
    }
    if (ROS_config.find("enable_ros_publisher").asInt() == 1 || ROS_config.find("enable_ros_publisher").asString() == "true")
    {
        m_enable_publish_ros_tf = true;
        yInfo() << "Map2DServer: Enabled ROS publisher";
    }
    if (ROS_config.check("enable_ros_subscriber") == false)
    {
        yError() << "Map2DServer: Missing 'enable_ros_subscriber' in ROS group";
        return false;
    }
    if (ROS_config.find("enable_ros_subscriber").asInt() == 1 || ROS_config.find("enable_ros_subscriber").asString() == "true")
    {
        m_enable_subscribe_ros_tf = true;
        yInfo() << "Map2DServer: Enabled ROS subscriber";
    }

    m_rosPublisherPort_markers.topic("/locationServerMarkers");
#endif

    return true;
}

bool Map2DServer::close()
{
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
        m_locations[name] = location;
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
    for (it = m_locations.begin(); it != m_locations.end(); ++it)
    {
        l = it->second;
        file << it->first + s + l.map_id + s << l.x << s << l.y << s << l.theta << "\n";
    }

    file.close();
    yDebug() << "Locations file" << locations_file << "saved.";
    return true;
}

bool Map2DServer::updateVizMarkers()
{
    TickDuration dur; dur.sec = 0xFFFFFFFF;
    double yarpTimeStamp = yarp::os::Time::now();
    uint64_t time;
    uint64_t nsec_part;
    uint64_t sec_part;
    TickTime ret;
    time = (uint64_t)(yarpTimeStamp * 1000000000UL);
    nsec_part = (time % 1000000000UL);
    sec_part = (time / 1000000000UL);
    if (sec_part > std::numeric_limits<unsigned int>::max())
    {
        yWarning() << "Timestamp exceeded the 64 bit representation, resetting it to 0";
        sec_part = 0;
    }

    visualization_msgs_Marker marker;
    TickTime                  tt;
    yarp::sig::Vector         rpy(3);
    yarp::math::Quaternion    q;

    visualization_msgs_MarkerArray& markers = m_rosPublisherPort_markers.prepare();
    markers.markers.clear();

    std::map<std::string, Map2DLocation>::iterator it;
    int count = 1;
    for (it = m_locations.begin(); it != m_locations.end(); ++it, ++count)
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
        marker.type               = visualization_msgs_Marker::ARROW;
        marker.action             = visualization_msgs_Marker::ADD;
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
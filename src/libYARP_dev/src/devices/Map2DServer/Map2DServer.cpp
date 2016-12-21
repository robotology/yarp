/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <sstream>
#include <limits>
#include "Map2DServer.h"
#include <yarp/dev/IMap2D.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/LockGuard.h>
#include <stdlib.h>
#include <fstream>

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
  * Helper functions
  */

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

bool Map2DServer::read(yarp::os::ConnectionReader& connection)
{
    LockGuard lock(m_mutex);
    yarp::os::Bottle in;
    yarp::os::Bottle out;
    bool ok = in.read(connection);
    if (!ok) return false;

    string request = in.get(0).asString();

    // parse in, prepare out
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
                auto it = m_maps_storage.find(the_map.m_map_name);
                if (it == m_maps_storage.end())
                {
                    //add a new map
                    m_maps_storage[the_map.m_map_name] = the_map;
                    out.clear();
                    out.addVocab(VOCAB_IMAP_OK);
                }
                else
                {
                    //the map alreay exists
                    m_maps_storage[the_map.m_map_name] = the_map;
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
                yError() << "Map not found";
            }
        }
        else if (cmd == VOCAB_IMAP_GET_NAMES)
        {
            out.clear();
            out.addVocab(VOCAB_IMAP_OK);

            for (auto it = m_maps_storage.begin(); it != m_maps_storage.end(); it++)
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
        else
        {
            yError("Invalid vocab received in Map2DServer");
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

    yarp::os::ConnectionWriter *returnToSender = connection.getWriter();
    if (returnToSender != NULL)
    {
        out.write(*returnToSender);
    }
    else
    {
        yError() << "FrameTransformServer: invalid return to sender";
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
    for (auto it = m_maps_storage.begin(); it != m_maps_storage.end(); it++)
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
    std::ifstream file;
    file.open(mapsfile.c_str());
    if (!file.is_open())
    {
        yError() << "sorry unable to open" << mapsfile;
        return false;
    }
    while (!file.eof())
    {
        string dummy;
        string buffer;
        std::getline(file, buffer);
        std::istringstream iss(buffer);
        iss >> dummy;
        if (dummy == "mapfile:")
        {
            string mapfilename;
            iss >> mapfilename;
            yarp::sig::MapGrid2D map;
            bool r = map.loadFromFile(mapfilename);
            if (r)
            { 
                auto p = m_maps_storage.find(map.m_map_name);
                if (p == m_maps_storage.end())
                {
                    m_maps_storage[map.m_map_name] = map;
                }
                else
                {
                    yError() << "A map with the same name '" << map.m_map_name << "'was found, skipping...";
                }
            }
            else
            {
                yError() << "Problems opening map file" << mapfilename;
            }
        }
        else
        {
            yError() << "Invalid syntax, missing mapfile tag";
        }
    }
    file.close();
    return true;
}

bool Map2DServer::open(yarp::os::Searchable &config)
{
    yarp::os::Value d;
    d.fromString("1.1");
    if (d.asDouble() == 1.1)
    {
        bool s = true;
    }

    Property params;
    params.fromString(config.toString().c_str());

    if (config.check("mapCollection"))
    {
        loadMaps(config.find("mapCollection").asString());
    }


    std::string name;
    if (!config.check("name"))
    {
        name = "mapServer";
    }
    else
    {
        name = config.find("name").asString().c_str();
    }
    m_rpcPortName = "/" + name + "/rpc";

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
#endif

    return true;
}

bool Map2DServer::close()
{
    yTrace("Map2DServer::Close");
    return true;
}

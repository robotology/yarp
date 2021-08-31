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
    bool b = m_RPC.read(connection);
    if (b)
    {
        return true;
    }
    else
    {
        yCDebug(MAP2D_NWS_YARP) << "read() Command failed";
        return false;
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    yarp::os::Bottle in;
    yarp::os::Bottle out;
    bool ok = in.read(connection);
    if (!ok) {
        return false;
    }

    //parse string command
    if(in.get(0).isString())
    {
        parse_string_command(in, out);
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
        m_RPC.setInterface(m_iMap2D);
    }

    if (nullptr == m_iMap2D)
    {
        yCError(MAP2D_NWS_YARP, "Subdevice passed to attach method is invalid");
        return false;
    }

    return true;
}

/*
 * Copyright (C) 2016 Istituto Italiano di Tecnologia (IIT)
 * Author: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LICENSE
 */

/*! \file Map2DClient.cpp */

#include "Map2DClient.h"
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/LockGuard.h>
#include <yarp/dev/INavigation2D.h>

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;

//------------------------------------------------------------------------------------------------------------------------------

bool yarp::dev::Map2DClient::open(yarp::os::Searchable &config)
{
    m_local_name.clear();
    m_map_server.clear();

    m_local_name       = config.find("local").asString().c_str();
    m_map_server       = config.find("remote").asString().c_str();

    if (m_local_name == "")
    {
        yError("Map2DClient::open() error you have to provide valid local name");
        return false;
    }
    if (m_map_server == "")
    {
        yError("Map2DClient::open() error you have to provide valid remote name");
        return false;
    }

    ConstString local_rpc1 = m_local_name;
    local_rpc1 += "/mapClient_rpc";

    ConstString remote_rpc1 = m_map_server;
    remote_rpc1 += "/rpc";

    if (!m_rpcPort_to_Map2DServer.open(local_rpc1.c_str()))
    {
        yError("Map2DClient::open() error could not open rpc port %s, check network", local_rpc1.c_str());
        return false;
    }

    bool ok=false;
    ok=Network::connect(local_rpc1.c_str(), remote_rpc1.c_str());
    if (!ok)
    {
        yError("Map2DClient::open() error could not connect to %s", remote_rpc1.c_str());
        return false;
    }

    return true;
}

bool yarp::dev::Map2DClient::store_map(const MapGrid2D& map)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_IMAP);
    b.addVocab(VOCAB_IMAP_SET_MAP);
    yarp::os::Bottle& mapbot = b.addList();
    MapGrid2D maptemp = map;
    if (Property::copyPortable(maptemp, mapbot) == false)
    {
        yError() << "Map2DClient::store_map() failed copyPortable()";
        return false;
    }
    //yDebug() << b.toString();
    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_IMAP_OK)
        {
            yError() << "Map2DClient::store_map() received error from server";
            return false;
        }
    }
    else
    {
        yError() << "Map2DClient::store_map() error on writing on rpc port";
        return false;
    }
    return true;
}

bool yarp::dev::Map2DClient::get_map(std::string map_name, MapGrid2D& map)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_IMAP);
    b.addVocab(VOCAB_IMAP_GET_MAP);
    b.addString(map_name);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_IMAP_OK)
        {
            yError() << "Map2DClient::get_map() received error from server";
            return false;
        }
        else
        {
            Value& b = resp.get(1);
            if (Property::copyPortable(b, map))
            {
                return true;
            }
            else
            {
                yError() << "Map2DClient::get_map() failed copyPortable()";
                return false;
            }
        }
    }
    else
    {
        yError() << "Map2DClient::get_map() error on writing on rpc port";
        return false;
    }
    return true;
}

bool yarp::dev::Map2DClient::clear()
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_IMAP);
    b.addVocab(VOCAB_IMAP_CLEAR);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_IMAP_OK)
        {
            yError() << "Map2DClient::clear() received error from server";
            return false;
        }
    }
    else
    {
        yError() << "Map2DClient::clear() error on writing on rpc port";
        return false;
    }
    return true;
}

bool yarp::dev::Map2DClient::get_map_names(std::vector<std::string>& map_names)
{
    map_names.clear();
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_IMAP);
    b.addVocab(VOCAB_IMAP_GET_NAMES);
    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_IMAP_OK)
        {
            yError() << "Map2DClient::get_map_names() received error from server";
            return false;
        }
        else
        {
            for (int i = 1; i < resp.size(); i++)
            {
                map_names.push_back(resp.get(i).asString());
            }
            return true;
        }
    }
    else
    {
        yError() << "Map2DClient::get_map_names() error on writing on rpc port";
        return false;
    }
    return true;
}

bool yarp::dev::Map2DClient::remove_map(std::string map_name)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_IMAP);
    b.addVocab(VOCAB_IMAP_REMOVE);
    b.addString(map_name);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_IMAP_OK)
        {
            yError() << "Map2DClient::remove_map() received error from server";
            return false;
        }
    }
    else
    {
        yError() << "Map2DClient::remove_map() error on writing on rpc port";
        return false;
    }
    return true;
}

bool yarp::dev::Map2DClient::storeLocation(yarp::os::ConstString location_name, Map2DLocation loc)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_STORE_ABS);
    b.addString(location_name);
    b.addString(loc.map_id);
    b.addDouble(loc.x);
    b.addDouble(loc.y);
    b.addDouble(loc.theta);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::storeLocation() received error from locations server";
            return false;
        }
    }
    else
    {
        yError() << "Navigation2DClient::storeLocation() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   yarp::dev::Map2DClient::getLocationsList(std::vector<yarp::os::ConstString>& locations)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_GET_LOCATION_LIST);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::getLocationsList() received error from locations server";
            return false;
        }
        else
        {
            Bottle* list = resp.get(1).asList();
            if (list)
            {
                locations.clear();
                for (int i = 0; i < list->size(); i++)
                {
                    locations.push_back(list->get(i).asString());
                }
                return true;
            }
            else
            {
                yError() << "Navigation2DClient::getLocationsList() error while reading from locations server";
                return false;
            }
        }
    }
    else
    {
        yError() << "Navigation2DClient::getLocationsList() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   yarp::dev::Map2DClient::getLocation(yarp::os::ConstString location_name, Map2DLocation& loc)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_GET_LOCATION);
    b.addString(location_name);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::getLocation() received error from locations server";
            return false;
        }
        else
        {
            loc.map_id = resp.get(1).asString();
            loc.x = resp.get(2).asDouble();
            loc.y = resp.get(3).asDouble();
            loc.theta = resp.get(4).asDouble();
        }
    }
    else
    {
        yError() << "Navigation2DClient::getLocation() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   yarp::dev::Map2DClient::deleteLocation(yarp::os::ConstString location_name)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_DELETE);
    b.addString(location_name);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::deleteLocation() received error from locations server";
            return false;
        }
    }
    else
    {
        yError() << "Navigation2DClient::deleteLocation() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   yarp::dev::Map2DClient::clearAllLocations()
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_CLEAR);

    bool ret = m_rpcPort_to_Map2DServer.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::clearAllLocations() received error from locations server";
            return false;
        }
    }
    else
    {
        yError() << "Navigation2DClient::clearAllLocations() error on writing on rpc port";
        return false;
    }
    return true;
}

bool yarp::dev::Map2DClient::close()
{
    return true;
}

yarp::dev::DriverCreator *createMap2DClient()
{
    return new DriverCreatorOf<Map2DClient>
               (
                   "map2DClient",
                   "",
                   "yarp::dev::Map2DClient"
               );
}

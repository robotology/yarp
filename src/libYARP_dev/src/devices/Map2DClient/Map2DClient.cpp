/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "Map2DClient.h"
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/LockGuard.h>

/*! \file Map2DClient.cpp */

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;

//------------------------------------------------------------------------------------------------------------------------------

bool yarp::dev::Map2DClient::open(yarp::os::Searchable &config)
{
    m_local_name.clear();
    m_remote_name.clear();

    m_local_name  = config.find("local").asString().c_str();
    m_remote_name = config.find("remote").asString().c_str();

    if (m_local_name == "")
    {
        yError("Map2DClient::open() error you have to provide valid local name");
        return false;
    }
    if (m_remote_name == "")
    {
        yError("Map2DClient::open() error you have to provide valid remote name");
        return false;
    }

    ConstString local_rpc = m_local_name;
    local_rpc += "/rpc";
    ConstString remote_rpc = m_remote_name;
    remote_rpc += "/rpc";

    if (!m_rpcPort.open(local_rpc.c_str()))
    {
        yError("Map2DClient::open() error could not open rpc port %s, check network", local_rpc.c_str());
        return false;
    }

    bool ok=Network::connect(local_rpc.c_str(), remote_rpc.c_str());
    if (!ok)
    {
        yError("Map2DClient::open() error could not connect to %s", remote_rpc.c_str());
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
    bool ret = m_rpcPort.write(b, resp);
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

    bool ret = m_rpcPort.write(b, resp);
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

    bool ret = m_rpcPort.write(b, resp);
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
    bool ret = m_rpcPort.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_IMAP_OK)
        {
            yError() << "Map2DClient::get_map_names() received error from server";
            return false;
        }
        else
        {
            for (size_t i = 1; i < resp.size(); i++)
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

    bool ret = m_rpcPort.write(b, resp);
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

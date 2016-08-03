/*
* Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
* Author: Marco Randazzo <marco.randazzo@iit.it>
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/

#include <yarp/dev/INavigation2D.h>
#include "LocationsServer.h"
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/LockGuard.h>

/*! \file LocationsServer.cpp */

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;


//------------------------------------------------------------------------------------------------------------------------------
bool yarp::dev::LocationsServer::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::Bottle in;
    yarp::os::Bottle out;
    bool ok = in.read(connection);
    if (!ok) return false;

    // parse in, prepare out
    int code = in.get(0).asVocab();
    bool ret = false;
    if (code == VOCAB_INAVIGATION)
    {
        int cmd = in.get(1).asVocab();
        if (cmd == VOCAB_NAV_GET_LOCATION_LIST)
        {
            if (1)
            {
                yarp::os::ConstString info;
                
                out.addVocab(VOCAB_IS);
                out.addVocab(cmd);
                out.addString(info);
                ret = true;
            }
        }
        else if (cmd == VOCAB_NAV_CLEAR)
        {
            m_locations.clear();
            out.addVocab(VOCAB_OK);
            ret = true;
        }
        else if (cmd == VOCAB_NAV_DELETE)
        {
        }
        else if (cmd == VOCAB_NAV_GET_LOCATION)
        {
            std::string name = in.get(2).asString();
            Map2DLocation loc = m_locations[name];
            out.addString(loc.map_id);
            out.addDouble(loc.x);
            out.addDouble(loc.y);
            out.addDouble(loc.theta);
            ret = true;
        }
        else if (cmd == VOCAB_NAV_STORE_ABS)
        {

        }
        else
        {
            yError("Invalid vocab received in BatteryWrapper");
        }
    }
    else
    {
        yError("Invalid vocab received in BatteryWrapper");
    }

    if (!ret)
    {
        out.clear();
        out.addVocab(VOCAB_FAILED);
    }

    yarp::os::ConnectionWriter *returnToSender = connection.getWriter();
    if (returnToSender != NULL)
    {
        out.write(*returnToSender);
    }
    return true;
}

bool yarp::dev::LocationsServer::open(yarp::os::Searchable &config)
{
    m_local_name.clear();
    m_local_name  = config.find("local").asString().c_str();
    
    if (m_local_name == "")
    {
        yError("LocationsServer::open() error you have to provide valid local name");
        return false;
    }
    
    if (config.check("period"))
    {
        m_period = config.find("period").asInt();
    }
    else
    {
        m_period = 10;
        yWarning("LocationsServer: using default period of %d ms" , m_period);
    }

    ConstString local_rpc = m_local_name;
    local_rpc += "/rpc";
    
    if (!m_rpc_port.open(local_rpc.c_str()))
    {
        yError("LocationsServer::open() error could not open rpc port %s, check network", local_rpc.c_str());
        return false;
    }

    m_rpc_port.setReader(*this);
    return true;
}

bool yarp::dev::LocationsServer::close()
{
    m_rpc_port.interrupt();
    m_rpc_port.close();
    return true;
}

yarp::dev::DriverCreator *createLocationsServer()
{
    return new DriverCreatorOf<LocationsServer>
               (
                   "locationsServer",
                   "",
                   "locationsServer"
               );
}

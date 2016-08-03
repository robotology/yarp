/*
* Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
* Author: Marco Randazzo <marco.randazzo@iit.it>
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/

#include <yarp/dev/INavigation2D.h>
#include "Navigation2DClient.h"
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/LockGuard.h>

/*! \file Navigation2DClient.cpp */

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;


//------------------------------------------------------------------------------------------------------------------------------

bool yarp::dev::Navigation2DClient::open(yarp::os::Searchable &config)
{
    m_local_name.clear();
    m_remote_name.clear();

    m_local_name  = config.find("local").asString().c_str();
    m_remote_name = config.find("remote").asString().c_str();

    if (m_local_name == "")
    {
        yError("Navigation2DClient::open() error you have to provide valid local name");
        return false;
    }
    if (m_remote_name == "")
    {
        yError("Navigation2DClient::open() error you have to provide valid remote name");
        return false;
    }

    if (config.check("period"))
    {
        m_period = config.find("period").asInt();
    }
    else
    {
        m_period = 10;
        yWarning("Navigation2DClient: using default period of %d ms" , m_period);
    }

    ConstString local_rpc = m_local_name;
    local_rpc += "/rpc";
    ConstString remote_rpc = m_remote_name;
    remote_rpc += "/rpc";
    ConstString remote_streaming_name = m_remote_name;
    remote_streaming_name += "/stream:o";
    ConstString local_streaming_name = m_local_name;
    local_streaming_name += "/stream:i";

    if (!m_rpc_port_navigation_server.open(local_rpc.c_str()))
    {
        yError("Navigation2DClient::open() error could not open rpc port %s, check network", local_rpc.c_str());
        return false;
    }

    /*
    //currently unused
    bool ok=Network::connect(remote_streaming_name.c_str(), local_streaming_name.c_str(), "tcp");
    if (!ok)
    {
        yError("Navigation2DClient::open() error could not connect to %s", remote_streaming_name.c_str());
        return false;
    }*/

    bool ok = Network::connect(local_rpc.c_str(), remote_rpc.c_str());
    if (!ok)
    {
        yError("Navigation2DClient::open() error could not connect to %s", remote_rpc.c_str());
        return false;
    }

    return true;
}

bool yarp::dev::Navigation2DClient::close()
{
    m_rpc_port_navigation_server.close();

    return true;
}

bool yarp::dev::Navigation2DClient::getNavigationStatus(NavigationStatusEnum& status)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_GET_STATUS);
    bool ret = m_rpc_port_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::gotoAbsPos() recived error from server";
            return false;
        }
        else
        {
            status = (NavigationStatusEnum) b.get(1).asInt();
            return true;
        }
    }
    else
    {
        yError() << "Navigation2DClient::gotoAbsPos() error on writing on rpc port";
        return false;
    }
    return true;
}


bool yarp::dev::Navigation2DClient::gotoTargetByAbsoluteLocation(Map2DLocation loc)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_GOTOABS);
    b.addString(loc.map_id);
    b.addDouble(loc.x);
    b.addDouble(loc.y);
    b.addDouble(loc.theta);
    bool ret = m_rpc_port_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::gotoAbsPos() recived error from server";
            return false;
        }
    }
    else
    {
        yError() << "Navigation2DClient::gotoAbsPos() error on writing on rpc port";
        return false;
    }
    return true;
}

bool yarp::dev::Navigation2DClient::gotoTargetByLocationName(yarp::os::ConstString location_name)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_GOTOLOC);
    b.addString(location_name);

    bool ret = m_rpc_port_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::gotoLocation() recived error from server";
            return false;
        }
    }
    else
    {
        yError() << "Navigation2DClient::gotoLocation() error on writing on rpc port";
        return false;
    }
    return true;
}

bool yarp::dev::Navigation2DClient::gotoTargetByRelativeLocation(double x, double y, double theta)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_GOTOREL);
    b.addDouble(x);
    b.addDouble(y);
    b.addDouble(theta);

    bool ret = m_rpc_port_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::gotoRelPos() recived error from server";
            return false;
        }
    }
    else
    {
        yError() << "Navigation2DClient::gotoRelPos() error on writing on rpc port";
        return false;
    }
    return true;
}

bool  yarp::dev::Navigation2DClient::getCurrentPosition(Map2DLocation loc)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_GET_CURRENT_POS);

    bool ret = m_rpc_port_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::getCurrentPosition() recived error from server";
            return false;
        }
        else
        {
            loc.map_id = b.get(1).asString();
            loc.x = b.get(2).asDouble();
            loc.y = b.get(3).asDouble();
            loc.theta = b.get(4).asDouble();
            return true;
        }
    }
    else
    {
        yError() << "Navigation2DClient::getCurrentPosition() error on writing on rpc port";
        return false;
    }
    return true;
}

bool yarp::dev::Navigation2DClient::suspendNavigation()
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_SUSPEND);

    bool ret = m_rpc_port_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::suspend() recived error from server";
            return false;
        }
    }
    else
    {
        yError() << "Navigation2DClient::suspend() error on writing on rpc port";
        return false;
    }
    return true;
}

bool yarp::dev::Navigation2DClient::getAbsoluteLocationOfCurrentTarget(Map2DLocation loc)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_GET_ABS_TARGET);

    bool ret = m_rpc_port_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::getAbsoluteLocationOfCurrentTarget() recived error from server";
            return false;
        }
        else
        {
            loc.map_id = b.get(1).asString();
            loc.x = b.get(2).asDouble();
            loc.y = b.get(3).asDouble();
            loc.theta = b.get(4).asDouble();
            return true;
        }
    }
    else
    {
        yError() << "Navigation2DClient::getAbsoluteLocationOfCurrentTarget() error on writing on rpc port";
        return false;
    }
    return true;
}

bool yarp::dev::Navigation2DClient::getNameOfCurrentTarget(yarp::os::ConstString& location_name)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_GET_NAME_TARGET);

    bool ret = m_rpc_port_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::getNameOfCurrentTarget() recived error from server";
            return false;
        }
        else
        {
            location_name = b.get(1).asString();
            return true;
        }
    }
    else
    {
        yError() << "Navigation2DClient::getNameOfCurrentTarget() error on writing on rpc port";
        return false;
    }
    return true;
}

bool yarp::dev::Navigation2DClient::getRelativeLocationOfCurrentTarget(double& x, double& y, double& theta)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_GET_REL_TARGET);

    bool ret = m_rpc_port_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::getRelativeLocationOfCurrentTarget() recived error from server";
            return false;
        }
        else
        {
            x = b.get(1).asDouble();
            y = b.get(2).asDouble();
            theta = b.get(3).asDouble();
            return true;
        }
    }
    else
    {
        yError() << "Navigation2DClient::getRelativeLocationOfCurrentTarget() error on writing on rpc port";
        return false;
    }
    return true;
}

bool yarp::dev::Navigation2DClient::storeCurrentPosition(yarp::os::ConstString location_name)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_STORE_CURRENT);
    b.addString(location_name);

    bool ret = m_rpc_port_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::storeCurrentPosition() recived error from server";
            return false;
        }
    }
    else
    {
        yError() << "Navigation2DClient::storeCurrentPosition() error on writing on rpc port";
        return false;
    }
    return true;
}

bool yarp::dev::Navigation2DClient::storeLocation(yarp::os::ConstString location_name, Map2DLocation loc)
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

    bool ret = m_rpc_port_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::storeLocation() recived error from server";
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

bool   yarp::dev::Navigation2DClient::getLocationsList(std::vector<yarp::os::ConstString>& locations)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_GET_LOCATION_LIST);

    bool ret = m_rpc_port_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::getLocationsList() recived error from server";
            return false;
        }
        else
        {
            Bottle* list = b.get(1).asList();
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
                yError() << "Navigation2DClient::getLocationsList() error while reading from server";
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

bool   yarp::dev::Navigation2DClient::getLocation(yarp::os::ConstString location_name, Map2DLocation& loc)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_GET_LOCATION);
    b.addString(location_name);

    bool ret = m_rpc_port_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::getLocation() recived error from server";
            return false;
        }
        else
        {
            loc.map_id = b.get(1).asString();
            loc.x = b.get(1).asDouble();
            loc.y = b.get(1).asDouble();
            loc.theta = b.get(1).asDouble();
        }
    }
    else
    {
        yError() << "Navigation2DClient::getLocation() error on writing on rpc port";
        return false;
    }
    return true;
}

bool   yarp::dev::Navigation2DClient::deleteLocation(yarp::os::ConstString location_name)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_DELETE);
    b.addString(location_name);

    bool ret = m_rpc_port_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::deleteLocation() recived error from server";
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

bool   yarp::dev::Navigation2DClient::clearAllLocations()
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_CLEAR);

    bool ret = m_rpc_port_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::clearAllLocations() recived error from server";
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

bool yarp::dev::Navigation2DClient::stopNavigation()
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_STOP);

    bool ret = m_rpc_port_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::stop() recived error from server";
            return false;
        }
    }
    else
    {
        yError() << "Navigation2DClient::stop() error on writing on rpc port";
        return false;
    }
    return true;
}

bool yarp::dev::Navigation2DClient::resumeNavigation()
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_RESUME);

    bool ret = m_rpc_port_navigation_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Navigation2DClient::resume() recived error from server";
            return false;
        }
    }
    else
    {
        yError() << "Navigation2DClient::resume() error on writing on rpc port";
        return false;
    }
    return true;
}

yarp::dev::DriverCreator *createNavigation2DClient()
{
    return new DriverCreatorOf<Navigation2DClient>
               (
                   "navigation2DClient",
                   "",
                   "navigation2DClient"
               );
}

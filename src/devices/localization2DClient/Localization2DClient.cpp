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

#include "Localization2DClient.h"
#include <yarp/dev/ILocalization2D.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <mutex>

/*! \file Localization2DClient.cpp */

using namespace yarp::dev;
using namespace yarp::dev::Nav2D;
using namespace yarp::os;
using namespace yarp::sig;


//------------------------------------------------------------------------------------------------------------------------------

bool Localization2DClient::open(yarp::os::Searchable &config)
{
    m_local_name.clear();
    m_remote_name.clear();

    m_local_name = config.find("local").asString();
    m_remote_name = config.find("remote").asString();

    if (m_local_name == "")
    {
        yError("Localization2DClient::open() error you have to provide a valid 'local' param");
        return false;
    }

    if (m_remote_name == "")
    {
        yError("Localization2DClient::open() error you have to provide valid 'remote' param");
        return false;
    }

    std::string
            local_rpc,
            remote_rpc,
            remote_streaming_name,
            local_streaming_name;

    local_rpc             = m_local_name  + "/localization/rpc";
    remote_rpc            = m_remote_name + "/rpc";
    remote_streaming_name = m_remote_name + "/stream:o";
    local_streaming_name  = m_local_name  + "/stream:i";

    if (!m_rpc_port_localization_server.open(local_rpc))
    {
        yError("Localization2DClient::open() error could not open rpc port %s, check network", local_rpc.c_str());
        return false;
    }

    /*
    //currently unused
    bool ok=Network::connect(remote_streaming_name.c_str(), local_streaming_name.c_str(), "tcp");
    if (!ok)
    {
        yError("Localization2DClient::open() error could not connect to %s", remote_streaming_name.c_str());
        return false;
    }*/

    bool ok = true;

    ok = Network::connect(local_rpc, remote_rpc);
    if (!ok)
    {
        yError("Localization2DClient::open() error could not connect to %s", remote_rpc.c_str());
        return false;
    }

    return true;
}

bool Localization2DClient::setInitialPose(const Map2DLocation& loc)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_SET_INITIAL_POS);
    b.addString(loc.map_id);
    b.addFloat64(loc.x);
    b.addFloat64(loc.y);
    b.addFloat64(loc.theta);

    bool ret = m_rpc_port_localization_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Localization2DClient::setInitialPose() received error from localization server";
            return false;
        }
    }
    else
    {
        yError() << "Localization2DClient::setInitialPose() error on writing on rpc port";
        return false;
    }
    return true;
}

bool  Localization2DClient::getCurrentPosition(Map2DLocation& loc)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_GET_CURRENT_POS);

    bool ret = m_rpc_port_localization_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK || resp.size() != 5)
        {
            yError() << "Localization2DClient::getCurrentPosition() received error from localization server";
            return false;
        }
        else
        {
            loc.map_id = resp.get(1).asString();
            loc.x = resp.get(2).asFloat64();
            loc.y = resp.get(3).asFloat64();
            loc.theta = resp.get(4).asFloat64();
            return true;
        }
    }
    else
    {
        yError() << "Localization2DClient::getCurrentPosition() error on writing on rpc port";
        return false;
    }
    return true;
}

bool  Localization2DClient::getEstimatedPoses(std::vector<Map2DLocation>& poses)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_GET_LOCALIZER_POSES);

    bool ret = m_rpc_port_localization_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "Localization2DClient::getEstimatedPoses() received error from localization server";
            return false;
        }
        else
        {
            int nposes = resp.get(1).asInt32();
            poses.clear();
            for (int i = 0; i < nposes; i++)
            {
                Map2DLocation loc;
                Bottle* b = resp.get(2 + i).asList();
                if (b)
                {
                    loc.map_id = b->get(0).asString();
                    loc.x = b->get(1).asFloat64();
                    loc.y = b->get(2).asFloat64();
                    loc.theta = b->get(3).asFloat64();
                }
                else
                {
                    poses.clear();
                    yError() << "Localization2DClient::getEstimatedPoses() parsing error";
                    return false;
                }
                poses.push_back(loc);
            }
            return true;
        }
    }
    else
    {
        yError() << "Localization2DClient::getEstimatedPoses() error on writing on rpc port";
        return false;
    }
    return true;
}

bool  Localization2DClient::getLocalizationStatus(yarp::dev::LocalizationStatusEnum& status)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_INAVIGATION);
    b.addVocab(VOCAB_NAV_GET_LOCALIZER_STATUS);

    bool ret = m_rpc_port_localization_server.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK || resp.size() != 2)
        {
            yError() << "Localization2DClient::getLocalizationStatus() received error from localization server";
            return false;
        }
        else
        {
            status = (yarp::dev::LocalizationStatusEnum)(resp.get(1).asVocab());
            return true;
        }
    }
    else
    {
        yError() << "Localization2DClient::getLocalizationStatus() error on writing on rpc port";
        return false;
    }
    return true;
}

bool Localization2DClient::close()
{
    m_rpc_port_localization_server.close();
    return true;
}

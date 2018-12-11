/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
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

#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Time.h>
#include <yarp/os/Port.h>
#include <yarp/os/Mutex.h>
#include <yarp/os/LockGuard.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/Bottle.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/IMap2D.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IFrameTransform.h>
#include "Localization2DServer.h"

#include <math.h>

/*! \file Localization2DServer.cpp */

using namespace yarp::os;
using namespace yarp::dev;
using namespace std;


//------------------------------------------------------------------------------------------------------------------------------

Localization2DServer::Localization2DServer() : PeriodicThread(DEFAULT_THREAD_PERIOD)
{
    m_period = 0.010;
    m_stats_time_last = yarp::os::Time::now();
    iLoc = 0;
    m_getdata_using_periodic_thread = true;
}

bool Localization2DServer::attachAll(const PolyDriverList &device2attach)
{
    if (device2attach.size() != 1)
    {
        yError("Localization2DServer: cannot attach more than one device");
        return false;
    }

    yarp::dev::PolyDriver * Idevice2attach = device2attach[0]->poly;

    if (Idevice2attach->isValid())
    {
        Idevice2attach->view(iLoc);
    }

    if (nullptr == iLoc)
    {
        yError("Localization2DServer: subdevice passed to attach method is invalid");
        return false;
    }

    PeriodicThread::setPeriod(m_period);
    return PeriodicThread::start();
}

bool Localization2DServer::detachAll()
{
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }
    iLoc = nullptr;
    return true;
}

bool Localization2DServer::open(Searchable& config)
{
    Property params;
    params.fromString(config.toString().c_str());
    yDebug() << "Localization2DServer configuration: \n" << config.toString().c_str();

    if (config.check("GENERAL") == false)
    {
        yWarning() << "Missing GENERAL group, assuming default options";
    }

    Bottle& general_group = config.findGroup("GENERAL");
    if (!general_group.check("period"))
    {
        yInfo() << "Localization2DServer: missing 'period' parameter. Using default value: 0.010";
        m_period = 0.010;
    }
    else
    {
        m_period = general_group.find("period").asFloat64();
        yInfo() << "Localization2DServer: period requested: " << m_period;
    }
    
    if (!general_group.check("xxxxxx"))
    {
        yInfo() << "Localization2DServer: missing 'xxxxxxx' parameter. Using default value: true";
        m_getdata_using_periodic_thread = true;
    }
    else
    {
        m_getdata_using_periodic_thread = general_group.find("xxxxxx").asBool();
        if (m_getdata_using_periodic_thread) { yInfo() << "Localization2DServer: xxxxxxxx requested"; }
        else { yInfo() << "Localization2DServer: xxxxxxxx requested"; }
    }


    string local_name = "/localizationServer";
    if (!general_group.check("name"))
    {
        yInfo() << "Localization2DServer: missing 'name' parameter. Using default value: /localizationServer";
    }
    else
    {
        local_name = general_group.find("name").asString();
        if (local_name.c_str()[0] != '/') { yError() << "Missing '/' in name parameter" ;  return false; }
        yInfo() << "Localization2DServer: using local name:" << local_name;
    }

    m_rpcPortName = local_name + "/rpc";
    m_streamingPortName = local_name + "/streaming:o";

    if (!initialize_YARP(config))
    {
        yError() << "Localization2DServer: Error initializing YARP ports";
        return false;
    }

    if (config.check("subdevice"))
    {
        Property       p;
        PolyDriverList driverlist;
        p.fromString(config.toString(), false);
        p.put("device", config.find("subdevice").asString());

        if (!pLoc.open(p) || !pLoc.isValid())
        {
            yError() << "Localization2DServer: failed to open subdevice.. check params";
            return false;
        }

        driverlist.push(&pLoc, "1");
        if (!attachAll(driverlist))
        {
            yError() << "Localization2DServer: failed to open subdevice.. check params";
            return false;
        }
    }
    m_stats_time_last = yarp::os::Time::now();
    return true;
}

bool Localization2DServer::initialize_YARP(yarp::os::Searchable &params)
{
    if (!m_streamingPort.open(m_streamingPortName.c_str()))
    {
        yError("Localization2DServer: failed to open port %s", m_streamingPortName.c_str());
        return false;
    }

    if (!m_rpcPort.open(m_rpcPortName.c_str()))
    {
        yError("Localization2DServer: failed to open port %s", m_rpcPortName.c_str());
        return false;
    }
    m_rpcPort.setReader(*this);

    return true;
}

bool Localization2DServer::close()
{
    yTrace("Localization2DServer::Close");
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }

    detachAll();
    return true;
}

bool Localization2DServer::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::Bottle command;
    yarp::os::Bottle reply;
    bool ok = command.read(connection);
    if (!ok) return false;

    reply.clear();

    if (command.get(0).isVocab())
    {
        if (command.get(0).asVocab() == VOCAB_INAVIGATION && command.get(1).isVocab())
        {
            int request = command.get(1).asVocab();
            if (request == VOCAB_NAV_GET_CURRENT_POS)
            {
                if (m_getdata_using_periodic_thread)
                {
                    reply.addVocab(VOCAB_OK);
                    reply.addString(m_current_position.map_id);
                    reply.addFloat64(m_current_position.x);
                    reply.addFloat64(m_current_position.y);
                    reply.addFloat64(m_current_position.theta);
                }
                else
                {
                    yarp::dev::Map2DLocation curr_loc;
                    iLoc->getCurrentPosition(curr_loc);
                    reply.addVocab(VOCAB_OK);
                    reply.addString(curr_loc.map_id);
                    reply.addFloat64(curr_loc.x);
                    reply.addFloat64(curr_loc.y);
                    reply.addFloat64(curr_loc.theta);
                }
            }
            else if (request == VOCAB_NAV_SET_INITIAL_POS)
            {
                yarp::dev::Map2DLocation init_loc;
                init_loc.map_id = command.get(2).asString();
                init_loc.x = command.get(3).asFloat64();
                init_loc.y = command.get(4).asFloat64();
                init_loc.theta = command.get(5).asFloat64();
                iLoc->setInitialPose(init_loc);
                reply.addVocab(VOCAB_OK);
            }
            else if (request == VOCAB_NAV_GET_LOCALIZER_STATUS)
            {
                if (m_getdata_using_periodic_thread)
                {
                    reply.addVocab(VOCAB_OK);
                    reply.addVocab(m_current_status);
                }
                else
                {
                    yarp::dev::LocalizationStatusEnum status;
                    iLoc->getLocalizationStatus(status);
                    reply.addVocab(VOCAB_OK);
                    reply.addVocab(status);
                }
            }
            else if (request == VOCAB_NAV_GET_LOCALIZER_POSES)
            {
                std::vector<yarp::dev::Map2DLocation> poses;
                iLoc->getEstimatedPoses(poses);
                reply.addVocab(VOCAB_OK);
                reply.addInt32(poses.size());
                for (size_t i=0; i<poses.size(); i++)
                {
                    Bottle& b = reply.addList();
                    b.addString(poses[i].map_id);
                    b.addFloat64(poses[i].x);
                    b.addFloat64(poses[i].y);
                    b.addFloat64(poses[i].theta);
                }
            }
            else
            {
                reply.addVocab(VOCAB_ERR);
            }
        }
        else
        {
            yError() << "Invalid vocab received";
            reply.addVocab(VOCAB_ERR);
        }
    }
    else if (command.get(0).isString() && command.get(0).asString() == "help")
    {
        reply.addVocab(Vocab::encode("many"));
        reply.addString("Available commands are:");
        reply.addString("getLoc");
        reply.addString("initLoc <map_name> <x> <y> <angle in degrees>");
    }
    else if (command.get(0).isString() && command.get(0).asString() == "getLoc")
    {
        yarp::dev::Map2DLocation curr_loc;
        iLoc->getCurrentPosition(curr_loc);
        std::string s = std::string("Current Location is: ") + curr_loc.toString();
        reply.addString(s);
    }
    else if (command.get(0).isString() && command.get(0).asString() == "initLoc")
    {
        yarp::dev::Map2DLocation init_loc;
        init_loc.map_id = command.get(1).asString();
        init_loc.x = command.get(2).asFloat64();
        init_loc.y = command.get(3).asFloat64();
        init_loc.theta = command.get(4).asFloat64();
        iLoc->setInitialPose(init_loc);
        std::string s = std::string("Localization initialized to: ") + init_loc.toString();
        reply.addString(s);
    }
    else
    {
        yError() << "Invalid command type";
        reply.addVocab(VOCAB_ERR);
    }

    yarp::os::ConnectionWriter *returnToSender = connection.getWriter();
    if (returnToSender != nullptr)
    {
        reply.write(*returnToSender);
    }

    return true;
}

void Localization2DServer::run()
{
    double m_stats_time_curr = yarp::os::Time::now();
    if (m_stats_time_curr - m_stats_time_last > 5.0)
    {
        yInfo() << "Localization2DServer running";
        m_stats_time_last = yarp::os::Time::now();
    }

    Bottle& b = m_streamingPort.prepare();
    b.clear();

    bool ret = iLoc->getLocalizationStatus(m_current_status);
    if (ret==false)
    {
        yError() << "Localization2DServer: getLocalizationStatus() failed";
    }

    if (m_current_status== LocalizationStatusEnum::localization_status_localized_ok)
    {
        bool ret2 =  iLoc->getCurrentPosition(m_current_position);
        if (ret2 == false)
        {
            yError() << "Localization2DServer: getCurrentPosition() failed";
        }
        b.addString(m_current_position.map_id);
        b.addFloat64(m_current_position.x);
        b.addFloat64(m_current_position.y);
        b.addFloat64(m_current_position.theta);
    }
    else
    {
        yarp::dev::Map2DLocation curr_loc;
        curr_loc.x = std::nan("");
        curr_loc.y = std::nan("");
        curr_loc.theta = std::nan("");
        b.addString(curr_loc.map_id);
        b.addFloat64(curr_loc.x);
        b.addFloat64(curr_loc.y);
        b.addFloat64(curr_loc.theta);
    }

    m_streamingPort.write();
}

yarp::dev::DriverCreator *createLocalization2DServer()
{
    return new DriverCreatorOf<Localization2DServer>
        (
            "localization2DServer",
            "",
            "localization2DServer"
            );
}

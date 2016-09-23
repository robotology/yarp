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
#include <yarp/math/Math.h>
#include <iostream>
#include <fstream>

/*! \file LocationsServer.cpp */

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::math;


//------------------------------------------------------------------------------------------------------------------------------
bool yarp::dev::LocationsServer::updateVizMarkers()
{
    if (m_ros_enabled == false) return false;

    visualization_msgs_Marker marker;
    TickTime                  tt;
    yarp::sig::Vector         rpy(3), q(4);

    visualization_msgs_MarkerArray markers = m_rosPublisherPort.prepare();

    std::map<std::string, Map2DLocation>::iterator it;
    for (it = m_locations.begin(); it != m_locations.end(); it++)
    {
        rpy[V3_X] = 0;
        rpy[V3_Y] = 0;
        rpy[V3_Z] = it->second.theta;
        q         = dcm2quat(SE3inv(rpy2dcm(rpy)));

        marker.header.frame_id    = "map";
        tt.sec                    = 0;
        tt.nsec                   = 0;
        marker.header.stamp       = tt;
        marker.ns                 = "my_namespace";
        marker.id                 = 0;
        marker.type               = visualization_msgs_Marker::ARROW;
        marker.action             = visualization_msgs_Marker::ADD;
        marker.pose.position.x    = it->second.x;
        marker.pose.position.y    = it->second.y;
        marker.pose.position.z    = 0;
        marker.pose.orientation.x = q[V4_X];
        marker.pose.orientation.y = q[V4_Y];
        marker.pose.orientation.z = q[V4_Z];
        marker.pose.orientation.w = q[V4_W];
        marker.scale.x            = 1;
        marker.scale.y            = 0.1;
        marker.scale.z            = 0.1;
        marker.color.a            = 1.0;
        marker.color.r            = 0.0;
        marker.color.g            = 1.0;
        marker.color.b            = 0.0;

        markers.markers.push_back(marker);
    }

    m_rosPublisherPort.write();
    return true;
}

bool yarp::dev::LocationsServer::load_locations(yarp::os::ConstString locations_file)
{
    std::ifstream file;
    file.open (locations_file.c_str());

    if(!file.is_open())
    {
        yError() << "sorry unable to open" << locations_file << "locations file";
    }

    std::string     buffer, key, xpos, ypos, theta;
    Map2DLocation   location;

    while(!file.eof())
    {
        std::getline(file, buffer);
        std::istringstream iss(buffer);

        iss >> key >> xpos >> ypos >> theta;

        location.map_id  = key;
        location.x       = std::atof(xpos.c_str());
        location.y       = std::atof(ypos.c_str());
        location.theta   = std::atof(theta.c_str());
        m_locations[key] = location;
    }

    file.close();
    return true;
}

bool yarp::dev::LocationsServer::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::Bottle in;
    yarp::os::Bottle out;
    bool             ret;
    int              code;

    bool ok = in.read(connection);
    if (!ok) return false;

    // parse in, prepare out
    code = in.get(0).asVocab();
    ret  = false;

    if (code == VOCAB_INAVIGATION)
    {
        int cmd = in.get(1).asVocab();
        if (cmd == VOCAB_NAV_GET_LOCATION_LIST)
        {
            yarp::os::ConstString info;
               
            out.addVocab(VOCAB_OK);
            Bottle l = out.addList();

            std::map<std::string, Map2DLocation>::iterator it;
            for (it = m_locations.begin(); it != m_locations.end(); it++)
            {
                l.addString(it->first);
            }

            ret = true;
        }
        else if (cmd == VOCAB_NAV_CLEAR)
        {
            m_locations.clear();
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
                Map2DLocation loc = it->second;
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
        yError("Invalid vocab received in LocationsServer");
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

    updateVizMarkers();
    return true;
}

bool yarp::dev::LocationsServer::open(yarp::os::Searchable &config)
{
    m_local_name.clear();
    m_local_name  = config.find("local").asString().c_str();
    m_ros_enabled = false;

    if (m_local_name == "")
    {
        yError("LocationsServer::open() error you have to provide valid local name");
        return false;
    }
    
    if (config.check("ROS_enabled"))
    {
        m_ros_enabled = true;
        m_rosNode     = new yarp::os::Node("/LocationServer");

        m_rosPublisherPort.topic("/LocationServerMarkers");
    }

    if (config.check("locations_file"))
    {
        std::string location_file = config.find("locations_file").asString();
        bool ret                  = load_locations(location_file);

        if (ret) { yInfo() << "Location file" << location_file << "succesfully loaded."; }
        else { yError() << "Problems opening file" << location_file; }
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

    if (m_ros_enabled == true)
    {
        m_rosPublisherPort.interrupt();
        m_rosPublisherPort.close();
    }
    return true;
}

yarp::dev::DriverCreator *createLocationsServer()
{
    return new DriverCreatorOf<LocationsServer>
               (
                   "locationsServer",
                   "locationsServer",
                   "yarp::dev::LocationsServer"
               );
}

/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/dev/IRobotDescription.h>
#include "RobotDescriptionServer.h"
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/LockGuard.h>

/*! \file Navigation2DClient.cpp */

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;


//------------------------------------------------------------------------------------------------------------------------------

bool yarp::dev::RobotDescriptionServer::open(yarp::os::Searchable &config)
{
    m_local_name.clear();
    m_local_name = config.find("local").asString().c_str();

    if (m_local_name == "")
    {
        yError("RobotDescriptionServer::open() error you have to provide valid local name");
        return false;
    }

    ConstString local_rpc = m_local_name;
    local_rpc += "/rpc";

    if (!m_rpc_port.open(local_rpc.c_str()))
    {
        yError("RobotDescriptionServer::open() error could not open rpc port %s, check network", local_rpc.c_str());
        return false;
    }

    m_rpc_port.setReader(*this);
    return true;
}

bool yarp::dev::RobotDescriptionServer::attachAll(const PolyDriverList &p)
{
    for (int i = 0; i < p.size(); i++)
    {
        //yDebug() << "***************" << p[i]->poly->getOptions().toString();
        //yDebug() << "***************" << p[i]->poly->getValue("device").toString();
        //yDebug() << "***************" << p[i]->poly->getValue("name").toString();
        RobotDescription dev;
        dev.device_name = p[i]->poly->getValue("name").toString();
        dev.device_type = p[i]->poly->getValue("device").toString();
        if (this->add_device(dev) == false)
        {
            yError() << "RobotDescriptionServer::attachAll() something strange happened here";
            //return false;
        }
    }
    return true;
}

bool yarp::dev::RobotDescriptionServer::detachAll()
{
    m_robot_devices.clear();
    return true;
}

bool yarp::dev::RobotDescriptionServer::close()
{
    m_rpc_port.close();

    return true;
}

bool yarp::dev::RobotDescriptionServer::add_device(RobotDescription dev)
{
    for (std::vector<RobotDescription>::iterator it = m_robot_devices.begin(); it != m_robot_devices.end(); it++)
    {
        if (dev.device_name == it->device_name)
        {
            yWarning() << "RobotDescriptionServer::add_device() device" << dev.device_name << "already exists, skipping";
            return false;
        }
    }
    m_robot_devices.push_back(dev);
    return true;
}

bool yarp::dev::RobotDescriptionServer::remove_device(RobotDescription dev)
{
    for (std::vector<RobotDescription>::iterator it = m_robot_devices.begin(); it != m_robot_devices.end(); it++)
    {
        if (dev.device_name == it->device_name)
        {
            m_robot_devices.erase(it);
            return true;
        }
    }
    return false;
}

bool yarp::dev::RobotDescriptionServer::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::Bottle in;
    yarp::os::Bottle out;
    bool             ret;
    int              code;

    bool ok = in.read(connection);
    if (!ok) return false;

    // parse in, prepare out
    code = in.get(0).asVocab();
    ret = false;

    if (code == VOCAB_IROBOT_DESCRIPTION)
    {
        int macro_cmd = in.get(1).asVocab();
        if (macro_cmd == VOCAB_IROBOT_GET)
        {
            int cmd = in.get(2).asVocab();
            if (cmd == VOCAB_IROBOT_ALL)
            {
                out.addVocab(VOCAB_OK);
                Bottle& l = out.addList();
                for (size_t i = 0; i < m_robot_devices.size(); i++)
                {
                    l.addString(m_robot_devices[i].device_name);
                    l.addString(m_robot_devices[i].device_type);
                }
                ret = true;
            }
            else if (cmd == VOCAB_IROBOT_CONTROLBOARD_WRAPPERS)
            {
                out.addVocab(VOCAB_OK);
                Bottle& l = out.addList();
                for (size_t i = 0; i < m_robot_devices.size(); i++)
                {
                    if (m_robot_devices[i].device_type == "controlboardwrapper" || 
                        m_robot_devices[i].device_type == "controlboardwrapper2")
                    {
                        l.addString(m_robot_devices[i].device_name);
                        l.addString(m_robot_devices[i].device_type);
                    }
                }
                ret = true;
            }
            else
            {
                ret = false;
                yError("Invalid vocab received in RobotDescriptionServer");
            }

        }
        else if (macro_cmd == VOCAB_IROBOT_SET)
        {
            int cmd = in.get(2).asVocab();
            if (cmd == VOCAB_IROBOT_DEVICE)
            {
                RobotDescription desc;
                desc.device_name = in.get(3).asString();
                desc.device_type = in.get(4).asString();
                bool b_add = this->add_device(desc);
                if (b_add == false)
                {
                    yError() << "RobotDescriptionServer add_device failed";
                }
                else
                {
                    out.addVocab(VOCAB_OK);
                    ret = true;
                }
            }
            else
            {
                ret = false;
                yError("Invalid vocab received in RobotDescriptionServer");
            }
        }
        else
        {
            ret = false;
            yError("Invalid vocab received in RobotDescriptionServer");
        }
    }
    else
    {
        ret = false;
        yError("Invalid vocab received in RobotDescriptionServer");
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


yarp::dev::DriverCreator *createRobotDescriptionServer()
{
    return new DriverCreatorOf<RobotDescriptionServer>
               (
                   "robotDescriptionServer",
                   "",
                   "robotDescriptionServer"
               );
}

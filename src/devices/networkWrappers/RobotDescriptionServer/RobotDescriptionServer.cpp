/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IRobotDescription.h>
#include "RobotDescriptionServer.h"
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <mutex>

/*! \file RobotDescriptionServer.cpp */

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;

namespace {
YARP_LOG_COMPONENT(ROBOTDESCRIPTIONSERVER, "yarp.device.robotDescriptionServer")
}

//------------------------------------------------------------------------------------------------------------------------------

bool RobotDescriptionServer::open(yarp::os::Searchable &config)
{
    m_local_name.clear();
    m_local_name = config.find("local").asString();

    if (m_local_name == "")
    {
        yCError(ROBOTDESCRIPTIONSERVER, "open(): Invalid local name");
        return false;
    }

    std::string local_rpc = m_local_name;
    local_rpc += "/rpc";

    if (!m_rpc_port.open(local_rpc))
    {
        yCError(ROBOTDESCRIPTIONSERVER, "open(): Could not open rpc port %s, check network", local_rpc.c_str());
        return false;
    }

    m_rpc_port.setReader(*this);
    return true;
}

bool RobotDescriptionServer::attachAll(const PolyDriverList &p)
{
    std::lock_guard<std::mutex> guard(m_external_mutex);
    for (int i = 0; i < p.size(); i++)
    {
        yCTrace(ROBOTDESCRIPTIONSERVER) << p[i]->poly->getOptions().toString();
        yCTrace(ROBOTDESCRIPTIONSERVER) << p[i]->poly->getValue("device").toString();
        yCTrace(ROBOTDESCRIPTIONSERVER) << p[i]->poly->getValue("name").toString();
        DeviceDescription dev;
        dev.device_name = p[i]->poly->getValue("name").toString();
        dev.device_type = p[i]->poly->getValue("device").toString();
        if (this->add_device(dev) == false)
        {
            yCError(ROBOTDESCRIPTIONSERVER) << "attachAll(): Something strange happened here";
            //return false;
        }
    }
    return true;
}

bool RobotDescriptionServer::detachAll()
{
    std::lock_guard<std::mutex> guard(m_external_mutex);
    m_robot_devices.clear();
    return true;
}

bool RobotDescriptionServer::close()
{
    m_rpc_port.close();
    return true;
}

bool RobotDescriptionServer::add_device(DeviceDescription dev)
{
    std::lock_guard<std::mutex> guard(m_internal_mutex);
    for (auto& m_robot_device : m_robot_devices)
    {
        if (dev.device_name == m_robot_device.device_name)
        {
            yCWarning(ROBOTDESCRIPTIONSERVER) << "add_device(): Device" << dev.device_name << "already exists, skipping";
            return false;
        }
    }
    m_robot_devices.push_back(dev);
    return true;
}

bool RobotDescriptionServer::remove_device(DeviceDescription dev)
{
    std::lock_guard<std::mutex> guard(m_internal_mutex);
    for (auto it = m_robot_devices.begin(); it != m_robot_devices.end(); it++)
    {
        if (dev.device_name == it->device_name)
        {
            m_robot_devices.erase(it);
            return true;
        }
    }
    return false;
}

bool RobotDescriptionServer::read(yarp::os::ConnectionReader& connection)
{
    std::lock_guard<std::mutex> guard(m_external_mutex);
    yarp::os::Bottle in;
    yarp::os::Bottle out;
    bool             ret;
    int              code;

    bool ok = in.read(connection);
    if (!ok) {
        return false;
    }

    // parse in, prepare out
    code = in.get(0).asVocab32();
    ret = false;

    if (code == VOCAB_IROBOT_DESCRIPTION)
    {
        int macro_cmd = in.get(1).asVocab32();
        if (macro_cmd == VOCAB_IROBOT_GET)
        {
            int cmd = in.get(2).asVocab32();
            if (cmd == VOCAB_IROBOT_ALL)
            {
                out.addVocab32(VOCAB_OK);
                Bottle& l = out.addList();
                for (auto& m_robot_device : m_robot_devices)
                {
                    l.addString(m_robot_device.device_name);
                    l.addString(m_robot_device.device_type);
                }
                ret = true;
            }
            else if (cmd == VOCAB_IROBOT_BY_TYPE)
            {
                std::string type = in.get(3).asString();
                out.addVocab32(VOCAB_OK);
                Bottle& l = out.addList();
                for (auto& m_robot_device : m_robot_devices)
                {
                    if (m_robot_device.device_type == type)
                    {
                        l.addString(m_robot_device.device_name);
                        l.addString(m_robot_device.device_type);
                    }
                }
                ret = true;
            }
            else
            {
                ret = false;
                yCError(ROBOTDESCRIPTIONSERVER, "Invalid vocab received");
            }

        }
        else if (macro_cmd == VOCAB_IROBOT_DELETE)
        {
            int cmd = in.get(2).asVocab32();
            if (cmd == VOCAB_IROBOT_DEVICE)
            {
                std::string name = in.get(3).asString();
                DeviceDescription dev;
                dev.device_name = name;
                bool b_rem = this->remove_device(dev);
                if (b_rem == false)
                {
                    yCError(ROBOTDESCRIPTIONSERVER) << "remove_device failed";
                }
                else
                {
                    out.addVocab32(VOCAB_OK);
                    ret = true;
                }
            }
            else
            {
                ret = false;
                yCError(ROBOTDESCRIPTIONSERVER, "Invalid vocab received");
            }
        }
        else if (macro_cmd == VOCAB_IROBOT_SET)
        {
            int cmd = in.get(2).asVocab32();
            if (cmd == VOCAB_IROBOT_DEVICE)
            {
                DeviceDescription desc;
                desc.device_name = in.get(3).asString();
                desc.device_type = in.get(4).asString();
                bool b_add = this->add_device(desc);
                if (b_add == false)
                {
                    yCError(ROBOTDESCRIPTIONSERVER) << "add_device failed";
                }
                else
                {
                    out.addVocab32(VOCAB_OK);
                    ret = true;
                }
            }
            else
            {
                ret = false;
                yCError(ROBOTDESCRIPTIONSERVER, "Invalid vocab received");
            }
        }
        else
        {
            ret = false;
            yCError(ROBOTDESCRIPTIONSERVER, "Invalid vocab received");
        }
    }
    else
    {
        ret = false;
        yCError(ROBOTDESCRIPTIONSERVER, "Invalid vocab received");
    }

    if (!ret)
    {
        out.clear();
        out.addVocab32(VOCAB_FAILED);
    }

    yarp::os::ConnectionWriter *returnToSender = connection.getWriter();

    if (returnToSender != nullptr)
    {
        out.write(*returnToSender);
    }

    return true;
}

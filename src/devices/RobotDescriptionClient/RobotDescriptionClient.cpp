/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "RobotDescriptionClient.h"
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

/*! \file RobotDescriptionClient.cpp */

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;


namespace {
YARP_LOG_COMPONENT(ROBOTDESCRIPTIONCLIENT, "yarp.device.robotDescriptionClient")
}


//------------------------------------------------------------------------------------------------------------------------------

bool RobotDescriptionClient::open(yarp::os::Searchable &config)
{
    m_local_name.clear();
    m_remote_name.clear();

    m_local_name           = config.find("local").asString();
    m_remote_name          = config.find("remote").asString();

    if (m_local_name == "")
    {
        yCError(ROBOTDESCRIPTIONCLIENT, "open(): Invalid local name");
        return false;
    }

    if (m_remote_name == "")
    {
        yCError(ROBOTDESCRIPTIONCLIENT, "open(): Invalid remote name");
        return false;
    }

    std::string local_rpc,  remote_rpc;

    local_rpc  = m_local_name + "/rpc";
    remote_rpc = m_remote_name + "/rpc";

    if (!m_rpc_port.open(local_rpc))
    {
        yCError(ROBOTDESCRIPTIONCLIENT, "open(): Could not open rpc port %s, check network", local_rpc.c_str());
        return false;
    }


    bool ok = true;

    ok = Network::connect(local_rpc, remote_rpc);
    if (!ok)
    {
        yCError(ROBOTDESCRIPTIONCLIENT, "open(): Could not connect to %s", remote_rpc.c_str());
        return false;
    }


    return true;
}

bool RobotDescriptionClient::close()
{
    m_rpc_port.close();

    return true;
}

bool RobotDescriptionClient::getAllDevicesByType(const std::string &type, std::vector<DeviceDescription>& dev_list)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;
    dev_list.clear();

    b.addVocab32(VOCAB_IROBOT_DESCRIPTION);
    b.addVocab32(VOCAB_IROBOT_GET);
    b.addVocab32(VOCAB_IROBOT_BY_TYPE);
    b.addString(type);
    bool ret = m_rpc_port.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(ROBOTDESCRIPTIONCLIENT) << "getAllDevices(): Received error from server";
            return false;
        }
        else
        {
            Bottle *b = resp.get(1).asList();
            for (size_t i = 0; i < b->size(); i += 2)
            {
                DeviceDescription desc;
                desc.device_name = b->get(i).asString();
                desc.device_type = b->get(i + 1).asString();
                dev_list.push_back(desc);
            }
            return true;
        }
    }
    else
    {
        yCError(ROBOTDESCRIPTIONCLIENT) << "Error on writing on rpc port";
        return false;
    }
    return true;
}

bool RobotDescriptionClient::unregisterDevice(const std::string& device_name)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_IROBOT_DESCRIPTION);
    b.addVocab32(VOCAB_IROBOT_DELETE);
    b.addVocab32(VOCAB_IROBOT_DEVICE);
    b.addString(device_name);
    bool ret = m_rpc_port.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(ROBOTDESCRIPTIONCLIENT) << "unregisterDevice(): Received error from server";
            return false;
        }
    }
    else
    {
        yCError(ROBOTDESCRIPTIONCLIENT) << "Error on writing on rpc port";
        return false;
    }
    return true;
}

bool RobotDescriptionClient::registerDevice(const DeviceDescription& dev)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab32(VOCAB_IROBOT_DESCRIPTION);
    b.addVocab32(VOCAB_IROBOT_SET);
    b.addVocab32(VOCAB_IROBOT_DEVICE);
    b.addString(dev.device_name);
    b.addString(dev.device_type);
    bool ret = m_rpc_port.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(ROBOTDESCRIPTIONCLIENT) << "registerDevice(): Received error from server";
            return false;
        }
    }
    else
    {
        yCError(ROBOTDESCRIPTIONCLIENT) << "Error on writing on rpc port";
        return false;
    }
    return true;
}

bool RobotDescriptionClient::getAllDevices(std::vector<DeviceDescription>& dev_list)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;
    dev_list.clear();

    b.addVocab32(VOCAB_IROBOT_DESCRIPTION);
    b.addVocab32(VOCAB_IROBOT_GET);
    b.addVocab32(VOCAB_IROBOT_ALL);
    bool ret = m_rpc_port.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab32() != VOCAB_OK)
        {
            yCError(ROBOTDESCRIPTIONCLIENT) << "getAllDevices(): Received error from server";
            return false;
        }
        else
        {
            Bottle *b = resp.get(1).asList();
            for (size_t i = 0; i < b->size();i+=2)
            {
                DeviceDescription desc;
                desc.device_name = b->get(i).asString();
                desc.device_type = b->get(i+1).asString();
                dev_list.push_back(desc);
            }
            return true;
        }
    }
    else
    {
        yCError(ROBOTDESCRIPTIONCLIENT) << "Error on writing on rpc port";
        return false;
    }
    return true;
}

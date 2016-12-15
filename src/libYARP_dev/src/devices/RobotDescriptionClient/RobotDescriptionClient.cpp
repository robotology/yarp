/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "RobotDescriptionClient.h"
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/LockGuard.h>

/*! \file RobotDescriptionClient.cpp */

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;


//------------------------------------------------------------------------------------------------------------------------------

bool yarp::dev::RobotDescriptionClient::open(yarp::os::Searchable &config)
{
    m_local_name.clear();
    m_remote_name.clear();

    m_local_name           = config.find("local").asString().c_str();
    m_remote_name          = config.find("remote").asString().c_str();

    if (m_local_name == "")
    {
        yError("RobotDescriptionClient::open() error you have to provide valid local name");
        return false;
    }

    if (m_remote_name == "")
    {
        yError("RobotDescriptionClient::open() error you have to provide valid remote name");
        return false;
    }

    ConstString local_rpc,  remote_rpc;

    local_rpc  = m_local_name + "/rpc";
    remote_rpc = m_remote_name + "/rpc";
    
    if (!m_rpc_port.open(local_rpc.c_str()))
    {
        yError("RobotDescriptionClient::open() error could not open rpc port %s, check network", local_rpc.c_str());
        return false;
    }


    bool ok = true;

    ok = Network::connect(local_rpc.c_str(), remote_rpc.c_str());
    if (!ok)
    {
        yError("RobotDescriptionClient::open() error could not connect to %s", remote_rpc.c_str());
        return false;
    }


    return true;
}

bool yarp::dev::RobotDescriptionClient::close()
{
    m_rpc_port.close();

    return true;
}

bool yarp::dev::RobotDescriptionClient::getControlBoardWrapperDevices(std::vector<RobotDescription>& dev_list)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_IROBOT_DESCRIPTION);
    b.addVocab(VOCAB_IROBOT_GET);
    b.addVocab(VOCAB_IROBOT_CONTROLBOARD_WRAPPERS);
    bool ret = m_rpc_port.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "RobotDescriptionClient::getAllDevices() received error from server";
            return false;
        }
        else
        {
            Bottle *b = resp.get(1).asList();
            for (int i = 0; i < b->size(); i += 2)
            {
                RobotDescription desc;
                desc.device_name = b->get(i).asString();
                desc.device_type = b->get(i + 1).asString();
                dev_list.push_back(desc);
            }
            return true;
        }
    }
    else
    {
        yError() << "RobotDescriptionClient: error on writing on rpc port";
        return false;
    }
    return true;
}

bool yarp::dev::RobotDescriptionClient::registerDevice(const RobotDescription& dev)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_IROBOT_DESCRIPTION);
    b.addVocab(VOCAB_IROBOT_SET);
    b.addVocab(VOCAB_IROBOT_DEVICE);
    b.addString(dev.device_name);
    b.addString(dev.device_type);
    bool ret = m_rpc_port.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "RobotDescriptionClient::registerDevice() received error from server";
            return false;
        }
    }
    else
    {
        yError() << "RobotDescriptionClient: error on writing on rpc port";
        return false;
    }
    return true;
}

bool yarp::dev::RobotDescriptionClient::getAllDevices(std::vector<RobotDescription>& dev_list)
{
    yarp::os::Bottle b;
    yarp::os::Bottle resp;

    b.addVocab(VOCAB_IROBOT_DESCRIPTION);
    b.addVocab(VOCAB_IROBOT_GET);
    b.addVocab(VOCAB_IROBOT_ALL);
    bool ret = m_rpc_port.write(b, resp);
    if (ret)
    {
        if (resp.get(0).asVocab() != VOCAB_OK)
        {
            yError() << "RobotDescriptionClient::getAllDevices() received error from server";
            return false;
        }
        else
        {
            Bottle *b = resp.get(1).asList();
            for (int i = 0; i < b->size();i+=2) 
            {
                RobotDescription desc;
                desc.device_name = b->get(i).asString();
                desc.device_type = b->get(i+1).asString();
                dev_list.push_back(desc);
            }
            return true;
        }
    }
    else
    {
        yError() << "RobotDescriptionClient: error on writing on rpc port";
        return false;
    }
    return true;
}

yarp::dev::DriverCreator *createRobotDescriptionClient()
{
    return new DriverCreatorOf<RobotDescriptionClient>
               (
                   "robotDescriptionClient",
                   "",
                   "robotDescriptionClient"
               );
}

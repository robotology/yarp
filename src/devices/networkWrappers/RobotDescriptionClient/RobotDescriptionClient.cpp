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
    if (!this->parseParams(config)) { return false; }

    if (!m_rpc_port.open(m_local))
    {
        yCError(ROBOTDESCRIPTIONCLIENT, "open(): Could not open rpc port %s, check network", m_local.c_str());
        return false;
    }


    bool ok = true;

    ok = Network::connect(m_local, m_remote);
    if (!ok)
    {
        yCError(ROBOTDESCRIPTIONCLIENT, "open(): Could not connect to %s", m_remote.c_str());
        return false;
    }

    if (!m_RPC.yarp().attachAsClient(m_rpc_port))
    {
       yCError(ROBOTDESCRIPTIONCLIENT, "Error! Cannot attach the port as a client");
       return false;
    }

    //protocol check
    //to be added here

    return true;
}

bool RobotDescriptionClient::close()
{
    m_rpc_port.close();

    return true;
}

yarp::dev::ReturnValue RobotDescriptionClient::getAllDevicesByType(const std::string &type, std::vector<DeviceDescription>& dev_list)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto ret = m_RPC.getAllDevicesByTypeRPC(type);
    if (!ret.ret) {
        yCError(ROBOTDESCRIPTIONCLIENT, "Unable to getAllDevicesByType");
        return ret.ret;
    }
    dev_list = ret.devices;
    return ReturnValue_ok;
}

yarp::dev::ReturnValue RobotDescriptionClient::unregisterDevice(const std::string& device_name)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto ret = m_RPC.unregisterDeviceRPC(device_name);
    if (!ret) {
        yCError(ROBOTDESCRIPTIONCLIENT, "Unable to unregisterDevice");
        return ret;
    }
    return ReturnValue_ok;
}

yarp::dev::ReturnValue RobotDescriptionClient::registerDevice(const DeviceDescription& dev)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto ret = m_RPC.registerDeviceRPC(dev);
    if (!ret) {
        yCError(ROBOTDESCRIPTIONCLIENT, "Unable to registerDevice");
        return ret;
    }
    return ReturnValue_ok;
}


yarp::dev::ReturnValue RobotDescriptionClient::getAllDevices(std::vector<DeviceDescription>& dev_list)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return_getAllDevices ret = m_RPC.getAllDevicesRPC();
    if (!ret.ret) {
        yCError(ROBOTDESCRIPTIONCLIENT, "Unable to getAllDevices");
        return ret.ret;
    }
    dev_list = ret.devices;
    return ReturnValue_ok;
}

yarp::dev::ReturnValue RobotDescriptionClient::unregisterAll()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto ret = m_RPC.unregisterAllRPC();
    if (!ret) {
        yCError(ROBOTDESCRIPTIONCLIENT, "Unable to unregisterAll");
        return ret;
    }
    return ReturnValue_ok;
}

/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "RobotDescription_nwc_yarp.h"
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;


namespace {
YARP_LOG_COMPONENT(ROBOTDESCRIPTION_NWC_YARP, "yarp.device.robotDescription_nwc_yarp")
}


//------------------------------------------------------------------------------------------------------------------------------

bool RobotDescription_nwc_yarp::open(yarp::os::Searchable &config)
{
    if (!this->parseParams(config)) { return false; }

    if (!m_rpc_port.open(m_local))
    {
        yCError(ROBOTDESCRIPTION_NWC_YARP, "open(): Could not open rpc port %s, check network", m_local.c_str());
        return false;
    }


    bool ok = true;

    ok = Network::connect(m_local, m_remote);
    if (!ok)
    {
        yCError(ROBOTDESCRIPTION_NWC_YARP, "open(): Could not connect to %s", m_remote.c_str());
        return false;
    }

    if (!m_RPC.yarp().attachAsClient(m_rpc_port))
    {
       yCError(ROBOTDESCRIPTION_NWC_YARP, "Error! Cannot attach the port as a client");
       return false;
    }

    // Check the protocol version
    if (!m_RPC.checkProtocolVersion()) {
        return false;
    }

    return true;
}

bool RobotDescription_nwc_yarp::close()
{
    m_rpc_port.close();

    return true;
}

yarp::dev::ReturnValue RobotDescription_nwc_yarp::getAllDevicesByType(const std::string &type, std::vector<DeviceDescription>& dev_list)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto ret = m_RPC.getAllDevicesByTypeRPC(type);
    if (!ret.ret) {
        yCError(ROBOTDESCRIPTION_NWC_YARP, "Unable to getAllDevicesByType");
        return ret.ret;
    }
    dev_list = ret.devices;
    return ReturnValue_ok;
}

yarp::dev::ReturnValue RobotDescription_nwc_yarp::unregisterDevice(const std::string& device_name)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto ret = m_RPC.unregisterDeviceRPC(device_name);
    if (!ret) {
        yCError(ROBOTDESCRIPTION_NWC_YARP, "Unable to unregisterDevice");
        return ret;
    }
    return ReturnValue_ok;
}

yarp::dev::ReturnValue RobotDescription_nwc_yarp::registerDevice(const DeviceDescription& dev)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto ret = m_RPC.registerDeviceRPC(dev);
    if (!ret) {
        yCError(ROBOTDESCRIPTION_NWC_YARP, "Unable to registerDevice");
        return ret;
    }
    return ReturnValue_ok;
}


yarp::dev::ReturnValue RobotDescription_nwc_yarp::getAllDevices(std::vector<DeviceDescription>& dev_list)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return_getAllDevices ret = m_RPC.getAllDevicesRPC();
    if (!ret.ret) {
        yCError(ROBOTDESCRIPTION_NWC_YARP, "Unable to getAllDevices");
        return ret.ret;
    }
    dev_list = ret.devices;
    return ReturnValue_ok;
}

yarp::dev::ReturnValue RobotDescription_nwc_yarp::unregisterAll()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto ret = m_RPC.unregisterAllRPC();
    if (!ret) {
        yCError(ROBOTDESCRIPTION_NWC_YARP, "Unable to unregisterAll");
        return ret;
    }
    return ReturnValue_ok;
}

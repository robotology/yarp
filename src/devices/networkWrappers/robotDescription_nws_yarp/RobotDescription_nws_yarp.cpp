/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IRobotDescription.h>
#include "RobotDescription_nws_yarp.h"
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <mutex>

using namespace yarp::dev;
using namespace yarp::os;

namespace {
YARP_LOG_COMPONENT(ROBOTDESCRIPTION_NWS_YARP, "yarp.device.robotDescription_nws_yarp")
}

return_getAllDevices IRobotDescriptiond::getAllDevicesRPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    std::vector<DeviceDescription> dev_list;
    return_getAllDevices retval;
    retval.ret = m_istorage->getAllDevices(dev_list);
    if (retval.ret) {
        retval.devices = dev_list;
    } else {
        yCError(ROBOTDESCRIPTION_NWS_YARP, "Unable to getAllDevices");
    }
    return retval;
}

return_getAllDevicesByType IRobotDescriptiond::getAllDevicesByTypeRPC(const std::string& type)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    std::vector<DeviceDescription> dev_list;
    return_getAllDevicesByType retval;
    retval.ret = m_istorage->getAllDevicesByType(type, dev_list);
    if (retval.ret) {
        retval.devices = dev_list;
    } else {
        yCError(ROBOTDESCRIPTION_NWS_YARP, "Unable to getAllDevicesByType");
    }
    return retval;
}

yarp::dev::ReturnValue IRobotDescriptiond::registerDeviceRPC(const yarp::dev::DeviceDescription& dev)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_istorage->registerDevice(dev);
    if (!ret)
    {
        yCError(ROBOTDESCRIPTION_NWS_YARP, "Unable to registerDevice");
    }
    return ret;
}

yarp::dev::ReturnValue IRobotDescriptiond::unregisterDeviceRPC(const std::string& dev)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_istorage->unregisterDevice(dev);
    if (!ret)
    {
        yCError(ROBOTDESCRIPTION_NWS_YARP, "Unable to unregisterDevice");
    }
    return ret;
}

yarp::dev::ReturnValue IRobotDescriptiond::unregisterAllRPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_istorage->unregisterAll();
    if (!ret)
    {
        yCError(ROBOTDESCRIPTION_NWS_YARP, "Unable to unregisterAll");
    }
    return ret;
}

//------------------------------------------------------------------------------------------------------------------------------

bool RobotDescription_nws_yarp::open(yarp::os::Searchable &config)
{
    if (!this->parseParams(config)) { return false; }

    if (!m_rpc_port.open(m_local))
    {
        yCError(ROBOTDESCRIPTION_NWS_YARP, "open(): Could not open rpc port %s, check network", m_local.c_str());
        return false;
    }

    m_rpc_port.setReader(*this);
    return true;
}

bool RobotDescription_nws_yarp::attach(yarp::dev::PolyDriver* driver)
{
    std::lock_guard lock(m_mutex);

    if (driver->isValid())
    {
        IRobotDescription* ird = nullptr;
        driver->view(ird);
        if (!ird)
        {
            yCError(ROBOTDESCRIPTION_NWS_YARP, "Subdevice passed to attach method is invalid");
            return false;
        }
        m_RPC = std::make_unique<IRobotDescriptiond>(ird);
    }

    yCInfo(ROBOTDESCRIPTION_NWS_YARP, "Attach done");
    return true;
}

bool RobotDescription_nws_yarp::detach()
{
    std::lock_guard lock (m_mutex);
    return true;
}

bool RobotDescription_nws_yarp::close()
{
    m_rpc_port.close();
    return true;
}

bool RobotDescription_nws_yarp::read(yarp::os::ConnectionReader& connection)
{
    if (!connection.isValid()) { return false;}
    if (!m_RPC) { return false;}

    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_RPC)
    {
        bool b = m_RPC->read(connection);
        if (b) {
            return true;
        }
    }

    yCDebug(ROBOTDESCRIPTION_NWS_YARP) << "read() Command failed";
    return false;
}

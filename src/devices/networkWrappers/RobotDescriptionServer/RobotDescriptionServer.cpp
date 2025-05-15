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

using namespace yarp::dev;
using namespace yarp::os;

namespace {
YARP_LOG_COMPONENT(ROBOTDESCRIPTIONSERVER, "yarp.device.robotDescriptionServer")
}

return_getAllDevices IRobotDescriptiond::getAllDevicesRPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    std::vector<DeviceDescription> dev_list;
    return_getAllDevices retval;
    retval.ret = m_storage->getAllDevices(dev_list);
    if (retval.ret) {
        retval.devices = dev_list;
    } else {
        yCError(ROBOTDESCRIPTIONSERVER, "Unable to getAllDevices");
    }
    return retval;
}

return_getAllDevicesByType IRobotDescriptiond::getAllDevicesByTypeRPC(const std::string& type)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    std::vector<DeviceDescription> dev_list;
    return_getAllDevicesByType retval;
    retval.ret = m_storage->getAllDevicesByType(type, dev_list);
    if (retval.ret) {
        retval.devices = dev_list;
    } else {
        yCError(ROBOTDESCRIPTIONSERVER, "Unable to getAllDevicesByType");
    }
    return retval;
}

yarp::dev::ReturnValue IRobotDescriptiond::registerDeviceRPC(const yarp::dev::DeviceDescription& dev)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_storage->registerDevice(dev);
    if (!ret)
    {
        yCError(ROBOTDESCRIPTIONSERVER, "Unable to registerDevice");
    }
    return ret;
}

yarp::dev::ReturnValue IRobotDescriptiond::unregisterDeviceRPC(const std::string& dev)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_storage->unregisterDevice(dev);
    if (!ret)
    {
        yCError(ROBOTDESCRIPTIONSERVER, "Unable to unregisterDevice");
    }
    return ret;
}

yarp::dev::ReturnValue IRobotDescriptiond::unregisterAllRPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_storage->unregisterAll();
    if (!ret)
    {
        yCError(ROBOTDESCRIPTIONSERVER, "Unable to unregisterAll");
    }
    return ret;
}

//------------------------------------------------------------------------------------------------------------------------------

bool RobotDescriptionServer::open(yarp::os::Searchable &config)
{
    if (!this->parseParams(config)) { return false; }

    if (!m_rpc_port.open(m_local))
    {
        yCError(ROBOTDESCRIPTIONSERVER, "open(): Could not open rpc port %s, check network", m_local.c_str());
        return false;
    }

    m_RPC = std::make_unique<IRobotDescriptiond>();

    m_rpc_port.setReader(*this);
    return true;
}

bool RobotDescriptionServer::attachAll(const PolyDriverList &p)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    for (int i = 0; i < p.size(); i++)
    {
        DeviceDescription dev;

        //BROKEN HERE. TO BE REIMPLEMENTED
        //Register every device to which the server is attached
        //dev.device_name = p[i]->poly->getValue("name").toString();
        //dev.device_type = p[i]->poly->getValue("device").toString();

        auto ret = m_RPC->registerDeviceRPC(dev);
        if (!ret)
        {
            yCError(ROBOTDESCRIPTIONSERVER) << "attachAll(): Something strange happened here";
            //return false;
        }
    }
    return true;
}

bool RobotDescriptionServer::detachAll()
{
    //Clear the storage
    m_RPC->unregisterAllRPC();

    return true;
}

bool RobotDescriptionServer::close()
{
    m_rpc_port.close();
    return true;
}

bool RobotDescriptionServer::read(yarp::os::ConnectionReader& connection)
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

    yCDebug(ROBOTDESCRIPTIONSERVER) << "read() Command failed";
    return false;
}

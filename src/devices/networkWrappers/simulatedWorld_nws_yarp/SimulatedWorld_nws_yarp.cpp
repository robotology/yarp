/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "SimulatedWorld_nws_yarp.h"
#include <sstream>
#include <string>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <time.h>
#include <stdlib.h>

using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::os;

namespace {
YARP_LOG_COMPONENT(SIMWORLDNWS, "yarp.device.SimulatedWorld_nws_yarp")
}

SimulatedWorld_nws_yarp::SimulatedWorld_nws_yarp()
{
}

SimulatedWorld_nws_yarp::~SimulatedWorld_nws_yarp()
{
    m_Isim_p = nullptr;
}

bool SimulatedWorld_nws_yarp::attach(PolyDriver* driver)
{
    if (driver==nullptr)
    {
        yCError(SIMWORLDNWS, "Invalid pointer to device driver received");
        return false;
    }

    driver->view(m_Isim_p);
    if (nullptr == m_Isim_p)
    {
        yCError(SIMWORLDNWS, "Unable to view ISimulatedWorld interface");
        return false;
    }
    m_msgsImpl = std::make_unique<ISimulatedWorldMsgsImpl>(m_Isim_p);

    return true;
}

bool SimulatedWorld_nws_yarp::detach()
{
    m_Isim_p = nullptr;
    return true;
}

bool SimulatedWorld_nws_yarp::read(yarp::os::ConnectionReader& connection)
{
    if (!connection.isValid()) { return false;}
    if (!m_msgsImpl) { return false;}

    bool b = m_msgsImpl->read(connection);
    if (b)
    {
        return true;
    }
    else
    {
        yCError(SIMWORLDNWS, "read() Command failed");
        return false;
    }
}

bool SimulatedWorld_nws_yarp::open(yarp::os::Searchable &config)
{
    if (!parseParams(config)) { return false; }

    m_rpcPortName = m_name + "/rpc:i";

    if (!m_rpcPort.open(m_rpcPortName.c_str()))
    {
        yCError(SIMWORLDNWS) << "Error opening port" << m_rpcPortName;
        return false;
    }
    m_rpcPort.setReader(*this);

    return true;
}

bool SimulatedWorld_nws_yarp::close()
{
    yCTrace(SIMWORLDNWS, "BatteryWrapper::Close");
    m_rpcPort.close();
    detachAll();
    return true;
}

ReturnValue ISimulatedWorldMsgsImpl::attach(const std::string& id, const std::string& link_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_iSim->attach(id, link_name);
    if(!ret)
    {
        yCError(SIMWORLDNWS) << "Could not attach";
    }
    return ret;
}

ReturnValue ISimulatedWorldMsgsImpl::changeColor(const std::string& id, const ColorRGB& color)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_iSim->changeColor(id, color);
    if(!ret)
    {
        yCError(SIMWORLDNWS) << "Could not changeColor";
    }
    return ret;
}

ReturnValue ISimulatedWorldMsgsImpl::deleteObject(const std::string& id)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_iSim->deleteObject(id);
    if(!ret)
    {
        yCError(SIMWORLDNWS) << "Could not deleteObject";
    }
    return ret;
}

ReturnValue ISimulatedWorldMsgsImpl::deleteAll()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_iSim->deleteAll();
    if(!ret)
    {
        yCError(SIMWORLDNWS) << "Could not deleteAll";
    }
    return ret;
}

ReturnValue ISimulatedWorldMsgsImpl::enableCollision(const std::string& id, const bool enable)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_iSim->enableCollision(id, enable);
    if(!ret)
    {
        yCError(SIMWORLDNWS) << "Could not enableCollision";
    }
    return ret;
}

ReturnValue ISimulatedWorldMsgsImpl::enableGravity(const std::string& id, const bool enable)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_iSim->enableGravity(id, enable);
    if(!ret)
    {
        yCError(SIMWORLDNWS) << "Could not enableGravity";
    }
    return ret;
}

ReturnValue ISimulatedWorldMsgsImpl::setPose(const std::string& id, const Pose6D& pose, const std::string& frame_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_iSim->setPose(id, pose, frame_name);
    if(!ret)
    {
        yCError(SIMWORLDNWS) << "Could not setPose";
    }
    return ret;
}

ReturnValue ISimulatedWorldMsgsImpl::makeSphere(const std::string& id_name, const double radius, const Pose6D& pose, const ColorRGB& color, const std::string& frame_name, const bool gravity_enable, const bool collision_enable)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_iSim->makeSphere(id_name, radius, pose, color, frame_name, gravity_enable, collision_enable);
    if(!ret)
    {
        yCError(SIMWORLDNWS) << "Could not makeSphere";
    }
    return ret;
}

ReturnValue ISimulatedWorldMsgsImpl::makeBox(const std::string& id_name, const double width, const double height, const double thickness, const Pose6D& pose, const ColorRGB& color, const std::string& frame_name, const bool gravity_enable, const bool collision_enable)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_iSim->makeBox(id_name, width, height, thickness, pose, color, frame_name, gravity_enable, collision_enable);
    if(!ret)
    {
        yCError(SIMWORLDNWS) << "Could not makeBox";
    }
    return ret;
}

ReturnValue ISimulatedWorldMsgsImpl::makeCylinder(const std::string& id_name, const double radius, const double length, const Pose6D& pose, const ColorRGB& color, const std::string& frame_name, const bool gravity_enable, const bool collision_enable)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_iSim->makeCylinder(id_name, radius, length, pose, color, frame_name, gravity_enable, collision_enable);
    if(!ret)
    {
        yCError(SIMWORLDNWS) << "Could not makeCylinder";
    }
    return ret;
}

ReturnValue ISimulatedWorldMsgsImpl::makeFrame(const std::string& id_name, const double size, const Pose6D& pose, const ColorRGB& color, const std::string& frame_name, const bool gravity_enable, const bool collision_enable)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_iSim->makeFrame(id_name, size, pose, color, frame_name, gravity_enable, collision_enable);
    if(!ret)
    {
        yCError(SIMWORLDNWS) << "Could not makeFrame";
    }
    return ret;
}

ReturnValue ISimulatedWorldMsgsImpl::makeModel(const std::string& id_name, const std::string& filename, const Pose6D& pose, const std::string& frame_name, const bool gravity_enable, const bool collision_enable)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_iSim->makeModel(id_name, filename, pose, frame_name, gravity_enable, collision_enable);
    if(!ret)
    {
        yCError(SIMWORLDNWS) << "Could not makeModel";
    }
    return ret;
}

ReturnValue ISimulatedWorldMsgsImpl::detach(const std::string& id)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_iSim->detach(id);
    if(!ret)
    {
        yCError(SIMWORLDNWS) << "Could not detach";
    }
    return ret;
}

ReturnValue ISimulatedWorldMsgsImpl::rename(const std::string& old_name, const std::string& new_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    auto ret = m_iSim->rename(old_name, new_name);
    if(!ret)
    {
        yCError(SIMWORLDNWS) << "Could not rename";
    }
    return ret;
}

getListReturnValue  ISimulatedWorldMsgsImpl::getList()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    getListReturnValue response;

    std::vector<std::string> names;
    auto ret = m_iSim->getList(names);
    if(!ret)
    {
        yCError(SIMWORLDNWS) << "Could not getList";
        response.retVal = ret;
        return response;
    }

    response.retVal = ret;
    response.listnames = names;

    return response;
}

getPoseReturnValue  ISimulatedWorldMsgsImpl::getPose(const std::string& id, const std::string& frame_name)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    getPoseReturnValue response;

    Pose6D pose;
    auto ret = m_iSim->getPose(id, pose, frame_name);
    if(!ret)
    {
        yCError(SIMWORLDNWS) << "Could not getPose";
        response.retVal = ret;
        return response;
    }

    response.retVal = ret;
    response.pose = pose;

    return response;
}

ISimulatedWorldMsgsImpl::ISimulatedWorldMsgsImpl(yarp::dev::ISimulatedWorld* _isim)
{
    m_iSim = _isim;
}

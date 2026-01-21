/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "SimulatedWorld_nwc_yarp.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;

namespace {
YARP_LOG_COMPONENT(SIMWORLDNWC, "yarp.device.SimulatedWorld_nwc_yarp")
} // namespace


bool SimulatedWorld_nwc_yarp::open(yarp::os::Searchable &config)
{
    if (!parseParams(config)) { return false; }

    std::string local_rpc = m_local;
    local_rpc += "/rpc:o";
    std::string remote_rpc = m_remote;
    remote_rpc += "/rpc:i";

    if (!m_rpcPort.open(local_rpc))
    {
        yCError(SIMWORLDNWC, "open(): Could not open rpc port %s, check network", local_rpc.c_str());
        return false;
    }

    bool ok=Network::connect(local_rpc, remote_rpc);
    if (!ok)
    {
       yCError(SIMWORLDNWC, "open() Could not connect %s -> %s", remote_rpc.c_str(), local_rpc.c_str());
       return false;
    }
    if (!m_sim_RPC.yarp().attachAsClient(m_rpcPort))
    {
        yCError(SIMWORLDNWC, "Error! Cannot attach the rpcPort as a client");
        return false;
    }

    return true;
}

bool SimulatedWorld_nwc_yarp::close()
{
    m_rpcPort.close();
    return true;
}

ReturnValue SimulatedWorld_nwc_yarp::attach(std::string id, std::string link_name)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto ret = m_sim_RPC.attach(id, link_name);
    if (!ret) {
        yCError(SIMWORLDNWC, "Unable to attach");
        return ret;
    }
    return ret;
}

ReturnValue SimulatedWorld_nwc_yarp::makeSphere (std::string id, double radius, Pose6D pose, ColorRGB color, std::string frame_name, bool gravity_enable, bool collision_enable)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto ret = m_sim_RPC.makeSphere(id, radius, pose, color, frame_name, gravity_enable, collision_enable);
    if (!ret) {
        yCError(SIMWORLDNWC, "Unable to makeSphere");
        return ret;
    }
    return ret;
}

ReturnValue SimulatedWorld_nwc_yarp::makeBox (std::string id, double width, double height, double thickness, Pose6D pose, ColorRGB color, std::string frame_name, bool gravity_enable, bool collision_enable)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto ret = m_sim_RPC.makeBox(id, width, height, thickness, pose, color, frame_name, gravity_enable, collision_enable);
    if (!ret) {
        yCError(SIMWORLDNWC, "Unable to makeBox");
        return ret;
    }
    return ret;
}

ReturnValue SimulatedWorld_nwc_yarp::makeCylinder (std::string id, double radius, double length, Pose6D pose, ColorRGB color, std::string frame_name, bool gravity_enable, bool collision_enable)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto ret = m_sim_RPC.makeCylinder(id, radius, length, pose, color, frame_name, gravity_enable, collision_enable);
    if (!ret) {
        yCError(SIMWORLDNWC, "Unable to makeCylinder");
        return ret;
    }
    return ret;
}

ReturnValue SimulatedWorld_nwc_yarp::makeFrame (std::string id, double size, Pose6D pose, ColorRGB color, std::string frame_name, bool gravity_enable, bool collision_enable)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto ret = m_sim_RPC.makeFrame(id, size, pose, color, frame_name, gravity_enable, collision_enable);
    if (!ret) {
        yCError(SIMWORLDNWC, "Unable to makeFrame");
        return ret;
    }
    return ret;
}

ReturnValue SimulatedWorld_nwc_yarp::changeColor (std::string id, ColorRGB color)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto ret = m_sim_RPC.changeColor(id, color);
    if (!ret) {
        yCError(SIMWORLDNWC, "Unable to changeColor");
        return ret;
    }
    return ret;
}

ReturnValue SimulatedWorld_nwc_yarp::setPose(std::string id, Pose6D pose, std::string frame_name)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto ret = m_sim_RPC.setPose(id, pose, frame_name);
    if (!ret) {
        yCError(SIMWORLDNWC, "Unable to setPose");
        return ret;
    }
    return ret;
}

ReturnValue SimulatedWorld_nwc_yarp::enableGravity (std::string id, bool enable)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto ret = m_sim_RPC.enableGravity(id, enable);
    if (!ret) {
        yCError(SIMWORLDNWC, "Unable to enableGravity");
        return ret;
    }
    return ret;
}

ReturnValue SimulatedWorld_nwc_yarp::enableCollision (std::string id, bool enable)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto ret = m_sim_RPC.enableCollision(id, enable);
    if (!ret) {
        yCError(SIMWORLDNWC, "Unable to enableCollision");
        return ret;
    }
    return ret;
}

ReturnValue SimulatedWorld_nwc_yarp::getPose(std::string id, Pose6D& pose,  std::string frame_name)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto ret = m_sim_RPC.getPose(id, frame_name);
    if (!ret.retVal) {
        yCError(SIMWORLDNWC, "Unable to getPose");
        return ret.retVal;
    }
    pose = ret.pose;
    return ret.retVal;
}

ReturnValue SimulatedWorld_nwc_yarp::makeModel(std::string id,  std::string filename, Pose6D pose, std::string frame_name, bool gravity_enable, bool collision_enable)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto ret = m_sim_RPC.makeModel(id, filename, pose, frame_name, gravity_enable, collision_enable);
    if (!ret) {
        yCError(SIMWORLDNWC, "Unable to makeModel");
        return ret;
    }
    return ret;
}

ReturnValue SimulatedWorld_nwc_yarp::deleteObject(std::string id)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto ret = m_sim_RPC.deleteObject(id);
    if (!ret) {
        yCError(SIMWORLDNWC, "Unable to deleteObject");
        return ret;
    }
    return ret;
}

ReturnValue SimulatedWorld_nwc_yarp::deleteAll()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto ret = m_sim_RPC.deleteAll();
    if (!ret) {
        yCError(SIMWORLDNWC, "Unable to deleteAll");
        return ret;
    }
    return ret;
}

ReturnValue SimulatedWorld_nwc_yarp::getList(std::vector<std::string>& names)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto ret = m_sim_RPC.getList();
    if (!ret.retVal) {
        yCError(SIMWORLDNWC, "Unable to getList");
        return ret.retVal;
    }
    names = ret.listnames;
    return ret.retVal;

}

ReturnValue SimulatedWorld_nwc_yarp::detach(std::string id)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto ret = m_sim_RPC.detach(id);
    if (!ret) {
        yCError(SIMWORLDNWC, "Unable to detach");
        return ret;
    }
    return ret;
}

ReturnValue SimulatedWorld_nwc_yarp::rename(std::string old_name, std::string new_name)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto ret = m_sim_RPC.rename(old_name,new_name);
    if (!ret) {
        yCError(SIMWORLDNWC, "Unable to rename");
        return ret;
    }
    return ret;
}

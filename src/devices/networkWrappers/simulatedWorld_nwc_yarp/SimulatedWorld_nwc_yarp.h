/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_SIMULATEDWORLDNWC_H
#define YARP_DEV_SIMULATEDWORLDNWC_H

#include <mutex>

#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/ISimulatedWorld.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Time.h>
#include <yarp/dev/PolyDriver.h>

#include "ISimulatedWorldMsgs.h"
#include "SimulatedWorld_nwc_yarp_ParamsParser.h"

#define DEFAULT_THREAD_PERIOD 20 //ms

/**
* @ingroup dev_impl_network_clients
*
* \brief `SimulatedWorld_nwc_yarp`: The client side of any ISimulatedWorld capable device.
*
* Parameters required by this device are shown in class: SimulatedWorld_nwc_yarp_ParamsParser
*
*/
class SimulatedWorld_nwc_yarp :
        public yarp::dev::DeviceDriver,
        public yarp::dev::ISimulatedWorld,
        public SimulatedWorld_nwc_yarp_ParamsParser
{
protected:
    yarp::os::Port      m_rpcPort;
    ISimulatedWorldMsgs m_sim_RPC;
    std::mutex          m_mutex;

public:

    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    /* ISimulatedWorld methods */
    yarp::dev::ReturnValue makeSphere (std::string id, double radius, yarp::sig::Pose6D pose, yarp::sig::ColorRGB color, std::string frame_nam, bool gravity_enable, bool collision_enable) override;
    yarp::dev::ReturnValue makeBox (std::string id, double width, double height, double thickness, yarp::sig::Pose6D pose, yarp::sig::ColorRGB color, std::string frame_name, bool gravity_enable, bool collision_enable) override;
    yarp::dev::ReturnValue makeCylinder (std::string id, double radius, double length, yarp::sig::Pose6D pose, yarp::sig::ColorRGB color, std::string frame_name, bool gravity_enable, bool collision_enable) override;
    yarp::dev::ReturnValue makeFrame (std::string id, double size, yarp::sig::Pose6D pose, yarp::sig::ColorRGB color, std::string frame_name,  bool gravity_enable, bool collision_enable) override;
    yarp::dev::ReturnValue makeModel(std::string id,  std::string filename, yarp::sig::Pose6D pose,  std::string frame_name, bool gravity_enable, bool collision_enable) override;
    yarp::dev::ReturnValue changeColor (std::string id, yarp::sig::ColorRGB color) override;
    yarp::dev::ReturnValue setPose(std::string id, yarp::sig::Pose6D pose, std::string frame_name) override;
    yarp::dev::ReturnValue enableGravity (std::string id, bool enable) override;
    yarp::dev::ReturnValue enableCollision (std::string id, bool enable) override;
    yarp::dev::ReturnValue getPose(std::string id, yarp::sig::Pose6D& pose,  std::string frame_name) override;
    yarp::dev::ReturnValue deleteObject(std::string id) override;
    yarp::dev::ReturnValue deleteAll() override;
    yarp::dev::ReturnValue getList(std::vector<std::string>& names) override;
    yarp::dev::ReturnValue attach(std::string id, std::string link_name) override;
    yarp::dev::ReturnValue detach(std::string id) override;
    yarp::dev::ReturnValue rename(std::string old_name, std::string new_name) override;
};

#endif // YARP_DEV_SIMULATEDWORLDNWC_H

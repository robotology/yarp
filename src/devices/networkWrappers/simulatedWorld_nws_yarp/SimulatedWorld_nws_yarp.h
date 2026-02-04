/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_SIMULATEDWORDNWSYARP_H
#define YARP_DEV_SIMULATEDWORDNWSYARP_H

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>

#include <yarp/sig/Vector.h>
#include <yarp/sig/Pose6D.h>
#include <yarp/sig/ColorRGB.h>

#include <yarp/dev/ISimulatedWorld.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/api.h>

#include "ISimulatedWorldMsgs.h"

#include "SimulatedWorld_nws_yarp_ParamsParser.h"

// rpc commands
class ISimulatedWorldMsgsImpl : public ISimulatedWorldMsgs
{
private:
    std::mutex                     m_mutex;
    yarp::dev::ISimulatedWorld*    m_iSim{nullptr};

public:
    yarp::dev::ReturnValue makeSphere(const std::string& id_name, const double radius, const yarp::sig::Pose6D& pose, const yarp::sig::ColorRGB& color, const std::string& frame_name, const bool gravity_enable, const bool collision_enable) override;
    yarp::dev::ReturnValue makeBox(const std::string& id_name, const double width, const double height, const double thickness, const yarp::sig::Pose6D& pose, const yarp::sig::ColorRGB& color, const std::string& frame_name, const bool gravity_enable, const bool collision_enable) override;
    yarp::dev::ReturnValue makeCylinder(const std::string& id_name, const double radius, const double length, const yarp::sig::Pose6D& pose, const yarp::sig::ColorRGB& color, const std::string& frame_name, const bool gravity_enable, const bool collision_enable) override;
    yarp::dev::ReturnValue makeFrame(const std::string& id_name, const double size, const yarp::sig::Pose6D& pose, const yarp::sig::ColorRGB& color, const std::string& frame_name, const bool gravity_enable, const bool collision_enable) override;
    yarp::dev::ReturnValue changeColor(const std::string& id, const yarp::sig::ColorRGB& color) override;
    yarp::dev::ReturnValue setPose(const std::string& id, const yarp::sig::Pose6D& pose, const std::string& frame_name = "") override;
    yarp::dev::ReturnValue enableGravity(const std::string& id, const bool enable) override;
    yarp::dev::ReturnValue enableCollision(const std::string& id, const bool enable) override;
    getPoseReturnValue     getPose(const std::string& id, const std::string& frame_name) override;
    yarp::dev::ReturnValue makeModel(const std::string& id_name, const std::string& filename, const yarp::sig::Pose6D& pose, const std::string& frame_name, const bool gravity_enable, const bool collision_enable) override;
    yarp::dev::ReturnValue deleteObject(const std::string& id) override;
    yarp::dev::ReturnValue deleteAll() override;
    getListReturnValue     getList() override;
    yarp::dev::ReturnValue attach(const std::string& id, const std::string& link_name) override;
    yarp::dev::ReturnValue detach(const std::string& id) override;
    yarp::dev::ReturnValue rename(const std::string& old_name, const std::string& new_name) override;

    ISimulatedWorldMsgsImpl   (yarp::dev::ISimulatedWorld* iSim);
    ~ISimulatedWorldMsgsImpl() = default;

public:
    std::mutex* getMutex() { return &m_mutex; }
};


 /**
 *  @ingroup dev_impl_nws_yarp
 *
 * \brief `SimulatedWorld_nws_yarp`: Device that expose a simulator engine (using the ISimulatedWorld interface) on the YARP network.
 *
 * Parameters required by this device are shown in class: SimulatedWorld_nws_yarp_ParamsParser
 */
class SimulatedWorld_nws_yarp :
        public yarp::dev::DeviceDriver,
        public yarp::dev::WrapperSingle,
        public yarp::os::PortReader,
        SimulatedWorld_nws_yarp_ParamsParser
{
public:
    SimulatedWorld_nws_yarp();
    ~SimulatedWorld_nws_yarp();

    bool open(yarp::os::Searchable &params) override;
    bool close() override;

private:
    yarp::dev::ISimulatedWorld*               m_Isim_p = nullptr;
    std::unique_ptr<ISimulatedWorldMsgsImpl>  m_msgsImpl;

    //ports stuff
    std::string m_rpcPortName;
    yarp::os::Port m_rpcPort;

    //public methods
    bool read(yarp::os::ConnectionReader& connection) override;

private:
    //private methods
    bool attach(yarp::dev::PolyDriver* driver) override;
    bool detach() override;
};

#endif // YARP_DEV_SIMULATEDWORDNWSYARP_H

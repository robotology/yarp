/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/ISimulatedWorld.h>
#include <yarp/os/Stamp.h>
#include "FakeSimulatedWorld_ParamsParser.h"

/**
* @ingroup dev_impl_fake
* \brief `fakeSimulatedWorld` : fake device implementing the ISimulatedWorld interface.
*
* Parameters required by this device are shown in class: FakeSimulatedWorld_ParamsParser
*
*/
class FakeSimulatedWorld :
        public yarp::dev::DeviceDriver,
        public yarp::os::PeriodicThread,
        public yarp::dev::ISimulatedWorld,
        public FakeSimulatedWorld_ParamsParser
{
public:
    FakeSimulatedWorld();
    FakeSimulatedWorld(const FakeSimulatedWorld&) = delete;
    FakeSimulatedWorld(FakeSimulatedWorld&&) = delete;
    FakeSimulatedWorld& operator=(const FakeSimulatedWorld&) = delete;
    FakeSimulatedWorld& operator=(FakeSimulatedWorld&&) = delete;

    ~FakeSimulatedWorld() override;

    // Device Driver interface
    bool open(yarp::os::Searchable &config) override;
    bool close() override;

    /* ISimulatedWorld methods */
    yarp::dev::ReturnValue makeSphere (std::string id, double radius, yarp::sig::Pose6D pose, yarp::sig::ColorRGB color, std::string frame_name, bool gravity_enable, bool collision_enable)  override;
    yarp::dev::ReturnValue makeBox (std::string id, double width, double height, double thickness, yarp::sig::Pose6D pose, yarp::sig::ColorRGB color, std::string frame_name, bool gravity_enable, bool collision_enable)  override;
    yarp::dev::ReturnValue makeCylinder (std::string id, double radius, double length, yarp::sig::Pose6D pose, yarp::sig::ColorRGB color, std::string frame_name, bool gravity_enable, bool collision_enable)  override;
    yarp::dev::ReturnValue makeFrame (std::string id, double size, yarp::sig::Pose6D pose, yarp::sig::ColorRGB color, std::string frame_name, bool gravity_enable, bool collision_enable)  override;
    yarp::dev::ReturnValue changeColor (std::string id, yarp::sig::ColorRGB color)  override;
    yarp::dev::ReturnValue setPose(std::string id, yarp::sig::Pose6D pose, std::string frame_name)  override;
    yarp::dev::ReturnValue enableGravity (std::string id, bool enable)  override;
    yarp::dev::ReturnValue enableCollision (std::string id, bool enable)  override;
    yarp::dev::ReturnValue getPose(std::string id, yarp::sig::Pose6D& pose,  std::string frame_name)  override;
    yarp::dev::ReturnValue makeModel(std::string id,  std::string filename, yarp::sig::Pose6D pose,  std::string frame_name, bool gravity_enable, bool collision_enable)  override;
    yarp::dev::ReturnValue deleteObject(std::string id)  override;
    yarp::dev::ReturnValue deleteAll()  override;
    yarp::dev::ReturnValue getList(std::vector<std::string>& names)  override;
    yarp::dev::ReturnValue attach(std::string id, std::string link_name)  override;
    yarp::dev::ReturnValue detach(std::string id)  override;
    yarp::dev::ReturnValue rename(std::string old_name, std::string new_name) override;

private:
    bool threadInit() override;
    void run() override;
};

/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FakeSimulatedWorld.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

#include <string>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

namespace {
YARP_LOG_COMPONENT(FAKESIMULATEDWORLD, "yarp.device.fakeSimulatedWorld")
}

FakeSimulatedWorld::FakeSimulatedWorld() :
        PeriodicThread(1.0)
{
}

FakeSimulatedWorld::~FakeSimulatedWorld()
{
    close();
}

bool FakeSimulatedWorld::open(yarp::os::Searchable &config)
{
    if (!this->parseParams(config)) {return false;}

    start();
    return true;
}

bool FakeSimulatedWorld::close()
{
    FakeSimulatedWorld::stop();
    return true;
}

bool FakeSimulatedWorld::threadInit()
{
    return true;
}

void FakeSimulatedWorld::run()
{
}


ReturnValue FakeSimulatedWorld::makeSphere (std::string id, double radius, yarp::sig::Pose6D pose, yarp::sig::ColorRGB color, std::string frame_name, bool gravity_enable, bool collision_enable)
{
   return ReturnValue_ok;
}

ReturnValue FakeSimulatedWorld::makeBox (std::string id, double width, double height, double thickness, yarp::sig::Pose6D pose, yarp::sig::ColorRGB color, std::string frame_name, bool gravity_enable, bool collision_enable)
{
   return ReturnValue_ok;
}

ReturnValue FakeSimulatedWorld::makeCylinder (std::string id, double radius, double length, yarp::sig::Pose6D pose, yarp::sig::ColorRGB color, std::string frame_name, bool gravity_enable, bool collision_enable)
{
   return ReturnValue_ok;
}

ReturnValue FakeSimulatedWorld::makeFrame (std::string id, double size, yarp::sig::Pose6D pose, yarp::sig::ColorRGB color, std::string frame_name, bool gravity_enable, bool collision_enable)
{
   return ReturnValue_ok;
}

ReturnValue FakeSimulatedWorld::changeColor (std::string id, yarp::sig::ColorRGB color)
{
   return ReturnValue_ok;
}

ReturnValue FakeSimulatedWorld::setPose(std::string id, yarp::sig::Pose6D pose, std::string frame_name)
{
   return ReturnValue_ok;
}

ReturnValue FakeSimulatedWorld::enableGravity (std::string id, bool enable)
{
   return ReturnValue_ok;
}

ReturnValue FakeSimulatedWorld::enableCollision (std::string id, bool enable)
{
   return ReturnValue_ok;
}

ReturnValue FakeSimulatedWorld::getPose(std::string id, yarp::sig::Pose6D& pose,  std::string frame_name)
{
   return ReturnValue_ok;
}

ReturnValue FakeSimulatedWorld::makeModel(std::string id,  std::string filename, yarp::sig::Pose6D pose,  std::string frame_name, bool gravity_enable, bool collision_enable)
{
   return ReturnValue_ok;
}

ReturnValue FakeSimulatedWorld::deleteObject(std::string id)
{
   return ReturnValue_ok;
}

ReturnValue FakeSimulatedWorld::deleteAll()
{
   return ReturnValue_ok;
}

ReturnValue FakeSimulatedWorld::getList(std::vector<std::string>& names)
{
   return ReturnValue_ok;
}

ReturnValue FakeSimulatedWorld::attach(std::string id, std::string link_name)
{
   return ReturnValue_ok;
}

ReturnValue FakeSimulatedWorld::detach(std::string id)
{
   return ReturnValue_ok;
}

ReturnValue FakeSimulatedWorld::rename(std::string old_name, std::string new_name)
{
   return ReturnValue_ok;
}

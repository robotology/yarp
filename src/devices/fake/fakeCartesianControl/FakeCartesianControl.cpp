/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FakeCartesianControl.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>

#include <iostream>
#include <cstring>

using namespace yarp::os;
using namespace yarp::dev;

namespace {
YARP_LOG_COMPONENT(FAKECARTESIANCONTROL, "yarp.device.fakeCartesianControl")
constexpr double default_period = 0.02;
}

FakeCartesianControl::FakeCartesianControl() :
        PeriodicThread(default_period)
{
}

bool FakeCartesianControl::open(yarp::os::Searchable& config)
{
   // if (!this->parseParams(config)) {return false;}
  //  setPeriod(m_period);

    PeriodicThread::start();

    return true;
}

bool FakeCartesianControl::close()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    // Stop the thread
    PeriodicThread::stop();

    return true;
}

void FakeCartesianControl::run()
{
    std::lock_guard<std::mutex> lock(m_mutex);
}

ReturnValue FakeCartesianControl::setTrackingMode(const bool f)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::getTrackingMode(bool *f)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::setReferenceMode(const bool f)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::getReferenceMode(bool *f)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::setPosePriority(const std::string &p)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::getPosePriority(std::string &p)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::getPose(yarp::sig::Vector &x, yarp::sig::Vector &o,
                                   yarp::os::Stamp *stamp)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::getPose(const int axis, yarp::sig::Vector &x,
                                   yarp::sig::Vector &o,
                                   yarp::os::Stamp *stamp)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::goToPose(const yarp::sig::Vector &xd,
                                    const yarp::sig::Vector &od,
                                    const double t)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::goToPosition(const yarp::sig::Vector &xd,
                                        const double t)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::goToPoseSync(const yarp::sig::Vector &xd,
                                        const yarp::sig::Vector &od,
                                        const double t)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::goToPositionSync(const yarp::sig::Vector &xd,
                                            const double t)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::getDesired(yarp::sig::Vector &xdhat,
                                      yarp::sig::Vector &odhat,
                                      yarp::sig::Vector &qdhat)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::askForPose(const yarp::sig::Vector &xd,
                                      const yarp::sig::Vector &od,
                                      yarp::sig::Vector &xdhat,
                                      yarp::sig::Vector &odhat,
                                      yarp::sig::Vector &qdhat)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::askForPose(const yarp::sig::Vector &q0,
                                      const yarp::sig::Vector &xd,
                                      const yarp::sig::Vector &od,
                                      yarp::sig::Vector &xdhat,
                                      yarp::sig::Vector &odhat,
                                      yarp::sig::Vector &qdhat)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::askForPosition(const yarp::sig::Vector &xd,
                                          yarp::sig::Vector &xdhat,
                                          yarp::sig::Vector &odhat,
                                          yarp::sig::Vector &qdhat)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::askForPosition(const yarp::sig::Vector &q0,
                                          const yarp::sig::Vector &xd,
                                          yarp::sig::Vector &xdhat,
                                          yarp::sig::Vector &odhat,
                                          yarp::sig::Vector &qdhat)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::getDOF(yarp::sig::Vector &curDof)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::setDOF(const yarp::sig::Vector &newDof,
                                  yarp::sig::Vector &curDof)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::getRestPos(yarp::sig::Vector &curRestPos)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::setRestPos(const yarp::sig::Vector &newRestPos,
                                       yarp::sig::Vector &curRestPos)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::getRestWeights(yarp::sig::Vector &curRestWeights)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::setRestWeights(const yarp::sig::Vector &newRestWeights,
                                          yarp::sig::Vector &curRestWeights)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::getLimits(const int axis, double *min, double *max)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::setLimits(const int axis, const double min, const double max)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::getTrajTime(double *t)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::setTrajTime(const double t)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::getInTargetTol(double *tol)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::setInTargetTol(const double tol)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::getJointsVelocities(yarp::sig::Vector &qdot)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::getTaskVelocities(yarp::sig::Vector &xdot,
                           yarp::sig::Vector &odot)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::setTaskVelocities(const yarp::sig::Vector &xdot,
                                             const yarp::sig::Vector &odot)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::attachTipFrame(const yarp::sig::Vector &x,
                                          const yarp::sig::Vector &o)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::getTipFrame(yarp::sig::Vector &x, yarp::sig::Vector &o)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::removeTipFrame()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::checkMotionDone(bool *f)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::waitMotionDone(const double period,
                                          const double timeout)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::stopControl()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::storeContext(int *id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::restoreContext(const int id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::deleteContext(const int id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::getInfo(yarp::os::Bottle &info)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::registerEvent(CartesianEvent &event)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::unregisterEvent(CartesianEvent &event)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::tweakSet(const yarp::os::Bottle &options)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

ReturnValue FakeCartesianControl::tweakGet(yarp::os::Bottle &options)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return ReturnValue_ok;
}

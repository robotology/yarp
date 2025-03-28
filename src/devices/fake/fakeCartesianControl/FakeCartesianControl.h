/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_FAKECARTESIANCONTROL_H
#define YARP_FAKECARTESIANCONTROL_H

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/RpcServer.h>

#include <yarp/dev/CartesianControl.h>
#include <yarp/dev/PolyDriver.h>

#include <mutex>

//#include "FakeCartesianControl_ParamsParser.h"

 /**
 * @ingroup dev_impl_fake
 *
 * \brief `fakeCartesianControl`: Documentation to be added
 *
 * Parameters required by this device are shown in class: FakeCartesianControl_ParamsParser
 */
class FakeCartesianControl :
        public yarp::os::PeriodicThread,
        public yarp::dev::ICartesianControl,
        public yarp::dev::DeviceDriver
   //     public FakeCartesianControl_ParamsParser
{
protected:
    std::mutex m_mutex;

public:
    FakeCartesianControl();
    FakeCartesianControl(const FakeCartesianControl&) = delete;
    FakeCartesianControl(FakeCartesianControl&&) = delete;
    FakeCartesianControl& operator=(const FakeCartesianControl&) = delete;
    FakeCartesianControl& operator=(FakeCartesianControl&&) = delete;

    ~FakeCartesianControl() override = default;

    // yarp::dev::DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    // yarp::os::PeriodicThread
    void run() override;

    // yarp::dev::ICartesianControl
    yarp::dev::ReturnValue setTrackingMode(const bool f) override;
    yarp::dev::ReturnValue getTrackingMode(bool *f) override;
    yarp::dev::ReturnValue setReferenceMode(const bool f) override;
    yarp::dev::ReturnValue getReferenceMode(bool *f) override;
    yarp::dev::ReturnValue setPosePriority(const std::string &p) override;
    yarp::dev::ReturnValue getPosePriority(std::string &p)  override;
    yarp::dev::ReturnValue getPose(yarp::sig::Vector &x, yarp::sig::Vector &o,
                                   yarp::os::Stamp *stamp=NULL)  override;
    yarp::dev::ReturnValue getPose(const int axis, yarp::sig::Vector &x,
                                   yarp::sig::Vector &o,
                                   yarp::os::Stamp *stamp=NULL)  override;
    yarp::dev::ReturnValue goToPose(const yarp::sig::Vector &xd,
                                    const yarp::sig::Vector &od,
                                    const double t = 0.0)  override;
    yarp::dev::ReturnValue goToPosition(const yarp::sig::Vector &xd,
                                        const double t = 0.0)  override;
    yarp::dev::ReturnValue goToPoseSync(const yarp::sig::Vector &xd,
                                        const yarp::sig::Vector &od,
                                        const double t = 0.0)  override;
    yarp::dev::ReturnValue goToPositionSync(const yarp::sig::Vector &xd,
                                            const double t = 0.0)  override;
    yarp::dev::ReturnValue getDesired(yarp::sig::Vector &xdhat,
                                      yarp::sig::Vector &odhat,
                                      yarp::sig::Vector &qdhat)  override;
    yarp::dev::ReturnValue askForPose(const yarp::sig::Vector &xd,
                                      const yarp::sig::Vector &od,
                                      yarp::sig::Vector &xdhat,
                                      yarp::sig::Vector &odhat,
                                      yarp::sig::Vector &qdhat)  override;
    yarp::dev::ReturnValue askForPose(const yarp::sig::Vector &q0,
                                      const yarp::sig::Vector &xd,
                                      const yarp::sig::Vector &od,
                                      yarp::sig::Vector &xdhat,
                                      yarp::sig::Vector &odhat,
                                      yarp::sig::Vector &qdhat)  override;
    yarp::dev::ReturnValue askForPosition(const yarp::sig::Vector &xd,
                                          yarp::sig::Vector &xdhat,
                                          yarp::sig::Vector &odhat,
                                          yarp::sig::Vector &qdhat)  override;
    yarp::dev::ReturnValue askForPosition(const yarp::sig::Vector &q0,
                                          const yarp::sig::Vector &xd,
                                          yarp::sig::Vector &xdhat,
                                          yarp::sig::Vector &odhat,
                                          yarp::sig::Vector &qdhat)  override;
    yarp::dev::ReturnValue getDOF(yarp::sig::Vector &curDof)  override;
    yarp::dev::ReturnValue setDOF(const yarp::sig::Vector &newDof,
                                  yarp::sig::Vector &curDof)  override;
    yarp::dev::ReturnValue getRestPos(yarp::sig::Vector &curRestPos)  override;
    yarp::dev::ReturnValue setRestPos(const yarp::sig::Vector &newRestPos,
                                       yarp::sig::Vector &curRestPos)  override;
    yarp::dev::ReturnValue getRestWeights(yarp::sig::Vector &curRestWeights)  override;
    yarp::dev::ReturnValue setRestWeights(const yarp::sig::Vector &newRestWeights,
                                          yarp::sig::Vector &curRestWeights)  override;
    yarp::dev::ReturnValue getLimits(const int axis, double *min, double *max)  override;
    yarp::dev::ReturnValue setLimits(const int axis, const double min, const double max)  override;
    yarp::dev::ReturnValue getTrajTime(double *t) override;
    yarp::dev::ReturnValue setTrajTime(const double t) override;
    yarp::dev::ReturnValue getInTargetTol(double *tol) override;
    yarp::dev::ReturnValue setInTargetTol(const double tol)  override;
    yarp::dev::ReturnValue getJointsVelocities(yarp::sig::Vector &qdot)  override;
    yarp::dev::ReturnValue getTaskVelocities(yarp::sig::Vector &xdot,
                           yarp::sig::Vector &odot)  override;
    yarp::dev::ReturnValue setTaskVelocities(const yarp::sig::Vector &xdot,
                                             const yarp::sig::Vector &odot)  override;
    yarp::dev::ReturnValue attachTipFrame(const yarp::sig::Vector &x,
                                          const yarp::sig::Vector &o)  override;
    yarp::dev::ReturnValue getTipFrame(yarp::sig::Vector &x, yarp::sig::Vector &o)  override;
    yarp::dev::ReturnValue removeTipFrame()  override;
    yarp::dev::ReturnValue checkMotionDone(bool *f)  override;
    yarp::dev::ReturnValue waitMotionDone(const double period = 0.1,
                                          const double timeout = 0.0)  override;
    yarp::dev::ReturnValue stopControl()  override;
    yarp::dev::ReturnValue storeContext(int *id)  override;
    yarp::dev::ReturnValue restoreContext(const int id)  override;
    yarp::dev::ReturnValue deleteContext(const int id) override;
    yarp::dev::ReturnValue getInfo(yarp::os::Bottle &info) override;
    yarp::dev::ReturnValue registerEvent(yarp::dev::CartesianEvent &event)  override;
    yarp::dev::ReturnValue unregisterEvent(yarp::dev::CartesianEvent &event)  override;
    yarp::dev::ReturnValue tweakSet(const yarp::os::Bottle &options)  override;
    yarp::dev::ReturnValue tweakGet(yarp::os::Bottle &options)  override;
};

#endif

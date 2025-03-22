/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ICARTESIANTEST_H
#define ICARTESIANTEST_H

#include <yarp/dev/CartesianControl.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::os;

namespace yarp::dev::tests {
inline void exec_iCartesian_test_1(ICartesianControl* icart)
{
    REQUIRE(icart != nullptr);

    {
        yarp::dev::ReturnValue r;
        yarp::sig::Vector q0;
        yarp::sig::Vector xd;
        yarp::sig::Vector od;
        yarp::sig::Vector xdhat;
        yarp::sig::Vector odhat;
        yarp::sig::Vector qdhat;
        r = icart->askForPose(q0,
                              xd,
                              od,
                              xdhat,
                              odhat,
                              qdhat);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        yarp::sig::Vector q0;
        yarp::sig::Vector xd;
        yarp::sig::Vector xdhat;
        yarp::sig::Vector odhat;
        yarp::sig::Vector qdhat;
        r = icart->askForPosition(q0,
                                  xd,
                                  xdhat,
                                  odhat,
                                  qdhat);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        yarp::sig::Vector x;
        yarp::sig::Vector o;
        r = icart->attachTipFrame(x,o);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        bool done;
        r = icart->checkMotionDone(&done);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        int id = 0;
        r = icart->deleteContext(id);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        yarp::sig::Vector xdhat;
        yarp::sig::Vector odhat;
        yarp::sig::Vector qdhat;
        r = icart->getDesired(xdhat,odhat,qdhat);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        yarp::sig::Vector dof;
        r = icart->getDOF(dof);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        yarp::os::Bottle info;
        r = icart->getInfo(info);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        double tol = 0;
        r = icart->getInTargetTol(&tol);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        yarp::sig::Vector qdot;
        r = icart->getJointsVelocities(qdot);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        int axis = 0;
        double min = 0;
        double max = 0;
        r = icart->getLimits(axis,&min,&max);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        int axis = 0;
        yarp::sig::Vector x;
        yarp::sig::Vector o;
        yarp::os::Stamp stamp;
        r = icart->getPose(axis, x, o, &stamp);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        std::string s;
        r = icart->getPosePriority(s);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        bool mode;
        r = icart->getReferenceMode(&mode);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        yarp::sig::Vector curRestPos;
        r = icart->getRestPos(curRestPos);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        yarp::sig::Vector curRestWeights;
        r = icart->getRestWeights(curRestWeights);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        yarp::sig::Vector xdot;
        yarp::sig::Vector odot;
        r = icart->setTaskVelocities(xdot,odot);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        yarp::sig::Vector x;
        yarp::sig::Vector o;
        r = icart->getTipFrame(x,o);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        bool mode;
        r = icart->getTrackingMode(&mode);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        double t = 0;
        r = icart->getTrajTime(&t);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        yarp::sig::Vector xd;
        yarp::sig::Vector od;
        double t = 1;
        r = icart->goToPose(xd,od,t);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        yarp::sig::Vector xd;
        yarp::sig::Vector od;
        double t = 1;
        r = icart->goToPoseSync(xd,od,t);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        yarp::sig::Vector xd;
        double t = 1;
        r = icart->goToPosition(xd,t);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        yarp::sig::Vector xd;
        double t = 1;
        r = icart->goToPositionSync(xd,t);
        CHECK(r);
    }
    {
        /* TO BE COMPLETED
        yarp::dev::ReturnValue r;
        CartesianEvent event;
        r = icart->registerEvent(event);
        CHECK(r);
        */
    }
    {
        yarp::dev::ReturnValue r;
        r = icart->removeTipFrame();
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        int id = 0;
        r = icart->restoreContext(id);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        yarp::sig::Vector newDof;
        yarp::sig::Vector curDof;
        r = icart->setDOF(newDof, curDof);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        double tol = 0;
        r = icart->setInTargetTol(tol);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        int axis = 0;
        double min = 0;
        double max = 0;
        r = icart->setLimits(axis,min,max);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        std::string p;
        r = icart->setPosePriority(p);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        bool mode = true;
        r = icart->setReferenceMode(mode);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        yarp::sig::Vector newRestPos;
        yarp::sig::Vector curRestPos;
        r = icart->setRestPos(newRestPos,curRestPos);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        yarp::sig::Vector newRestWeights;
        yarp::sig::Vector curRestWeights;
        r = icart->setRestWeights(newRestWeights,curRestWeights);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        yarp::sig::Vector newRestWeights;
        yarp::sig::Vector curRestWeights;
        r = icart->setTaskVelocities(newRestWeights,curRestWeights);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        bool mode = true;
        r = icart->setTrackingMode(mode);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        double t = 1;
        r = icart->setTrajTime(t);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        r = icart->stopControl();
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        int id = 0;
        r = icart->storeContext(&id);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        yarp::os::Bottle option;
        r = icart->tweakGet(option);
        CHECK(r);
    }
    {
        yarp::dev::ReturnValue r;
        yarp::os::Bottle option;
        r = icart->tweakSet(option);
        CHECK(r);
    }
    {
        /* TO BE COMPLETED
        yarp::dev::ReturnValue r;
        CartesianEvent event;
        r = icart->unregisterEvent(&event);
        CHECK(r);
        */
    }
    {
        yarp::dev::ReturnValue r;
        double period = 1;
        double timeout = 2;
        r = icart->waitMotionDone(period,timeout);
        CHECK(r);
    }
}

}

#endif

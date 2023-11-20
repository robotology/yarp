/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ICONTROLMODETEST_H
#define ICONTROLMODETEST_H

#include <yarp/os/SystemClock.h>
#include <yarp/dev/IControlMode.h>
#include <yarp/dev/IAxisInfo.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::os;

#define COMMAND_DELAY 1.0

void test_single_joint(IControlMode* icmd, IAxisInfo* iinfo, int mode)
{
    REQUIRE(icmd != nullptr);
    bool b = false;
    b = icmd->setControlMode(0, mode);
    CHECK(b);
    yarp::os::SystemClock::delaySystem(COMMAND_DELAY);
    int ret=0;
    b = icmd->getControlMode(0, &ret);
    CHECK(b);
    CHECK(mode==ret);
}

void test_all_joints(IControlMode* icmd, IAxisInfo* iinfo, int mode)
{
    REQUIRE(icmd != nullptr);
    REQUIRE(iinfo != nullptr);
    int ax = 0; bool bax = false;
    bax = iinfo->getAxes(&ax);
    REQUIRE(bax);
    REQUIRE(ax==2);

    bool b = false;
    int modes[2];
    modes[0]=mode;
    modes[1]=mode;
    b = icmd->setControlModes(modes);
    CHECK(b);
    yarp::os::SystemClock::delaySystem(COMMAND_DELAY);
    int rets[2] = {0,-0};
    b = icmd->getControlModes(rets);
    CHECK(b);
    CHECK(mode == rets[0]);
    CHECK(mode == rets[1]);
}

void test_multi_joint(IControlMode* icmd, IAxisInfo* iinfo, int mode)
{
    REQUIRE(icmd != nullptr);
    REQUIRE(iinfo != nullptr);
    int ax=0; bool bax= false;
    bax=iinfo->getAxes(&ax);
    REQUIRE (bax);
    REQUIRE (ax==2);

    bool b = false;
    int modes[2];
    modes[0] = mode;
    modes[1] = mode;
    int joints[2];
    joints[0] = 0;
    joints[1] = 1;
    b = icmd->setControlModes(2,joints,modes);
    CHECK(b);
    yarp::os::SystemClock::delaySystem(COMMAND_DELAY);
    int rets[2] = { 0,-0 };
    b = icmd->getControlModes(2,joints,rets);
    CHECK(b);
    CHECK(mode == rets[0]);
    CHECK(mode == rets[1]);
}

void test_joint(IControlMode* icmd, IAxisInfo* iinfo, int mode)
{
    REQUIRE(icmd != nullptr);
    test_single_joint(icmd, iinfo, mode);
    test_all_joints(icmd, iinfo, mode);
    test_multi_joint(icmd, iinfo, mode);
}

namespace yarp::dev::tests
{
    inline void exec_iControlMode_test_1(IControlMode* icmd, IAxisInfo* iinfo)
    {
        REQUIRE(icmd != nullptr);
        REQUIRE(iinfo != nullptr);

        bool b= false;
        int ax=0;
        std::string name_ret;
        yarp::dev::JointTypeEnum jointtype_ret;

        b = iinfo->getAxes(&ax);
        CHECK(b); CHECK(ax>0);
        b = iinfo->getAxisName(0, name_ret);
        CHECK(b); CHECK (!name_ret.empty());
        b = iinfo->getJointType(0, jointtype_ret);
        CHECK(b); CHECK (jointtype_ret== yarp::dev::JointTypeEnum::VOCAB_JOINTTYPE_REVOLUTE);

        test_joint(icmd, iinfo, VOCAB_CM_IDLE);
        test_joint(icmd, iinfo, VOCAB_CM_TORQUE);
        test_joint(icmd, iinfo, VOCAB_CM_POSITION);
        test_joint(icmd, iinfo, VOCAB_CM_POSITION_DIRECT);
        test_joint(icmd, iinfo, VOCAB_CM_VELOCITY);
        test_joint(icmd, iinfo, VOCAB_CM_CURRENT);
        test_joint(icmd, iinfo, VOCAB_CM_PWM);
        test_joint(icmd, iinfo, VOCAB_CM_IMPEDANCE_POS);
        test_joint(icmd, iinfo, VOCAB_CM_IMPEDANCE_VEL);
        test_joint(icmd, iinfo, VOCAB_CM_MIXED);
    }
}

#endif

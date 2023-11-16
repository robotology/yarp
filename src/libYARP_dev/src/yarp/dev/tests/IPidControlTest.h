/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IPIDCONTROLTEST_H
#define IPIDCONTROLTEST_H

#include <yarp/dev/IPidControl.h>
#include <yarp/dev/IControlMode.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_iPidControl_test_1(IPidControl* ipid)
    {
        REQUIRE(ipid != nullptr);

        bool b = false;
        yarp::dev::PidControlTypeEnum pp = yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_POSITION;
        b = ipid->disablePid(pp,0);
        CHECK(b);

        b = ipid->enablePid(pp,0);
        CHECK(b);

        yarp::dev::Pid retpid;
        b=ipid->getPid(pp,0, &retpid);
        CHECK(b);

        yarp::dev::Pid setpid;
        yarp::dev::Pid setpid2(retpid);
        b = ipid->setPid(pp, 0, setpid);
        CHECK(b);

        b = ipid->resetPid(pp,0);
        CHECK(b);

        yarp::dev::Pid testpid;
        testpid.setKp(1); CHECK(testpid.kp ==1);
        testpid.setKd(2); CHECK(testpid.kd == 2);
        testpid.setKff(3); CHECK(testpid.kff == 3);
        testpid.setKi(4); CHECK(testpid.ki == 4);
        testpid.setMaxInt(5); CHECK(testpid.max_int == 5);
        testpid.setMaxOut(6); CHECK(testpid.max_output == 6);
        testpid.setOffset(7); CHECK(testpid.offset == 7);
        testpid.setScale(8); CHECK(testpid.scale == 8);
        testpid.setStictionValues(9,10);
        CHECK(testpid.stiction_up_val == 9);
        CHECK(testpid.stiction_down_val==10);

        yarp::dev::Pid testpid2 (testpid);
        CHECK(testpid2 == testpid);

        yarp::dev::Pid emptypid;
        testpid2.clear();
        CHECK(testpid2 == emptypid);
    }

}

#endif

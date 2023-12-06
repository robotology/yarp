/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IPIDCONTROLTEST_H
#define IPIDCONTROLTEST_H

#include <yarp/dev/IPidControl.h>
#include <yarp/dev/IControlMode.h>
#include <yarp/dev/IAxisInfo.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_iPidControl_test_1(IPidControl* ipid, IAxisInfo* iaxis)
    {
        REQUIRE(ipid != nullptr);
        REQUIRE(iaxis != nullptr);

        int ax;
        bool b = false;

        b = iaxis->getAxes(&ax);
        CHECK(b);
        REQUIRE(ax > 0);

        yarp::dev::PidControlTypeEnum pp = yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_POSITION;
        b = ipid->disablePid(pp,0);
        CHECK(b);

        b = ipid->enablePid(pp,0);
        CHECK(b);

        bool enabled;
        b = ipid->isPidEnabled(pp, 0, &enabled);
        CHECK(b);

        yarp::dev::Pid retpid;
        b=ipid->getPid(pp,0, &retpid);
        CHECK(b);

        auto retpids = std::vector<yarp::dev::Pid>(ax);
        b= ipid->getPids(pp, retpids.data());
        CHECK(b);

        yarp::dev::Pid setpid;
        b = ipid->setPid(pp, 0, setpid);
        CHECK(b);

        auto setpids = std::vector<yarp::dev::Pid>(ax);
        b = ipid->setPids(pp, setpids.data());
        CHECK(b);

        b = ipid->resetPid(pp,0);
        CHECK(b);

        b = ipid->setPidOffset(pp,0,0);
        CHECK(b);

        double getpidref=0;
        b = ipid->getPidReference(pp, 0, &getpidref);
        CHECK(b);

        auto getpidrefs = std::vector<double>(ax);
        b = ipid->getPidReferences(pp, getpidrefs.data());
        CHECK(b);

        double setpidref=0;
        b = ipid->setPidReference(pp, 0, setpidref);
        CHECK(b);

        auto setpidrefs = std::vector<double>(ax);
        b = ipid->setPidReferences(pp, setpidrefs.data());
        CHECK(b);

        double errlim=0;
        b = ipid->getPidErrorLimit(pp, 0, &errlim);
        CHECK(b);

        auto errlims = std::vector<double>(ax);
        b = ipid->getPidErrorLimits(pp, errlims.data());
        CHECK(b);
    }

    inline void exec_iPidControl_test_2(IPidControl* ipid)
    {
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

        yarp::dev::Pid testpid3 (1,2,3,4,5,6);
        CHECK(testpid3.kp == 1);
        CHECK(testpid3.kd == 2);
        CHECK(testpid3.ki == 3);
        CHECK(testpid3.max_int == 4);
        CHECK(testpid3.scale == 5);
        CHECK(testpid3.max_output == 6);

        yarp::dev::Pid testpid4 (1,2,3,4,5,6,7,8,9);
        CHECK(testpid4.kp == 1);
        CHECK(testpid4.kd == 2);
        CHECK(testpid4.ki == 3);
        CHECK(testpid4.max_int == 4);
        CHECK(testpid4.scale == 5);
        CHECK(testpid4.max_output == 6);
        CHECK(testpid4.stiction_up_val == 7);
        CHECK(testpid4.stiction_down_val == 8);
        CHECK(testpid4.kff == 9);
    }

}

#endif

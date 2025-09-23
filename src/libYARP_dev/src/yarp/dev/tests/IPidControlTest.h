/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IPIDCONTROLTEST_H
#define IPIDCONTROLTEST_H

#include <yarp/os/Vocab32.h>
#include <yarp/os/Time.h>

#include <yarp/dev/IPidControl.h>
#include <yarp/dev/IControlMode.h>
#include <yarp/dev/IAxisInfo.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::os;

std::vector<yarp::dev::PidControlTypeEnum> pidenums =
{
    yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_POSITION,
    yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_1,
    yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_2,
    yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_3,
    yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY,
    yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_1,
    yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_2,
    yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_3,
    yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE,
    yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_1,
    yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_2,
    yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_3,
    yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT,
    yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_1,
    yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_2,
    yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_3,
    yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_1,
    yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_2,
    yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_3,
    yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_1,
    yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_2,
    yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_3
};

namespace yarp::dev::tests
{
    inline void exec_iPidControl_test_1(IPidControl* ipid, IAxisInfo* iaxis)
    {
        REQUIRE(ipid != nullptr);
        REQUIRE(iaxis != nullptr);

        int ax=0;
        bool b = false;

        b = iaxis->getAxes(&ax);
        CHECK(b);
        REQUIRE(ax > 0);

        yarp::dev::PidControlTypeEnum pp = yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_POSITION;
        bool enabled;
        b = ipid->disablePid(pp,0);
        CHECK(b);
        b = ipid->isPidEnabled(pp, 0, enabled);
        CHECK(b);
        CHECK(!enabled);

        b = ipid->enablePid(pp,0);
        CHECK(b);
        b = ipid->isPidEnabled(pp, 0, enabled);
        CHECK(b);
        CHECK(enabled);

        b = ipid->disablePid(pp,0);
        CHECK(b);
        b = ipid->isPidEnabled(pp, 0, enabled);
        CHECK(b);
        CHECK(!enabled);

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

        b = ipid->setPidOffset(pp,0,42);
        CHECK(b);
        //Streaming message sent on separated thread, delay required before get()
        yarp::os::Time::delay(0.050);

        double pidoff=0;
        b = ipid->getPidOffset(pp,0,pidoff);
        CHECK(b);
        CHECK(pidoff == 42);

        b = ipid->setPidFeedforward(pp,0,43);
        CHECK(b);
        //Streaming message sent on separated thread, delay required before get()
        yarp::os::Time::delay(0.050);

        double pidffd=0;
        b = ipid->getPidFeedforward(pp,0,pidffd);
        CHECK(b);
        CHECK(pidffd == 43);

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

    inline void exec_iPidControl_test_2(IPidControl* ipid, IAxisInfo* iaxis)
    {
        //test one single pid
        {
            for (auto pp : pidenums)
            {
                std::string dv = yarp::os::Vocab32::decode(yarp::conf::vocab32_t(pp));
                INFO("Testing PidControlTypeEnum: " << dv);

                bool b = false;
                yarp::dev::PidExtraInfo infopid;
                yarp::dev::Pid retpid;
                yarp::dev::Pid emptypid;
                yarp::dev::Pid testpid(0, 1, 2, 3, 4, 5, 6, 7, 8);

                b=ipid->setPid(pp,0, emptypid);
                CHECK(b);

                b=ipid->getPid(pp,0, &retpid);
                CHECK(b);
                CHECK(retpid == emptypid);

                b=ipid->setPid(pp,0, testpid);
                CHECK(b);

                b=ipid->getPid(pp,0, &retpid);
                CHECK(b);
                CHECK(retpid == testpid);

                b = ipid->getPidExtraInfo(pp,0, infopid);
                CHECK(b);
            }
        }

        //test multiple pids
        {
            for (auto pp : pidenums)
            {
                std::string dv = yarp::os::Vocab32::decode(yarp::conf::vocab32_t(pp));
                INFO("Testing PidControlTypeEnum: " << dv);

                bool b = false;
                int ax=0;
                b = iaxis->getAxes(&ax);
                CHECK(b);
                REQUIRE(ax > 0);

                std::vector<yarp::dev::Pid> retpids(ax);
                std::vector<yarp::dev::Pid> emptypids(ax);
                std::vector<yarp::dev::Pid> testpids(ax);
                std::vector<yarp::dev::PidExtraInfo> infopids(ax);
                testpids[0] = yarp::dev::Pid(0, 1, 2, 3, 4, 5, 6, 7, 8);
                testpids[1] = yarp::dev::Pid(10, 11, 12, 13, 14, 15, 16, 17, 18);

                b=ipid->setPids(pp, emptypids.data());
                CHECK(b);

                b=ipid->getPids(pp, retpids.data());
                CHECK(b);
                CHECK(retpids != testpids);

                b=ipid->setPids(pp, testpids.data());
                CHECK(b);

                b=ipid->getPids(pp, retpids.data());
                CHECK(b);
                CHECK(retpids == testpids);

                b = ipid->getPidExtraInfos(pp, infopids);
                CHECK(b);
            }
        }
    }


}

#endif

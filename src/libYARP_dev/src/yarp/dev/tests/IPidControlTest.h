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

#include "Utils.h"

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
    yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_3,
    yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_1,
    yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_2,
    yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_3
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
        yarp::dev::PidControlTypeEnum vv = yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY;

        // Pid enable/disable
        {
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
        }

        // Pid set/get
        {
            yarp::dev::Pid retpid;
            yarp::dev::Pid setpid;

            b=ipid->getPid(pp,0, &retpid);
            CHECK(b);

            b = ipid->setPid(pp, 0, setpid);
            CHECK(b);

            b=ipid->getPid(pp,0, &retpid);
            CHECK(b);
            CHECK(retpid == setpid);
        }

        // Pids set/get
        {
            auto retpids = std::vector<yarp::dev::Pid>(ax);
            auto setpids = std::vector<yarp::dev::Pid>(ax);
            setpids[0] = yarp::dev::Pid(0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0);
            setpids[1] = yarp::dev::Pid(0.1, 1.1, 2.1, 3.1, 4.1, 5.1, 6.1, 7.1, 8.1);

            b= ipid->getPids(pp, retpids.data());
            CHECK(b);

            b = ipid->setPids(pp, setpids.data());
            CHECK(b);

            b= ipid->getPids(pp, retpids.data());
            CHECK(b);
            CHECK(retpids == setpids);
        }

        b = ipid->resetPid(pp,0);
        CHECK(b);

        //PidOffset
        {
            double pidoff=0;
            b = ipid->getPidOffset(pp,0,pidoff);
            CHECK(b);
            CHECK(pidoff == 0);
            b = ipid->setPidOffset(pp,0,42);
            CHECK(b);
            //Streaming message sent on separated thread, delay required before get()
            yarp::os::Time::delay(0.050);
            b = ipid->getPidOffset(pp,0,pidoff);
            CHECK(b);
            CHECK(pidoff == 42);
        }

        //PidFeedforward
        {
            double pidffd=0;
            b = ipid->getPidFeedforward(pp,0,pidffd);
            CHECK(b);
            CHECK(pidffd == 0);
            b = ipid->setPidFeedforward(pp,0,43);
            CHECK(b);
            //Streaming message sent on separated thread, delay required before get()
            yarp::os::Time::delay(0.050);
            b = ipid->getPidFeedforward(pp,0,pidffd);
            CHECK(b);
            CHECK(pidffd == 43);
        }

        //PidReference
        {
            double getpidref=0;
            b = ipid->getPidReference(pp, 0, &getpidref);
            CHECK(b);
            CHECK(getpidref == 0.0);
            b = ipid->setPidReference(pp, 0, 10.5);
            CHECK(b);
            b = ipid->getPidReference(pp, 0, &getpidref);
            CHECK(b);
            CHECK(getpidref ==  10.5);
        }

        //PidReferences
        {
            auto setpidrefs = std::vector<double>(ax);
            yarp::dev::tests::set_vector_crescent(setpidrefs, 11.1);
            auto pidrefs = std::vector<double>(ax);
            b = ipid->getPidReferences(pp, pidrefs.data());
            CHECK(b);
            b = ipid->setPidReferences(vv, setpidrefs.data());
            CHECK(b);
            b = ipid->getPidReferences(vv, pidrefs.data());
            CHECK(b);
            CHECK(setpidrefs == pidrefs);
        }

        //Error limit
        {
            double errlim=0;
            b = ipid->getPidErrorLimit(pp, 0, &errlim);
            CHECK(b);
            b = ipid->setPidErrorLimit(pp, 0, 5.0);
            CHECK(b);
            b = ipid->getPidErrorLimit(pp, 0, &errlim);
            CHECK(b);
            CHECK(errlim == 5.0);
        }

        //Error limits
        {
            auto seterrlims = std::vector<double>(ax);
            auto errlims = std::vector<double>(ax);
            yarp::dev::tests::set_vector_crescent(seterrlims,22.2);
            b = ipid->getPidErrorLimits(vv, errlims.data());
            CHECK(b);
            b = ipid->setPidErrorLimits(vv, seterrlims.data());
            CHECK(b);
            b = ipid->getPidErrorLimits(vv, errlims.data());
            CHECK(b);
            CHECK(seterrlims == errlims);
        }
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

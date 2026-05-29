/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IJOYPADCONTROLLERTEST_H
#define IJOYPADCONTROLLERTEST_H

#include <yarp/dev/IJoypadController.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_iJoypadController_test_1(yarp::dev::IJoypadController* iJoy)
    {
        REQUIRE(iJoy != nullptr);

        bool b=false;
        size_t count=0;
        double value=0;

        b = iJoy->getAxisCount(count);
        CHECK(b);
        CHECK(count==4);

        b = iJoy->getButtonCount(count);
        CHECK(b);
        CHECK(count==4);

        b = iJoy->getHatCount(count);
        CHECK(b);
        CHECK(count==4);

        b = iJoy->getStickCount(count);
        CHECK(b);
        CHECK(count==4);

        b = iJoy->getTouchSurfaceCount(count);
        CHECK(b);
        CHECK(count==4);

        b = iJoy->getTrackballCount(count);
        CHECK(b);
        CHECK(count==4);

        //-----

        for (size_t i=0; i<4; i++)
        {
           b = iJoy->getAxis(i, value);
           CHECK(b);
           CHECK(value == i*10);
        }

        for (size_t i=0; i<4; i++)
        {
           b = iJoy->getButton(i, value);
           CHECK(b);
           CHECK(value == i*11);
        }

        for (size_t i=0; i<4; i++)
        {
           unsigned char hatdata;
           b = iJoy->getHat(i, hatdata);
           CHECK(b);
           CHECK(hatdata == i*12);
        }

        for (size_t i=0; i<4; i++)
        {
           StickData sdata;
           b = iJoy->getStick(i, sdata, IJoypadController::JoypadCtrl_coordinateMode::JypCtrlcoord_CARTESIAN);
           CHECK(b);
           CHECK(sdata.s1 == i*100);
           CHECK(sdata.s2 == i*101);
        }
        for (size_t i=0; i<4; i++)
        {
           StickData sdata;
           b = iJoy->getStick(i, sdata, IJoypadController::JoypadCtrl_coordinateMode::JypCtrlcoord_POLAR);
           CHECK(b);
           CHECK(sdata.s1 == i*102);
           CHECK(sdata.s2 == i*103);
        }


        for (size_t i=0; i<4; i++)
        {
           std::vector<yarp::dev::TouchData> tval;
           b = iJoy->getTouch(i, tval);
           CHECK(b);
           REQUIRE(tval.size()==2);
           CHECK(tval[0].x == i * 110);
           CHECK(tval[0].y == i * 111);
           CHECK(tval[1].x == i * 112);
           CHECK(tval[1].y == i * 113);
        }

        for (size_t i=0; i<4; i++)
        {
           TrackballData tdata;
           b = iJoy->getTrackball(i, tdata);
           CHECK(b);
           CHECK(tdata.x == i*120);
           CHECK(tdata.y == i*121);
        }

    }
}

#endif

/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IFRAMEGRABBERCONTROLSTEST_H
#define IFRAMEGRABBERCONTROLSTEST_H

#include <array>
#include <yarp/dev/IFrameGrabberControls.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_IFrameGrabberControls_test_1(IFrameGrabberControls* ictl)
    {
        REQUIRE(ictl);

        // Check busType2String()
        {
            yarp::dev::BusType typeb=BUS_USB;
            std::string s = ictl->busType2String(typeb);
        }
        // Check getActive()
        {
            bool active = false;
            CHECK(ictl->getActive(0, active));
        }

        // Check getCameraDescription()
        {
            yarp::dev::CameraDescriptor camera;
            CHECK(ictl->getCameraDescription(camera));
        }

        // Check getFeature()
        {
            double val = 0;
            CHECK(ictl->getFeature(0, val));
        }

        // Check getMode()
        {
            yarp::dev::FeatureMode mode;
            CHECK(ictl->getMode(0, mode));
        }

        // Check hasAuto()
        {
            bool bauto;
            CHECK(ictl->hasAuto(0, bauto));
        }

        // Check hasFeature()
        {
            bool bfeat;
            CHECK(ictl->hasFeature(0, bfeat));
        }

        // Check hasManual()
        {
            bool bman;
            CHECK(ictl->hasManual(0, bman));
        }

        // Check hasOnePush()
        {
            bool bpush;
            CHECK(ictl->hasOnePush(0, bpush));
        }

        // Check hasOnOff()
        {
            bool bon;
            CHECK(ictl->hasOnOff(0, bon));
        }

        // Check setFeature()
        {
            CHECK(ictl->setFeature(0,0));
            CHECK(ictl->setFeature(0,0,0));
        }

        // Check setMode()
        {
            yarp::dev::FeatureMode mode=MODE_AUTO;
            CHECK(ictl->setMode(0,mode));
        }

        // Check setOnePush()
        {
            int feat = 0;
            CHECK(ictl->setOnePush(feat));
        }

        // Check toFeatureMode()
        {
            bool autob = true;
            CHECK(ictl->toFeatureMode(autob));
        }
    }
}

#endif

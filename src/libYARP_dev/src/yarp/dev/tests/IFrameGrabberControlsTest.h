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
            yarp::dev::BusType typeb = yarp::dev::BusType::BUS_USB;
            std::string s = ictl->busType2String(typeb);
        }
        // Check getActive()
        {
            bool active = false;
            CHECK(ictl->getActive(cameraFeature_id_t::YARP_FEATURE_BRIGHTNESS, active));
        }

        // Check getCameraDescription()
        {
            yarp::dev::CameraDescriptor camera;
            CHECK(ictl->getCameraDescription(camera));
        }

        // Check getFeature()
        {
            double val = 0;
            CHECK(ictl->getFeature(cameraFeature_id_t::YARP_FEATURE_BRIGHTNESS, val));
        }

        // Check getMode()
        {
            yarp::dev::FeatureMode mode;
            CHECK(ictl->getMode(cameraFeature_id_t::YARP_FEATURE_BRIGHTNESS, mode));
        }

        // Check hasAuto()
        {
            bool bauto;
            CHECK(ictl->hasAuto(cameraFeature_id_t::YARP_FEATURE_BRIGHTNESS, bauto));
        }

        // Check hasFeature()
        {
            bool bfeat;
            CHECK(ictl->hasFeature(cameraFeature_id_t::YARP_FEATURE_BRIGHTNESS, bfeat));
        }

        // Check hasManual()
        {
            bool bman;
            CHECK(ictl->hasManual(cameraFeature_id_t::YARP_FEATURE_BRIGHTNESS, bman));
        }

        // Check hasOnePush()
        {
            bool bpush;
            CHECK(ictl->hasOnePush(cameraFeature_id_t::YARP_FEATURE_BRIGHTNESS, bpush));
        }

        // Check hasOnOff()
        {
            bool bon;
            CHECK(ictl->hasOnOff(cameraFeature_id_t::YARP_FEATURE_BRIGHTNESS, bon));
        }

        // Check setFeature()
        {
            CHECK(ictl->setFeature(cameraFeature_id_t::YARP_FEATURE_BRIGHTNESS, 0));
            CHECK(ictl->setFeature(cameraFeature_id_t::YARP_FEATURE_BRIGHTNESS, 0, 0));
        }

        // Check setMode()
        {
            yarp::dev::FeatureMode mode = yarp::dev::FeatureMode::MODE_AUTO;
            CHECK(ictl->setMode(cameraFeature_id_t::YARP_FEATURE_BRIGHTNESS, mode));
        }

        // Check setOnePush()
        {
            auto feat = cameraFeature_id_t::YARP_FEATURE_BRIGHTNESS;
            CHECK(ictl->setOnePush(feat));
        }

        // Check toFeatureMode()
        {
            bool autob = true;
            CHECK(ictl->toFeatureMode(autob) != FeatureMode::MODE_UNKNOWN);
        }
    }
}

#endif

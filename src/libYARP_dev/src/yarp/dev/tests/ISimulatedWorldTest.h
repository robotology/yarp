/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ISIMULATEDWORLDTEST_H
#define ISIMULATEDWORLDTEST_H

#include <memory>
#include <numeric>

#include <yarp/dev/ISimulatedWorld.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_ISimulatedWorld_test_1(ISimulatedWorld* isim)
    {
        REQUIRE(isim != nullptr);

        yarp::dev::ReturnValue ret;

        std::string frame_name;
        yarp::sig::Pose6D pose;
        yarp::sig::ColorRGB color;
        bool gravity_enabled = true;
        bool collision_enabled = true;

        ret = isim->makeSphere("sphere1", 0.1, pose, color,frame_name,gravity_enabled,collision_enabled);
        CHECK(ret);

        ret = isim->makeBox("box1",1,1,1, pose, color,frame_name,gravity_enabled,collision_enabled);
        CHECK(ret);

        ret = isim->makeCylinder("cyl1",1,1, pose, color,frame_name,gravity_enabled,collision_enabled);
        CHECK(ret);

        ret = isim->makeFrame("frame",1, pose, color,frame_name,gravity_enabled,collision_enabled);
        CHECK(ret);

        ret = isim->makeModel("model1", "filename", pose, frame_name,gravity_enabled,collision_enabled);
        CHECK(ret);

        ret = isim->deleteObject("sphere1");
        CHECK(ret);

        ret = isim->deleteAll();
        CHECK(ret);

        ret = isim->setPose("box1", pose);
        CHECK(ret);

        ret = isim->getPose("box1", pose);
        CHECK(ret);

        ret = isim->rename("box1", "box2");
        CHECK(ret);

        std::vector<std::string> objects;
        ret = isim->getList(objects);
        CHECK(ret);

        ret = isim->enableGravity("cyl1", true);
        CHECK(ret);
        ret = isim->enableGravity("cyl1", false);
        CHECK(ret);

        ret = isim->enableCollision("cyl1", true);
        CHECK(ret);
        ret = isim->enableCollision("cyl1", false);
        CHECK(ret);

        ret = isim->changeColor("box1", color);
        CHECK(ret);

        ret = isim->attach("box1", "frame1");
        CHECK(ret);

        ret = isim->detach("box1");
        CHECK(ret);
    }
}

#endif

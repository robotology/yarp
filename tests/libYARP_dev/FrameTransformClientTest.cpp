/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#define _USE_MATH_DEFINES

#include <yarp/sig/Matrix.h>
#include <yarp/sig/Vector.h>
#include <yarp/math/Quaternion.h>
#include <yarp/dev/IFrameSet.h>
#include <yarp/dev/IFrameSource.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/Network.h>
#include <yarp/os/Time.h>
#include <yarp/math/Math.h>

#include <cmath>
#include <vector>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::math;
using namespace std::chrono_literals;

static bool isEqual(const yarp::sig::Vector& v1, const yarp::sig::Vector& v2, double precision)
{
    if (v1.size() != v2.size())
    {
        return false;
    }

    for (size_t i = 0; i < v1.size(); i++)
    {
        double check = fabs(v1[i] - v2[i]);
        if (check > precision)
        {
            return false;
        }
    }
    return true;
}

static bool isEqual(const yarp::math::Quaternion& q1, const yarp::math::Quaternion& q2, double precision)
{
    yarp::sig::Vector v1 = q1.toVector();
    yarp::sig::Vector v2 = q2.toVector();

    for (size_t i = 0; i < v1.size(); i++)
    {
        double check = fabs(v1[i] - v2[i]);
        if (check > precision)
        {
            return false;
        }
    }
    return true;
}

static bool isEqual(const yarp::sig::Matrix& m1, const yarp::sig::Matrix& m2, double precision)
{
    if (m1.cols() != m2.cols() || m1.rows() != m2.rows())
    {
        return false;
    }

    for (size_t i = 0; i < m1.rows(); i++)
    {
        if (!isEqual(m1.getRow(i), m2.getRow(i), precision))
        {
            return false;
        }
    }
    return true;
}



TEST_CASE("dev::FrameTransformClientTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("FrameBroadcaster", "device");
    YARP_REQUIRE_PLUGIN("FrameReceiver",    "device");

#if defined(DISABLE_FAILING_TESTS)
    YARP_SKIP_TEST("Skipping failing tests")
#endif

        Network::setLocalMode(true);
        //Network yarp;
    SECTION("Test the transform client")
    {
        bool precision_verbose = false;

        PolyDriver framebroadcasterpd;
        Property config{ {"device", Value("FrameBroadcaster")}, {"topic",Value("/testframetransform")} };
        REQUIRE(framebroadcasterpd.open(config)); // ddtransformserver open reported successful
        IFrameSet* itfB{ nullptr };
        REQUIRE(framebroadcasterpd.view(itfB));
        REQUIRE(itfB != nullptr);
        
        config.clear();
        PolyDriver framereceiverpd;
        config = { {"device", Value("FrameReceiver")}, {"topic", Value("/testframetransform")} };
        REQUIRE(framereceiverpd.open(config)); // ddtransformclient open reported successful
        IFrameSource* itfR{ nullptr };
        REQUIRE(framereceiverpd.view(itfR));
        REQUIRE(itfR != nullptr);

        REQUIRE(yarp::os::Network::connect("/testframetransform+@/frameBroadcaster0", "/testframetransform-@/frameReceiver0"));
        yarp::sig::Matrix m1(4, 4);
        m1[0][0] = cos(M_PI / 4); m1[0][1] = -sin(M_PI / 4); m1[0][2] = 0; m1[0][3] = 3;
        m1[1][0] = sin(M_PI / 4); m1[1][1] = cos(M_PI /4);   m1[1][2] = 0; m1[1][3] = 1;
        m1[2][0] = 0;             m1[2][1] = 0;              m1[2][2] = 1; m1[2][3] = 2;
        m1[3][0] = 0;             m1[3][1] = 0;              m1[3][2] = 0; m1[3][3] = 1;
        yarp::sig::Matrix m2(4, 4);
        m2[0][0] = cos(M_PI / 4);  m2[0][1] = 0; m2[0][2] = sin(M_PI / 4);  m2[0][3] = 0.1;
        m2[1][0] = 0;              m2[1][1] = 1; m2[1][2] = 0;              m2[1][3] = 0.2;
        m2[2][0] = -sin(M_PI / 4); m2[2][1] = 0; m2[2][2] = cos(M_PI / 4);  m2[2][3] = 0.3;
        m2[3][0] = 0;              m2[3][1] = 0; m2[3][2] = 0;              m2[3][3] = 1;
        yarp::sig::Matrix sibiling(4, 4);
        sibiling[0][0] = 1;              sibiling[0][1] = 0;              sibiling[0][2] = 0;              sibiling[0][3] = 10;
        sibiling[1][0] = 0;              sibiling[1][1] = cos(M_PI / 3);  sibiling[1][2] = -sin(M_PI / 3); sibiling[1][3] = 15;
        sibiling[2][0] = 0;              sibiling[2][1] = sin(M_PI / 3);  sibiling[2][2] =  cos(M_PI / 3); sibiling[2][3] = 5;
        sibiling[3][0] = 0;              sibiling[3][1] = 0;              sibiling[3][2] = 0;              sibiling[3][3] = 1;
        yarp::sig::Matrix m3(4, 4);
        m3 = m1*m2;

        double precision;
        precision = 0.00000001;
        itfB->setTransforms({
            {"frame1", "frame2",   m1, true},
            {"frame2", "frame3",   m2, true},
            {"frame3", "frame4",   m3, true},
            {"frame10", "frame11", m1, true},
            {"frame2", "frame3b",  m2, true},
            {"frame1", "sibiling_test_frame", sibiling, true}
            });

        yarp::sig::Matrix m4(4, 4);
        m4[0][0] = +0.9585267399;  m4[0][1] = -0.2305627908;  m4[0][2] = +0.1675329472;  m4[0][3] = 0.1;
        m4[1][0] = +0.2433237939;  m4[1][1] = +0.9680974922;  m4[1][2] = -0.0598395928;  m4[1][3] = 0.2;
        m4[2][0] = -0.1483914426;  m4[2][1] = +0.0981226021;  m4[2][2] = +0.9840487461;  m4[2][3] = 0.3;
        m4[3][0] = 0;              m4[3][1] = 0;              m4[3][2] = 0;              m4[3][3] = 1;

        yarp::os::Time::delay(1);

        //test 0
        auto ids = itfR->getAllFrameIds();
        
        char buff[1024]; buff[0] = 0;
        for (auto& frameId : ids)
        {
            sprintf(buff +strlen(buff), "%s ", frameId.c_str());
        }
        INFO("Found frames: " << buff);
        bool b_ids = (ids.size() == 8);
        CHECK(b_ids); // getAllFrameIds ok

        //test 1
        auto parentRes = itfR->getParent("frame3");
        CHECK(parentRes.valid);
        CHECK(parentRes.value == "frame2"); // getParent ok


        //test 2
        auto b_gt = itfR->getTransform("frame1", "frame3");
        auto mt   = b_gt.value.toMatrix();
        CHECK(b_gt.valid); // getTransform ok
        CHECK(isEqual(mt, m3, precision));
        if (precision_verbose || b_gt.valid == false)
        {
            INFO("Precision error:\n" + (mt - m3).toString());
        }

        //test3
        CHECK(itfR->frameExists("frame3"));
        CHECK_FALSE(itfR->frameExists("frame3_err")); // frameExists ok

        //test4
        CHECK(itfR->canTransform("frame1", "frame2").value);
        CHECK_FALSE(itfR->canTransform("frame1", "frame11").value); // canTransform ok

        //test4bis
        {
            
            CHECK(itfR->canTransform("frame1", "frame3b").value); // canTransform Bis ok
        }

        //test4 tris (transform between sibilings)
        {
            CHECK(itfR->canTransform("sibiling_test_frame", "frame3").value); // canTransform between sibilings ok
            auto t = itfR->getTransform("sibiling_test_frame", "frame3");
            CHECK(t.valid); // getTransform between sibilings ok
            CHECK(isEqual(t.value.toMatrix(), SE3inv(m2) * SE3inv(m1) * sibiling, precision)); // transform between sibilings ok
        }

        //test 5
        auto t5 = itfR->getTransform("frame3b", "frame1");
        auto mti = t5.value.toMatrix();
        bool b_gt_inv = isEqual(mti, yarp::math::SE3inv(m3), precision);
        CHECK(b_gt_inv); // inverted getTransform ok
        if (precision_verbose || b_gt_inv==false) {
            INFO("Precision error: " + (mti - yarp::math::SE3inv(m3)).toString());
        }

        //test 6
        yarp::sig::Vector in_point1(3), out_point1(3), verPoint1(4);
        yarp::sig::Vector in_pose1(6),  out_pose1(6),  verPose(6);
        yarp::math::Quaternion in_quat1,  out_quat1,   verQuat;

        in_quat1.fromRotationMatrix(m4);

        in_pose1[0] = 1;  in_pose1[1] = 2;  in_pose1[2] = 3;
        in_pose1[3] = 30; in_pose1[4] = 60; in_pose1[5] = 90;

        in_point1[0] = 10; in_point1[1] = 15; in_point1[2] = 5;

        in_point1.push_back(1);
        verPoint1 = m1*m2*in_point1;
        verPoint1.pop_back();
        in_point1.pop_back();

        yarp::sig::Matrix mat(4, 4);
        yarp::sig::Vector temp(3);

        double rot[3]     = { in_pose1[3], in_pose1[4], in_pose1[5] };
        mat               = yarp::math::rpy2dcm(yarp::sig::Vector(3, rot));
        mat[0][3]         = in_pose1[0]; mat[1][3] = in_pose1[1]; mat[2][3] = in_pose1[2];
        mat               = m3 * mat;
        verPose[0]        = mat[0][3]; verPose[1] = mat[1][3]; verPose[2] = mat[2][3];
        temp              = yarp::math::dcm2rpy(mat);
        verPose[3]        = temp[0]; verPose[4] = temp[1]; verPose[5] = temp[2];

        verQuat.fromRotationMatrix(m1 * m2 * m4);

        out_point1 = itfR->transformPoint("frame1", "frame3", in_point1).value;
        out_pose1 = itfR->transformPose("frame1", "frame3", in_pose1).value;
        out_quat1  = itfR->transformQuaternion("frame1", "frame3", in_quat1).value;

        bool b_tpoint = isEqual(verPoint1, out_point1, precision);
        CHECK(b_tpoint); // transformPoint ok
        if (precision_verbose || b_tpoint == false) {
            INFO("Precision error:" << (verPoint1 - out_point1).toString());
        }

        bool b_tpose = isEqual(verPose, out_pose1, precision);
        CHECK(b_tpose); // transformPose ok
        if (precision_verbose || b_tpose == false) {
            INFO("Precision error:" << (verPose - out_pose1).toString());
        }

        bool b_tquat = isEqual(verQuat, out_quat1, precision);
        CHECK(b_tquat); // transformQuaternion ok
        if (precision_verbose || b_tquat == false) {
            INFO("Precision error:" << (verQuat.toVector() - out_quat1.toVector()).toString());
        }

        //test 7
        {
            
            auto all_frames = itfR->allFramesAsString();
            CHECK(!all_frames.empty());
            CHECK(all_frames.find("frame1") != std::string::npos);
            CHECK(all_frames.find("frame2") != std::string::npos);
            CHECK(all_frames.find("frame3") != std::string::npos);
            CHECK(all_frames.find("frame4") != std::string::npos);
            CHECK(all_frames.find("frame10") != std::string::npos);
            CHECK(all_frames.find("frame11") != std::string::npos);
            CHECK(all_frames.find("frame3b") != std::string::npos);
            // allFramesAsString ok
        }

        //test 8
        {
            itfB->setTransform({"frame1", "frame_test", m1});
            yarp::os::Time::delay(1);
            bool del_bool = itfR->frameExists("frame_test");
            yarp::os::Time::delay(1);
            itfR->clearOlderFrames(10ms);
            del_bool &= (!itfR->frameExists("frame_test"));
            CHECK(del_bool); // deleteTransform ok
        }

        //test 9
        {
            itfB->clear();
            itfR->clearOlderFrames(0ms);
            itfR->clearStaticFrames();
            auto cids = itfR->getAllFrameIds();
            CHECK(cids.size() == 0); // clear ok
        }

        //test 10
        {
            itfB->setTransform({"frame10", "frame2", m1});
            yarp::os::Time::delay(0.050);
            CHECK(itfR->canTransform("frame10", "frame2").value); // itf->setTransform ok
            itfR->clearOlderFrames(40ms);
            CHECK_FALSE(itfR->canTransform("frame10", "frame2").value); // itf->setTransform successfully expired after 0.6s
        }

        //test 11
        {
            itfB->clear();
            itfR->clearOlderFrames(0ms);
            itfR->clearStaticFrames();
            bool set_b1 = itfB->setTransform({ "frame10", "frame2", m1 });
            yarp::os::Time::delay(0.050);
            
            auto mt1 = itfR->getTransform("frame10", "frame2").value.toMatrix();
            bool set_b2 = itfB->setTransform({ "frame10", "frame2", m2 });
            yarp::os::Time::delay(0.050);
            
            auto mt2 = itfR->getTransform("frame10", "frame2").value.toMatrix();
            bool a, b;
            a = isEqual(m1, mt1, precision);
            b = isEqual(m2, mt2, precision);
            CHECK(set_b1);
            CHECK(set_b2);
            CHECK(a);
            CHECK(b); // itf->setTransform successfully updated
        }

        //test 11b
        /*{ update: the test is no more valid cause with the new event based infrastructure makes sense to update static transform without deleting them first
            itfB->clear();
            itfR->clearOlderFrames(0ms);
            bool set_b1 = itfB->setTransform({"frame10", "frame2", m1, true});
            yarp::os::Time::delay(0.050);
            
            auto mt1 = itfR->getTransform("frame10", "frame2").value.toMatrix();
            bool set_b2 = itfB->setTransform({ "frame10", "frame2", m2, true });
            yarp::os::Time::delay(0.050);
            
            auto mt2 = itfR->getTransform("frame10", "frame2").value.toMatrix();
            CHECK(set_b1);
            CHECK_FALSE(set_b2);
            CHECK(isEqual(m1, mt1, precision));
            CHECK_FALSE(isEqual(m2, mt2, precision)); // itf->setTransformStatic successfully not-updated
        }*/

        //test 12
        {
            itfB->clear();
            itfR->clearOlderFrames(0ms);
            itfR->clearStaticFrames();
            CHECK(itfB->setTransform({ "frame1", "frame2", m1 }));
            yarp::os::Time::delay(0.050);
            CHECK(itfB->setTransform({ "frame2", "frame3", m2 }));
            yarp::os::Time::delay(0.050);
            CHECK_FALSE(itfB->setTransform({ "frame1", "frame3", m1 }));
            // itf->setTransform duplicate transform successfully skipped

            auto mt1 = itfR->getTransform("frame1", "frame2").value.toMatrix();
            auto mt2 = itfR->getTransform("frame2", "frame3").value.toMatrix();
            auto mt3 = itfR->getTransform("frame1", "frame3").value.toMatrix();
            CHECK(isEqual(mt1, m1, precision));
            CHECK(isEqual(mt2, m2, precision));
            CHECK(isEqual(mt3, (m1*m2), precision));
            // itf->setTransform still working after duplicate transform
        }

        //test 12b
        {
            itfB->clear();
            itfR->clearOlderFrames(std::chrono::seconds(0));
            itfR->clearStaticFrames();
            CHECK(itfB->setTransform({ "frame1", "frame2", m1, true }));
            yarp::os::Time::delay(0.050);
            CHECK(itfB->setTransform({ "frame2", "frame3", m2, true }));
            yarp::os::Time::delay(0.050);
            //CHECK_FALSE(itfB->setTransform({ "frame1", "frame2", m1, true })); it is possible now to update also a static transform without deleting it first. maybe all the test 12b can be dismissed?
            // itf->setTransformStatic duplicate transform successfully skipped
            auto mt1 = itfR->getTransform("frame1", "frame2").value.toMatrix();
            auto mt2 = itfR->getTransform("frame2", "frame3").value.toMatrix();
            auto mt3 = itfR->getTransform("frame1", "frame3").value.toMatrix();
            CHECK(isEqual(mt1, m1, precision));
            CHECK(isEqual(mt2, m2, precision));
            CHECK(isEqual(mt3, (m1*m2), precision));
            // itf->setTransformStatic still working after duplicate transform
        }

        // Close devices
        REQUIRE(yarp::os::Network::disconnect("/testframetransform+@/frameBroadcaster0", "/testframetransform-@/frameReceiver0"));
        CHECK(framereceiverpd.close()); // ddtransformclient successfully closed
        CHECK(framebroadcasterpd.close()); // ddtransformserver successfully closed
    }

    Network::setLocalMode(false);

}

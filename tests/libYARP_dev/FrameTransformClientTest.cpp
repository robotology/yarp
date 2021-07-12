/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define _USE_MATH_DEFINES

#include <yarp/sig/Matrix.h>
#include <yarp/sig/Vector.h>
#include <yarp/math/Quaternion.h>
#include <yarp/dev/IFrameTransform.h>
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
    YARP_REQUIRE_PLUGIN("transformServer", "device");
    YARP_REQUIRE_PLUGIN("transformClient", "device");

#if defined(DISABLE_FAILING_TESTS)
    YARP_SKIP_TEST("Skipping failing tests")
#endif

    Network::setLocalMode(true);

    SECTION("Test the transform client")
    {
        bool precision_verbose = false;

        PolyDriver ddtransformserver;
        Property pTransformserver_cfg;
        pTransformserver_cfg.put("device", "transformServer");
        Property& ros_prop = pTransformserver_cfg.addGroup("ROS");
        ros_prop.put("enable_ros_publisher", "0");
        ros_prop.put("enable_ros_subscriber", "0");
        pTransformserver_cfg.put("transforms_lifetime", 0.500);
        bool ok_server = ddtransformserver.open(pTransformserver_cfg);
        CHECK(ok_server); // ddtransformserver open reported successful

        IFrameTransform* itf = nullptr;
        PolyDriver ddtransformclient;
        Property pTransformclient_cfg;
        pTransformclient_cfg.put("device", "transformClient");
        pTransformclient_cfg.put("local", "/transformClientTest");
        pTransformclient_cfg.put("remote", "/transformServer");
        bool ok_client = ddtransformclient.open(pTransformclient_cfg);
        CHECK(ok_client); // ddtransformclient open reported successful

        bool ok_view = ddtransformclient.view(itf);
        REQUIRE(ok_view);
        REQUIRE(itf != nullptr); // iTransform interface open reported successful

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

        itf->setTransformStatic("frame2", "frame1", m1);
        itf->setTransformStatic("frame3", "frame2", m2);
        itf->setTransformStatic("frame4", "frame3", m3);
        itf->setTransformStatic("frame11", "frame10", m1);
        itf->setTransformStatic("frame3b", "frame2", m2);
        itf->setTransformStatic("sibiling_test_frame", "frame1", sibiling);

        yarp::sig::Matrix m4(4, 4);
        m4[0][0] = +0.9585267399;  m4[0][1] = -0.2305627908;  m4[0][2] = +0.1675329472;  m4[0][3] = 0.1;
        m4[1][0] = +0.2433237939;  m4[1][1] = +0.9680974922;  m4[1][2] = -0.0598395928;  m4[1][3] = 0.2;
        m4[2][0] = -0.1483914426;  m4[2][1] = +0.0981226021;  m4[2][2] = +0.9840487461;  m4[2][3] = 0.3;
        m4[3][0] = 0;              m4[3][1] = 0;              m4[3][2] = 0;              m4[3][3] = 1;

        yarp::os::Time::delay(1);

        //test 0
        std::vector<std::string> ids;
        itf->getAllFrameIds(ids);
        char buff[1024]; buff[0] = 0;
        for (size_t i = 0; i < ids.size(); i++)
        {
            sprintf(buff +strlen(buff), "%s ", ids[i].c_str());
        }
        INFO("Found frames: " << buff);
        bool b_ids = (ids.size() == 8);
        CHECK(b_ids); // getAllFrameIds ok

        //test 1
        std::string parent;
        itf->getParent("frame3", parent);
        CHECK(parent == "frame2"); // getParent ok

        //test 2
        yarp::sig::Matrix mt(4, 4);
        bool b_gt = itf->getTransform("frame3", "frame1", mt);
        isEqual(mt, m3, precision);
        CHECK(b_gt); // getTransform ok
        if (precision_verbose || b_gt==false) {
            INFO("Precision error:\n" + (mt - m3).toString());
        }

        //test3
        CHECK(itf->frameExists("frame3"));
        CHECK_FALSE(itf->frameExists("frame3_err")); // frameExists ok

        //test4
        CHECK(itf->canTransform("frame2", "frame1"));
        CHECK_FALSE(itf->canTransform("frame11", "frame1")); // canTransform ok

        //test4bis
        {
            bool b_canb1;
            b_canb1 = itf->canTransform("frame3b", "frame1");
            CHECK(b_canb1); // canTransform Bis ok
        }

        //test4 tris (transform between sibilings)
        {
            yarp::sig::Matrix sib;
            CHECK(itf->canTransform("sibiling_test_frame", "frame3")); // canTransform between sibilings ok
            CHECK(itf->getTransform("sibiling_test_frame", "frame3", sib)); // getTransform between sibilings ok
            CHECK(isEqual(sib, SE3inv(m2) * SE3inv(m1) * sibiling, precision)); // transform between sibilings ok
        }

        //test 5
        yarp::sig::Matrix mti(4, 4);
        itf->getTransform("frame1", "frame3b", mti);
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

        itf->transformPoint("frame3", "frame1", in_point1, out_point1);
        itf->transformPose("frame3", "frame1", in_pose1, out_pose1);
        itf->transformQuaternion("frame3", "frame1", in_quat1, out_quat1);

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
            std::string all_frames;
            CHECK(itf->allFramesAsString(all_frames));
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
            itf->setTransformStatic("frame_test", "frame1", m1);
            yarp::os::Time::delay(1);
            bool del_bool = itf->frameExists("frame_test");
            itf->deleteTransform("frame_test", "frame1");
            yarp::os::Time::delay(1);
            del_bool &= (!itf->frameExists("frame_test"));
            CHECK(del_bool); // deleteTransform ok
        }

        //test 9
        {
            itf->clear();
            std::vector<std::string> cids;
            itf->getAllFrameIds(cids);
            CHECK(cids.size() == 0); // clear ok
        }

        //test 10
        {
            itf->setTransform("frame2", "frame10", m1);
            yarp::os::Time::delay(0.050);
            bool b_can;
            b_can = itf->canTransform("frame2", "frame10");
            CHECK(b_can); // itf->setTransform ok
            yarp::os::Time::delay(0.6);
            b_can = itf->canTransform("frame2", "frame10");
            CHECK_FALSE(b_can); // itf->setTransform successfully expired after 0.6s
        }

        //test 11
        {
            itf->clear();
            bool set_b1 = itf->setTransform("frame2", "frame10", m1);
            yarp::os::Time::delay(0.050);
            yarp::sig::Matrix mt1;
            itf->getTransform("frame2", "frame10", mt1);
            bool set_b2 = itf->setTransform("frame2", "frame10", m2);
            yarp::os::Time::delay(0.050);
            yarp::sig::Matrix mt2;
            itf->getTransform("frame2", "frame10", mt2);
            bool a, b;
            a = isEqual(m1, mt1, precision);
            b = isEqual(m2, mt2, precision);
            CHECK(set_b1);
            CHECK(set_b2);
            CHECK(a);
            CHECK(b); // itf->setTransform successfully updated
        }

        //test 11b
        {
            itf->clear();
            bool set_b1 = itf->setTransformStatic("frame2", "frame10", m1);
            yarp::os::Time::delay(0.050);
            yarp::sig::Matrix mt1;
            itf->getTransform("frame2", "frame10", mt1);
            bool set_b2 = itf->setTransformStatic("frame2", "frame10", m2);
            yarp::os::Time::delay(0.050);
            yarp::sig::Matrix mt2;
            itf->getTransform("frame2", "frame10", mt2);
            CHECK(set_b1);
            CHECK_FALSE(set_b2);
            CHECK(isEqual(m1, mt1, precision));
            CHECK_FALSE(isEqual(m2, mt2, precision)); // itf->setTransformStatic successfully not-updated
        }

        //test 12
        {
            itf->clear();
            CHECK(itf->setTransform("frame2", "frame1", m1));
            yarp::os::Time::delay(0.050);
            CHECK(itf->setTransform("frame3", "frame2", m2));
            yarp::os::Time::delay(0.050);
            CHECK_FALSE(itf->setTransform("frame3", "frame1", m1));
            // itf->setTransform duplicate transform successfully skipped

            yarp::sig::Matrix mt1;
            yarp::sig::Matrix mt2;
            yarp::sig::Matrix mt3;
            itf->getTransform("frame2", "frame1", mt1);
            itf->getTransform("frame3", "frame2", mt2);
            itf->getTransform("frame3", "frame1", mt3);
            CHECK(isEqual(mt1, m1, precision));
            CHECK(isEqual(mt2, m2, precision));
            CHECK(isEqual(mt3, (m1*m2), precision));
            // itf->setTransform still working after duplicate transform
        }

        //test 12b
        {
            itf->clear();
            CHECK(itf->setTransformStatic("frame2", "frame1", m1));
            yarp::os::Time::delay(0.050);
            CHECK(itf->setTransformStatic("frame3", "frame2", m2));
            yarp::os::Time::delay(0.050);
            CHECK_FALSE(itf->setTransformStatic("frame3", "frame1", m1));
            // itf->setTransformStatic duplicate transform successfully skipped

            yarp::sig::Matrix mt1;
            yarp::sig::Matrix mt2;
            yarp::sig::Matrix mt3;
            itf->getTransform("frame2", "frame1", mt1);
            itf->getTransform("frame3", "frame2", mt2);
            itf->getTransform("frame3", "frame1", mt3);
            CHECK(isEqual(mt1, m1, precision));
            CHECK(isEqual(mt2, m2, precision));
            CHECK(isEqual(mt3, (m1*m2), precision));
            // itf->setTransformStatic still working after duplicate transform
        }

        //test 13
        {
            itf->clear();
            bool bcan = false;
            bcan = itf->canTransform("not_existing_frame", "not_existing_frame");
            CHECK(bcan);

            CHECK(itf->setTransformStatic("frame2", "frame1", m1));
            bcan = itf->canTransform("frame2", "frame2");
            CHECK(bcan);
            bcan = itf->canTransform("frame1", "frame1");
            CHECK(bcan);

            yarp::sig::Matrix mt1;
            yarp::sig::Matrix eyemat(4, 4); eyemat.eye();
            itf->getTransform("frame1", "frame1", mt1);
            CHECK(isEqual(mt1, eyemat, precision));
            itf->getTransform("frame2", "frame2", mt1);
            CHECK(isEqual(mt1, eyemat, precision));
        }

        // Close devices
        CHECK(ddtransformclient.close()); // ddtransformclient successfully closed
        CHECK(ddtransformserver.close()); // ddtransformserver successfully closed
    }

    Network::setLocalMode(false);

}

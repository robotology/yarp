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
#include <yarp/math/FrameTransform.h>

#include <cmath>
#include <vector>
#include <chrono>
#include <thread>

#include <catch.hpp>
#include <harness.h>

#include "IFrameTransformTest.h"

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



void exec_frameTransform_test_1(IFrameTransform* itf)
{
    bool precision_verbose= false;

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
    if (precision_verbose || b_gt==false)
    {
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
        CHECK(del_bool); // frame exists
        del_bool &= itf->deleteTransform("frame_test", "frame1");
        CHECK(del_bool); // deleteTransform ok
        del_bool &= (!itf->frameExists("frame_test"));
        CHECK(del_bool); // check if frame has been successfully removed

        //let's wait some time and check gain to be sure that
        //frame does not reappears....
        yarp::os::Time::delay(1);

        del_bool &= (!itf->frameExists("frame_test"));
        std::string strs;
        del_bool &= itf->allFramesAsString(strs);
        INFO("Existing frames:" << strs);
        CHECK(del_bool); // confirmed: frame does not exist anymore
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
}


void exec_frameTransform_test_2(IFrameTransform* itf)
{
    // Fake transform
    yarp::sig::Matrix armToHand(4, 4);
    yarp::sig::Matrix result(4, 4);
    yarp::sig::Matrix handToFinger(4, 4);

    armToHand.zero();
    armToHand(0, 0) = 1;  armToHand(0, 1) = 0;            armToHand(0, 2) = 0;          armToHand(0, 3) = 5;
    armToHand(1, 0) = 0;  armToHand(1, 1) = sqrt(2) / 2;    armToHand(1, 2) = sqrt(2) / 2;  armToHand(1, 3) = 4;
    armToHand(2, 0) = 0;  armToHand(2, 1) = -sqrt(2) / 2;  armToHand(2, 2) = sqrt(2) / 2;  armToHand(2, 3) = 3;
    armToHand(3, 0) = 0;  armToHand(3, 1) = 0;            armToHand(3, 2) = 0;          armToHand(3, 3) = 1;

    handToFinger(0, 0) = 1;  handToFinger(0, 1) = 0;            handToFinger(0, 2) = 0;          handToFinger(0, 3) = 5;
    handToFinger(1, 0) = 0;  handToFinger(1, 1) = sqrt(2) / 2;    handToFinger(1, 2) = sqrt(2) / 2;  handToFinger(1, 3) = 4;
    handToFinger(2, 0) = 0;  handToFinger(2, 1) = -sqrt(2) / 2;  handToFinger(2, 2) = sqrt(2) / 2;  handToFinger(2, 3) = 3;
    handToFinger(3, 0) = 0;  handToFinger(3, 1) = 0;            handToFinger(3, 2) = 0;          handToFinger(3, 3) = 1;

    result = armToHand * handToFinger;

    // test setTransformStatic
    // static transforms cannot be set if they already exists
    CHECK(itf->setTransformStatic("/hand", "/arm", armToHand));
    CHECK(!itf->setTransformStatic("/hand", "/arm", handToFinger));

    // static transforms cannot be set if an indirect connection already exists
    //CHECK(itf->setTransformStatic("/wrong_transform", "/hand", handToFinger));
   //CHECK(!itf->setTransformStatic("/wrong_transform", "/arm", handToFinger));
   // CHECK(itf->deleteTransform("/wrong_transform", "/hand"));

    // test setTransform
    CHECK(itf->setTransform("/finger", "/hand", handToFinger));
    CHECK(!itf->setTransform("/finger", "/arm", result));

    // test getTransform
    yarp::sig::Matrix resultFromTransform(4, 4);
    CHECK(itf->getTransform("/finger", "/arm", resultFromTransform));
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            CHECK((resultFromTransform(i, j) - result(i, j)) <= pow(10, -14));
        }
    }

    // test canTransform
    CHECK(itf->canTransform("/finger", "/arm"));
    CHECK(!itf->canTransform("/finger", "/head"));

    // test frameExists
    CHECK(itf->frameExists("/finger"));
    CHECK(!itf->frameExists("/head"));

    // test getAllFrameIds
    std::vector<std::string> frameIdsReturned;
    std::vector<std::string> correctFrameIdsReturned{
        "/arm",
        "/hand",
        "/finger"
    };
    CHECK(itf->getAllFrameIds(frameIdsReturned));
    CHECK(frameIdsReturned.size() == correctFrameIdsReturned.size());
    CHECK(frameIdsReturned == correctFrameIdsReturned);
    std::vector<std::string> wrongFrameIdsReturned1{
        "/hand",
        "/arm",
        "/finger"
    };
    CHECK(!(frameIdsReturned == wrongFrameIdsReturned1));

    // test getParent
    std::string parent;
    CHECK(itf->getParent("/finger", parent));
    CHECK(parent == "/hand");

    // test transformPoint
    yarp::sig::Vector startingPoint{ 0.0,0.0,0.0 };
    yarp::sig::Vector wrongStartingPoint{ 0.0,0.0,0.0,0.0 };
    yarp::sig::Vector endPoint{ 0.0,0.0,0.0,0.0 };
    CHECK(itf->transformPoint("/finger", "/arm", startingPoint, endPoint));
    CHECK(!itf->transformPoint("/finger", "/arm", wrongStartingPoint, endPoint));
    CHECK(!itf->transformPoint("/finger", "/head", startingPoint, endPoint));
    startingPoint.push_back(1);
    yarp::sig::Vector correctEndPoint = result * startingPoint;
    correctEndPoint.pop_back();
    CHECK(endPoint.size() == correctEndPoint.size());
    for (int i = 0; i < 3; i++) {
        CHECK((endPoint[i] - correctEndPoint[i]) <= pow(10, -14));
    }

    // test transformPose
    yarp::sig::Vector startingPose{ 0,0,0,0,0,0 };
    yarp::sig::Vector wrongStartingPose{ 0,0,0,0,0,0,0 };
    yarp::sig::Vector endPose;
    yarp::math::FrameTransform transform;
    transform.transFromVec(startingPose[0], startingPose[1], startingPose[2]);
    transform.rotFromRPY(startingPose[3], startingPose[4], startingPose[5]);
    transform.fromMatrix(result * transform.toMatrix());
    CHECK(itf->transformPose("/finger", "/arm", startingPose, endPose));
    CHECK((transform.translation.tX - endPose[0]) <= pow(10, -14));
    CHECK((transform.translation.tY - endPose[1]) <= pow(10, -14));
    CHECK((transform.translation.tZ - endPose[2]) <= pow(10, -14));
    CHECK((transform.translation.tZ - endPose[2]) <= pow(10, -14));
    CHECK((transform.getRPYRot()[0] - endPose[3]) <= pow(10, -14));
    CHECK((transform.getRPYRot()[1] - endPose[4]) <= pow(10, -14));
    CHECK((transform.getRPYRot()[2] - endPose[5]) <= pow(10, -14));
    CHECK(!itf->transformPose("/finger", "/arm", wrongStartingPose, endPose));
    CHECK(!itf->transformPose("/finger", "/head", startingPose, endPose));

    // test transformQuaternion
    yarp::math::Quaternion startingPoseQuaternion = transform.rotation;
    yarp::math::Quaternion endPoseQuaternion;
    CHECK(itf->transformQuaternion("/finger", "/arm", startingPoseQuaternion, endPoseQuaternion));
    yarp::math::Quaternion correctedEndPoseQuaternion;
    yarp::math::FrameTransform transformQuaternion;
    itf->getTransform("/finger", "/arm", result);
    transformQuaternion.rotation = startingPoseQuaternion;
    correctedEndPoseQuaternion.fromRotationMatrix(result * transformQuaternion.toMatrix());
    CHECK((endPoseQuaternion.x() - correctedEndPoseQuaternion.x()) <= pow(10, -14));
    CHECK((endPoseQuaternion.y() - correctedEndPoseQuaternion.y()) <= pow(10, -14));
    CHECK((endPoseQuaternion.z() - correctedEndPoseQuaternion.z()) <= pow(10, -14));
    CHECK((endPoseQuaternion.w() - correctedEndPoseQuaternion.w()) <= pow(10, -14));
    CHECK(!itf->transformQuaternion("/finger", "/head", startingPoseQuaternion, endPoseQuaternion));

    // test clear
    CHECK(itf->clear());
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    CHECK(!itf->getTransform("/hand", "/arm", result));
    CHECK(!itf->getTransform("/finger", "/hand", result));
    CHECK(!itf->getTransform("/finger", "/arm", result));
}

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IFrameTransform.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Property.h>
#include <yarp/os/Network.h>
#include <yarp/sig/Matrix.h>
#include <yarp/math/Math.h>
#include <yarp/math/FrameTransform.h>
#include <thread>
#include <iostream>
#include <math.h>


#include <catch.hpp>
#include <harness.h>

TEST_CASE("dev::newFrameTransformClientTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeFrameGrabber", "device");
    YARP_REQUIRE_PLUGIN("frameGrabberCropper", "device");

    yarp::os::Network::setLocalMode(true);

    SECTION("test the frameTransformClient local only mode")
    {
        yarp::dev::IFrameTransform* ft;
        yarp::dev::PolyDriver pd;
        yarp::os::Property p;
        p.put("device","frameTransformClient");
        p.put("filexml_option","ftc_local_only.xml");
        REQUIRE(pd.open(p));
        REQUIRE(pd.view(ft));

        // Fake transform
        yarp::sig::Matrix armToHand(4,4);
        yarp::sig::Matrix result(4,4);
        yarp::sig::Matrix handToFinger(4,4);

        armToHand.zero();
        armToHand(0,0) = 1;  armToHand(0,1) = 0;            armToHand(0,2) = 0;          armToHand(0,3) = 5;
        armToHand(1,0) = 0;  armToHand(1,1) = sqrt(2)/2;    armToHand(1,2) = sqrt(2)/2;  armToHand(1,3) = 4;
        armToHand(2,0) = 0;  armToHand(2,1) = - sqrt(2)/2;  armToHand(2,2) = sqrt(2)/2;  armToHand(2,3) = 3;
        armToHand(3,0) = 0;  armToHand(3,1) = 0;            armToHand(3,2) = 0;          armToHand(3,3) = 1;

        handToFinger(0,0) = 1;  handToFinger(0,1) = 0;            handToFinger(0,2) = 0;          handToFinger(0,3) = 5;
        handToFinger(1,0) = 0;  handToFinger(1,1) = sqrt(2)/2;    handToFinger(1,2) = sqrt(2)/2;  handToFinger(1,3) = 4;
        handToFinger(2,0) = 0;  handToFinger(2,1) = - sqrt(2)/2;  handToFinger(2,2) = sqrt(2)/2;  handToFinger(2,3) = 3;
        handToFinger(3,0) = 0;  handToFinger(3,1) = 0;            handToFinger(3,2) = 0;          handToFinger(3,3) = 1;

        result = armToHand * handToFinger;

        // test setTransformStatic
        CHECK(ft->setTransformStatic("/hand","/arm",armToHand));
        CHECK(!ft->setTransformStatic("/hand","/arm",handToFinger));

        // test setTransform
        CHECK(ft->setTransform("/finger","/hand",handToFinger));
        CHECK(!ft->setTransform("/finger","/arm",result));
        std::this_thread::sleep_for(std::chrono::milliseconds(250));

        // test getTransform
        yarp::sig::Matrix resultFromTransform(4,4);
        CHECK(ft->getTransform("/finger","/arm",resultFromTransform));
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        for(int i = 0; i < 4; i++){
            for(int j = 0; j < 4; j++) {
                CHECK((resultFromTransform(i,j) - result(i,j)) <= pow(10,-14));
            }
        }

        // test canTransform
        CHECK(ft->canTransform("/finger","/arm"));
        CHECK(!ft->canTransform("/finger","/head"));

        // test frameExists
        CHECK(ft->frameExists("/finger"));
        CHECK(!ft->frameExists("/head"));

        // test getAllFrameIds
        std::vector<std::string> frameIdsReturned;
        std::vector<std::string> correctFrameIdsReturned{
            "/arm",
            "/hand",
            "/finger"
        };
        CHECK(ft->getAllFrameIds(frameIdsReturned));
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
        CHECK(ft->getParent("/finger", parent));
        CHECK(parent == "/hand");

        // test transformPoint
        yarp::sig::Vector startingPoint{0.0,0.0,0.0};
        yarp::sig::Vector wrongStartingPoint{0.0,0.0,0.0,0.0};
        yarp::sig::Vector endPoint{0.0,0.0,0.0,0.0};
        CHECK(ft->transformPoint("/finger", "/arm", startingPoint, endPoint));
        CHECK(!ft->transformPoint("/finger", "/arm", wrongStartingPoint, endPoint));
        CHECK(!ft->transformPoint("/finger", "/head", startingPoint, endPoint));
        startingPoint.push_back(1);
        yarp::sig::Vector correctEndPoint = result * startingPoint;
        correctEndPoint.pop_back();
        CHECK(endPoint.size() == correctEndPoint.size());
        for (int i = 0; i < 3; i++){
            CHECK((endPoint[i] - correctEndPoint[i]) <= pow(10,-14));
        }

        // test transformPose
        yarp::sig::Vector startingPose{0,0,0,0,0,0};
        yarp::sig::Vector wrongStartingPose{0,0,0,0,0,0,0};
        yarp::sig::Vector endPose;
        yarp::math::FrameTransform transform;
        transform.transFromVec(startingPose[0],startingPose[1],startingPose[2]);
        transform.rotFromRPY(startingPose[3],startingPose[4],startingPose[5]);
        transform.fromMatrix(result* transform.toMatrix());
        CHECK(ft->transformPose("/finger", "/arm", startingPose, endPose));
        CHECK((transform.translation.tX -endPose[0]) <= pow(10,-14));
        CHECK((transform.translation.tY -endPose[1]) <= pow(10,-14));
        CHECK((transform.translation.tZ -endPose[2]) <= pow(10,-14));
        CHECK((transform.translation.tZ -endPose[2]) <= pow(10,-14));
        CHECK((transform.getRPYRot()[0] -endPose[3]) <= pow(10,-14));
        CHECK((transform.getRPYRot()[1] -endPose[4]) <= pow(10,-14));
        CHECK((transform.getRPYRot()[2] -endPose[5]) <= pow(10,-14));
        CHECK(!ft->transformPose("/finger", "/arm", wrongStartingPose, endPose));
        CHECK(!ft->transformPose("/finger", "/head", startingPose, endPose));

        // test transformQuaternion
        yarp::math::Quaternion startingPoseQuaternion = transform.rotation;
        yarp::math::Quaternion endPoseQuaternion;
        CHECK(ft->transformQuaternion("/finger", "/arm", startingPoseQuaternion, endPoseQuaternion));
        yarp::math::Quaternion correctedEndPoseQuaternion;
        yarp::math::FrameTransform transformQuaternion;
        ft->getTransform("/finger", "/arm", result);
        transformQuaternion.rotation = startingPoseQuaternion;
        correctedEndPoseQuaternion.fromRotationMatrix(result * transformQuaternion.toMatrix());
        CHECK((endPoseQuaternion.x() - correctedEndPoseQuaternion.x()) <= pow(10,-14));
        CHECK((endPoseQuaternion.y() - correctedEndPoseQuaternion.y()) <= pow(10,-14));
        CHECK((endPoseQuaternion.z() - correctedEndPoseQuaternion.z()) <= pow(10,-14));
        CHECK((endPoseQuaternion.w() - correctedEndPoseQuaternion.w()) <= pow(10,-14));
        CHECK(!ft->transformQuaternion("/finger", "/head", startingPoseQuaternion, endPoseQuaternion));

        // test clear
        CHECK(ft->clear());
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        CHECK(!ft->getTransform("/hand","/arm", result));
        CHECK(!ft->getTransform("/finger","/hand", result));
        CHECK(!ft->getTransform("/finger","/arm", result));

    }

    yarp::os::Network::setLocalMode(false);
}

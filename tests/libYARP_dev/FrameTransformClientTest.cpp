/**
 * Copyright (C) 2016 RobotCub Consortium
 * Authors: Silvio Traversaro
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <vector>

#include <yarp/os/impl/UnitTest.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IFrameTransform.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/PolyDriverList.h>
#include <yarp/dev/Wrapper.h>
#include <yarp/os/Time.h>
#include <yarp/math/Math.h>
#include <yarp/math/FrameTransform.h>
#include <yarp/os/LogStream.h>
#define M_PI 3.14159265358979323846
#include<math.h>

using namespace yarp::os::impl;
using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::math;


class FrameTransformClientTest : public UnitTest
{
public:

    bool isEqual(const yarp::sig::Vector& v1, const yarp::sig::Vector& v2, double precision)
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

    bool isEqual(const yarp::sig::Matrix& m1, const yarp::sig::Matrix& m2, double precision)
    {
        if (m1.cols() != m2.cols() || m1.rows() != m2.rows())
        {
            return false;
        }

        for (int i = 0; i < m1.rows(); i++)
        {
            if (!isEqual(m1.getRow(i), m2.getRow(i), precision))
            {
                return false;
            }
        }
        return true;
    }

    virtual ConstString getName()
    {
        return "FrameTransformClientTest";
    }

    void testFrameTransformClient()
    {
        report(0,"\ntest the transform client");
        bool precision_verbose = false;

        PolyDriver ddtransformserver;
        Property pTransformserver_cfg;
        pTransformserver_cfg.put("device", "transformServer");
        Property& ros_prop = pTransformserver_cfg.addGroup("ROS");
        ros_prop.put("enable_ros_publisher", "0");
        ros_prop.put("enable_ros_subscriber", "0");
        pTransformserver_cfg.put("transforms_lifetime", 0.500);
        bool ok_server = ddtransformserver.open(pTransformserver_cfg);
        checkTrue(ok_server, "ddtransformserver open reported successful");

        IFrameTransform* itf = 0;
        PolyDriver ddtransformclient;
        Property pTransformclient_cfg;
        pTransformclient_cfg.put("device", "transformClient");
        pTransformclient_cfg.put("local", "/transformClientTest");
        pTransformclient_cfg.put("remote", "/transformServer");
        bool ok_client = ddtransformclient.open(pTransformclient_cfg);
        checkTrue(ok_client, "ddtransformclient open reported successful");

        bool ok_view = ddtransformclient.view(itf);
        checkTrue(ok_view, "iTransform interface open reported successful");

        if (itf)
        {
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

            yarp::sig::Matrix m3(4, 4);
            m3 = m1*m2;
            double precision;
            precision = 0.00000001;
            itf->setTransformStatic("frame2", "frame1", m1);
            itf->setTransformStatic("frame3", "frame2", m2);
            itf->setTransformStatic("frame4", "frame3", m3);
            itf->setTransformStatic("frame11", "frame10", m1);
            itf->setTransformStatic("frame3b", "frame2", m2);

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
            report(0, std::string("Found frames: ") + std::string(buff));
            bool b_ids = (ids.size() == 7);
            checkTrue(b_ids, "getAllFrameIds ok");

            //test 1
            std::string parent;
            itf->getParent("frame3", parent);
            checkTrue(parent == "frame2", "getParent ok");

            //test 2
            yarp::sig::Matrix mt(4, 4);
            bool b_gt = itf->getTransform("frame3", "frame1", mt);
            isEqual(mt, m3, precision);
            checkTrue(b_gt, "getTransform ok");
            if (precision_verbose || b_gt==false) { yInfo() << "precision error:\n" + (mt - m3).toString(); }

            //test3
            bool b_exist1, b_exist2;
            b_exist1 = itf->frameExists("frame3");
            b_exist2 = itf->frameExists("frame3_err");
            checkTrue(b_exist1 && !b_exist2, "frameExists ok");

            //test4
            bool b_can1, b_can2;
            b_can1 = itf->canTransform("frame2", "frame1");
            b_can2 = itf->canTransform("frame11", "frame1");
            checkTrue(b_can1 && !b_can2, "canTransform ok");

            //test4bis
            bool b_canb1;
            b_canb1 = itf->canTransform("frame3b", "frame1");
            checkTrue(b_canb1, "canTransform Bis ok");

            //test 5
            yarp::sig::Matrix mti(4, 4);
            itf->getTransform("frame1", "frame3b", mti);
            bool b_gt_inv = isEqual(mti, yarp::math::SE3inv(m3), precision);
            checkTrue(b_gt_inv, "inverted getTransform ok");
            if (precision_verbose || b_gt_inv==false) { yInfo() << "precision error:\n" + (mti - yarp::math::SE3inv(m3)).toString(); }

            //test 6
            yarp::sig::Vector in_point1(3), out_point1(3), verPoint1(4);
            yarp::sig::Vector in_pose1(6),  out_pose1(6),  verPose(6);
            yarp::sig::Vector in_quat1(4),  out_quat1(4),  verQuat(4);

            //andrea.ruzzenenti@iit.it 09/09/16:
            //added matrix inversion to simulate the effect of the missing method rot2quat()
            in_quat1 = yarp::math::dcm2quat(SE3inv(m4));

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

            //andrea.ruzzenenti@iit.it 09/09/16:
            //the first one was correct. now the matrix is inverted at the initialization. check the comment there

            //verQuat = yarp::math::dcm2quat(m1 * m2 * SE3inv(m4)); //@@@@ TO BE CHECKED
            verQuat = yarp::math::dcm2quat(m1 * m2 * m4);           //@@@@ TO BE CHECKED

            itf->transformPoint("frame3", "frame1", in_point1, out_point1);
            itf->transformPose("frame3", "frame1", in_pose1, out_pose1);
            itf->transformQuaternion("frame3", "frame1", in_quat1, out_quat1);

            bool b_tpoint = isEqual(verPoint1, out_point1, precision);
            checkTrue(b_tpoint, "transformPoint ok");
            if (precision_verbose || b_tpoint == false) { yInfo() << "precision error:\n" + (verPoint1 - out_point1).toString(); }

            bool b_tpose = isEqual(verPose, out_pose1, precision);
            checkTrue(b_tpose, "transformPose ok");
            if (precision_verbose || b_tpose == false) { yInfo() << "precision error:\n" + (verPose - out_pose1).toString(); }

            bool b_tquat = isEqual(verQuat, out_quat1, precision);
            checkTrue(b_tquat, "transformQuaternion ok");
            if (precision_verbose || b_tquat == false) { yInfo() << "precision error:\n" + (verQuat - out_quat1).toString(); }

            //test 7
            std::string all_frames;
            bool b_all_f = itf->allFramesAsString(all_frames);
            b_all_f &= (std::string::npos != all_frames.find("frame1"));
            b_all_f &= (std::string::npos != all_frames.find("frame2"));
            b_all_f &= (std::string::npos != all_frames.find("frame3"));
            b_all_f &= (std::string::npos != all_frames.find("frame4"));
            b_all_f &= (std::string::npos != all_frames.find("frame10"));
            b_all_f &= (std::string::npos != all_frames.find("frame11"));
            b_all_f &= (std::string::npos != all_frames.find("frame3b"));
            checkTrue(b_all_f, "allFramesAsString ok");

            //test 8
            itf->setTransformStatic("frame_test", "frame1", m1);
            yarp::os::Time::delay(1);
            bool del_bool = itf->frameExists("frame_test");
            itf->deleteTransform("frame_test", "frame1");
            yarp::os::Time::delay(1);
            del_bool &= (!itf->frameExists("frame_test"));
            checkTrue(del_bool, "deleteTransform ok");

            //test 9
            itf->clear();
            std::vector<std::string> cids;
            itf->getAllFrameIds(cids);
            checkTrue(cids.size() == 0, "clear ok");

            //test 10
            itf->setTransform("frame2", "frame1", m1);
            yarp::os::Time::delay(0.050);
            bool b_can;
            b_can = itf->canTransform("frame2", "frame1");
            checkTrue(b_can, "itf->setTransform ok");
            yarp::os::Time::delay(0.6);
            b_can = itf->canTransform("frame2", "frame1");
            checkFalse(b_can, "itf->setTransform successfully expired after 0.6s");
        }

        // Close devices
        bool cl1 = ddtransformclient.close();
        bool cl2 = ddtransformserver.close();
        checkTrue(cl1, "ddtransformclient successfully closed");
        checkTrue(cl2, "ddtransformserver successfully closed");
    }

    virtual void runTests()
    {
        Network::setLocalMode(true);
        testFrameTransformClient();
        Network::setLocalMode(false);
    }
};

static FrameTransformClientTest theFrameTransformClientTest;

UnitTest& getFrameTransformClientTest()
{
    return theFrameTransformClientTest;
}

/**
 * Copyright (C) 2016 RobotCub Consortium
 * Authors: Silvio Traversaro
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <vector>

#include <yarp/os/impl/UnitTest.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ITransform.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/PolyDriverList.h>
#include <yarp/dev/Wrapper.h>
#include <yarp/os/Time.h>
#include <yarp/math/Math.h>

using namespace yarp::os::impl;
using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::math;


class TransformClientTest : public UnitTest
{
public:
    virtual String getName()
    {
        return "TransformClientTest";
    }

    void testTransformClient()
    {
        report(0,"\ntest the transform client");

        PolyDriver ddtransformserver;
        Property pTransformserver_cfg;
        pTransformserver_cfg.put("device", "transformServer");
        pTransformserver_cfg.put("ROS", "");
        bool ok_server = ddtransformserver.open(pTransformserver_cfg);
        checkTrue(ok_server, "ddtransformserver open reported successful");

        ITransform* itf = 0;
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
            m1[0][0] = 1; m1[0][1] = 0; m1[0][2] = 0; m1[0][3] = 1;
            m1[1][0] = 0; m1[1][1] = 1; m1[1][2] = 0; m1[1][3] = 2;
            m1[2][0] = 0; m1[2][1] = 0; m1[2][2] = 1; m1[2][3] = 3;
            m1[3][0] = 0; m1[3][1] = 0; m1[3][2] = 0; m1[3][3] = 1;
            yarp::sig::Matrix m2(4, 4);
            m2[0][0] = 1; m2[0][1] = 0; m2[0][2] = 0; m2[0][3] = 0.1;
            m2[1][0] = 0; m2[1][1] = 1; m2[1][2] = 0; m2[1][3] = 0.2;
            m2[2][0] = 0; m2[2][1] = 0; m2[2][2] = 1; m2[2][3] = 0.3;
            m2[3][0] = 0; m2[3][1] = 0; m2[3][2] = 0; m2[3][3] = 1;
            yarp::sig::Matrix m3(4, 4);
            m3 = m1*m2;
            itf->setTransform("frame1", "frame2", m1);
            itf->setTransform("frame2", "frame3", m2);
            itf->setTransform("frame3", "frame4", m3);
            itf->setTransform("frame10", "frame11", m1);

            //test 0
            std::vector<std::string> ids;
            itf->getAllFrameIds(ids);
            report(0, "Found frames:\n"); char buff[1024];
            for (size_t i = 0; i < ids.size(); i++)
            {
                sprintf(buff, "%d, %s", i, ids[i].c_str());
                report(0, buff);
            }
            bool b_ids = (ids.size() == 6);
            checkTrue(b_ids, "getAllFrameIds ok");

            //test 1
            std::string parent;
            itf->getParent("frame3", parent);
            checkTrue(parent == "frame2", "getParent ok");

            //test 2
            yarp::sig::Matrix mt(4, 4);
            itf->getTransform("frame3", "frame1", mt);
            checkTrue(mt == m3, "getTransform ok");

            //test3 
            bool b_exist1, b_exist2;
            b_exist1 = itf->frameExists("frame3");
            b_exist2 = itf->frameExists("frame3_err");
            checkTrue(b_exist1 && !b_exist2, "frameExists ok");

            //test4
            bool b_can1, b_can2;
            b_can1 = itf->canTransform("frame1", "frame2");
            b_can2 = itf->canTransform("frame1", "frame11");
            checkTrue(b_can1 && !b_can2, "canTransform ok");

            //test 5
            yarp::sig::Matrix mti(4, 4);
            itf->getTransform("frame1", "frame3", mti);
            checkTrue(mt == m3, "inverted getTransform ok");

            //test 6
            yarp::sig::Vector point1(3), tpoint1(3);
            yarp::sig::Vector pose1(6), tpose1(6);
            yarp::sig::Vector quat1(4), tquat1(4);
            itf->transformPoint("frame1", point1, tpoint1);
            itf->transformPose("frame1", pose1, tpose1);
            itf->transformQuaternion("frame1", quat1, tquat1);
            checkTrue(point1 == tpoint1, "transformPoint ok");
            checkTrue(pose1 == tpose1, "transformPose ok");
            checkTrue(quat1 == tquat1, "transformQuaternion ok");

            //test 7
            std::string all_frames;
            bool b_all_f = itf->allFramesAsString(all_frames);
            b_all_f &= (std::string::npos != all_frames.find("frame1"));
            b_all_f &= (std::string::npos != all_frames.find("frame2"));
            b_all_f &= (std::string::npos != all_frames.find("frame3"));
            b_all_f &= (std::string::npos != all_frames.find("frame4"));
            b_all_f &= (std::string::npos != all_frames.find("frame10"));
            b_all_f &= (std::string::npos != all_frames.find("frame11"));
            checkTrue(b_all_f, "allFramesAsString ok");

            //test 8
            itf->setTransform("frame_test", "frame1", m1);
            bool del_bool = itf->frameExists("frame_test");
            itf->deleteTransform("frame_test", "frame1");
            del_bool &= (!itf->frameExists("frame_test"));
            checkTrue(del_bool, "deleteTransform ok");

            //test 9
            itf->clear();
            std::vector<std::string> cids;
            itf->getAllFrameIds(cids);
            checkTrue(cids.size() == 0, "clear ok");
        }
        
        // Close devices
        bool cl1 = ddtransformclient.close();
        bool cl2 = ddtransformserver.close();
        checkTrue(cl1, "ddtransformclient succesfully closed");
        checkTrue(cl2, "ddtransformserver succesfully closed");
    }

    virtual void runTests()
    {
        Network::setLocalMode(true);
        testTransformClient();
        Network::setLocalMode(false);
    }
};

static TransformClientTest theTransformClientTest;

UnitTest& getTransformClientTest()
{
    return theTransformClientTest;
}

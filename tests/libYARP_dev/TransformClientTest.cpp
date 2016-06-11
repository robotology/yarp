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

using namespace yarp::os::impl;
using namespace yarp::os;
using namespace yarp::dev;


class TransformClientTest : public UnitTest
{
public:
    virtual String getName()
    {
        return "TransformClientTest";
    }

    void testTransformClient()
    {
        yarp::os::Network::init();
        report(0,"\ntest the transform client");

        PolyDriver ddtransformserver;
        Property pTransformserver_cfg;
        pTransformserver_cfg.put("device", "transformServer");
        pTransformserver_cfg.put("ros", "");
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

        ddtransformclient.view(itf);
        if (itf)
        {

        }


        checkEqual(0, 0, "remapper seems functional");


        // Close devices
        ddtransformclient.close();
        ddtransformserver.close();
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

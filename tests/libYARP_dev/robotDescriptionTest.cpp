/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <vector>

#include <yarp/os/impl/UnitTest.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IRobotDescription.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/PolyDriverList.h>
#include <yarp/dev/Wrapper.h>
#include <yarp/os/Time.h>
#include <yarp/os/LogStream.h>
#include <algorithm>

using namespace yarp::os::impl;
using namespace yarp::os;
using namespace yarp::dev;

class RobotDescriptionTest : public UnitTest
{
public:

    virtual std::string getName() const override
    {
        return "RobotDescriptionTest";
    }

    void testRobotDescription()
    {
        report(0,"\ntest the RobotDescription client/server");

        PolyDriver ddserver;
        Property pserver_cfg;
        pserver_cfg.put("device", "robotDescriptionServer");
        pserver_cfg.put("local", "/robotDescriptionServerPort");
        bool ok_server = ddserver.open(pserver_cfg);
        checkTrue(ok_server, "robotDescriptionServer open reported successful");

        IRobotDescription* idesc = nullptr;
        PolyDriver ddclient;
        Property pclient_cfg;
        pclient_cfg.put("device", "robotDescriptionClient");
        pclient_cfg.put("local",  "/robotDescriptionClientPort");
        pclient_cfg.put("remote", "/robotDescriptionServerPort");
        bool ok_client = ddclient.open(pclient_cfg);
        checkTrue(ok_client, "robotDescriptionClient open reported successful");

        bool ok_view = ddclient.view(idesc);
        checkTrue(ok_view, "IRobotDescription interface open reported successful");

        //
        DeviceDescription dev1; dev1.device_name = "/icubTest/left_arm"; dev1.device_type = "controlboardwrapper2";
        DeviceDescription dev2; dev2.device_name = "/icubTest/left_leg"; dev2.device_type = "controlboardwrapper2";
        DeviceDescription dev3; dev3.device_name = "/icubTest/test";     dev3.device_type = "testDevice";
        idesc->registerDevice(dev1);
        idesc->registerDevice(dev2);
        idesc->registerDevice(dev3);
        std::vector<DeviceDescription> list1;
        std::vector<DeviceDescription> list2;
        idesc->getAllDevices(list1);
        idesc->getAllDevicesByType("controlboardwrapper2", list2);

        size_t sl1 = list1.size();
        size_t sl2 = list2.size();
        bool b1 = (std::find(list1.begin(), list1.end(), dev1) != list1.end());
        bool b2 = (std::find(list1.begin(), list1.end(), dev2) != list1.end());
        bool b3 = (std::find(list1.begin(), list1.end(), dev3) != list1.end());
        bool b4 = (std::find(list2.begin(), list2.end(), dev1) != list2.end());
        bool b5 = (std::find(list2.begin(), list2.end(), dev2) != list2.end());
        bool b6 = (std::find(list2.begin(), list2.end(), dev3) != list2.end());
        checkTrue((sl1 == 3) && b1 && b2 &&  b3, "IRobotDescription::getAllDevices() successfully tested");
        checkTrue((sl2 == 2) && b4 && b5 && !b6, "IRobotDescription::getControlBoardWrapperDevices() successfully tested");

        // Test unregister device
        idesc->unregisterDevice("/icubTest/test");
        idesc->getAllDevicesByType("testDevice", list2);
        checkTrue(list2.size() == 0, "IRobotDescription::unregisterDevice() successfully tested");

        // Close devices
        bool cl1 = ddclient.close();
        bool cl2 = ddserver.close();
        checkTrue(cl1, "robotDescriptionClient successfully closed");
        checkTrue(cl2, "robotDescriptionServer successfully closed");
    }

    virtual void runTests() override
    {
        Network::setLocalMode(true);
        testRobotDescription();
        Network::setLocalMode(false);
    }
};

static RobotDescriptionTest theRobotDescriptionTest;

UnitTest& getRobotDescriptionTest()
{
    return theRobotDescriptionTest;
}

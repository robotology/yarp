/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/tests/IRemoteCalibratorTest.h>
#include <yarp/os/Network.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/robotinterface/Param.h>
#include <yarp/robotinterface/XMLReader.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::fakeCalibrator", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeCalibrator", "device");
    YARP_REQUIRE_PLUGIN("controlBoard_nwc_yarp", "device");

    //Opened by yarprotobotinterface, but we need it here to be sure it is available for the test
    YARP_REQUIRE_PLUGIN("fakeMotionControl", "device");
    YARP_REQUIRE_PLUGIN("controlBoard_nws_yarp", "device");
    YARP_REQUIRE_PLUGIN("controlBoard_remapper", "device");

    Network::setLocalMode(true);

    SECTION("Checking fakeCalibrator device with XML configuration")
    {
        // Find file from disk
        yarp::os::ResourceFinder res;
        res.setDefaultContext("tests/libYARP_robotinterface");
        std::string filepath = res.findFileByName("RobotinterfaceWithFakeCalibrator.xml");
        // Load empty XML configuration file
        std::string XMLString = "";

        yarp::robotinterface::XMLReader reader;
        yarp::robotinterface::XMLReaderResult readerresult = reader.getRobotFromFile(filepath);
        REQUIRE(readerresult.parsingIsSuccessful);
        size_t device_num = readerresult.robot.devices().size();
        CHECK(device_num != 0);

        REQUIRE(readerresult.robot.enterPhase(yarp::robotinterface::ActionPhaseStartup));
        yarp::os::Time::delay(1.0);

        //----------------------------------------------------
        //open a client nwc
        {
            PolyDriver ddnwc;
            yarp::dev::IRemoteCalibrator* icalib = nullptr;
            {
                Property p_cfg;
                p_cfg.put("device", "controlBoard_nwc_yarp");
                p_cfg.put("local", "/local_controlboard");
                p_cfg.put("remote", "/robot_test/part_test");
                REQUIRE(ddnwc.open(p_cfg));
            }
            yarp::os::Time::delay(0.1);
            //test
            ddnwc.view(icalib);    REQUIRE(icalib);
            yarp::dev::tests::exec_iRemoteCalibrator_test_1(icalib);
            yarp::os::Time::delay(0.1);
            ddnwc.close();
            yarp::os::Time::delay(0.1);
        }

        //----------------------------------------------------

        REQUIRE(readerresult.robot.enterPhase(yarp::robotinterface::ActionPhaseShutdown));
        yarp::os::Time::delay(1.0);
    }

    Network::setLocalMode(false);
}

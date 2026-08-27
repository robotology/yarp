/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Time.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/PolyDriverList.h>
#include <yarp/dev/IMultipleWrapper.h>

#include <vector>

#include <yarp/dev/tests/TestUtils.h>
#include <yarp/dev/tests/ParametersTest.h>
#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::dev;

const char *fmcA_file_content   = "device fakeMotionControl\n"
                                  "[GENERAL]\n"
                                  "Joints 2\n"
                                  "\n"
                                  "AxisName (\"axisA1\" \"axisA2\") \n";

const char *fmcB_file_content   = "device fakeMotionControl\n"
                                  "[GENERAL]\n"
                                  "Joints 3\n"
                                  "\n"
                                  "AxisName (\"axisB1\" \"axisB2\" \"axisB3\") \n";

const char *fmcC_file_content   =  "device fakeMotionControl\n"
                                  "[GENERAL]\n"
                                  "Joints 4\n"
                                  "\n"
                                  "AxisName (\"axisC1\" \"axisC2\" \"axisC3\" \"axisC4\")  \n";

const char *wrapperA_file_content   = "device controlBoard_nws_yarp\n"
                                      "name /testRemapperRobot/a\n"
                                      "period 0.01\n";

const char *wrapperB_file_content   = "device controlBoard_nws_yarp\n"
                                      "name /testRemapperRobot/b\n"
                                      "period 0.01\n";

const char *wrapperC_file_content   = "device controlBoard_nws_yarp\n"
                                      "name /testRemapperRobot/c\n"
                                      "period 0.01\n";


TEST_CASE("dev::remotecontrolboardremapperTest2", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeMotionControl", "device");
    YARP_REQUIRE_PLUGIN("remotecontrolboardremapper", "device");

    Network::setLocalMode(true);

    SECTION("Test the invalid configurations of remotecontrolboardremapper")
    {
        // We first allocate three fakeMotionControl boards
        // and their wrappers that we will remap using the remapper
        std::vector<std::unique_ptr<PolyDriver>> fmcbs;
        std::vector<std::unique_ptr<PolyDriver>> wrappers;
        fmcbs.resize(3);
        wrappers.resize(3);

        std::vector<int> fmcbsSizes;
        fmcbsSizes.push_back(2);
        fmcbsSizes.push_back(3);
        fmcbsSizes.push_back(4);

        std::vector<std::string> fmcbsNames;
        fmcbsNames.push_back("fakeControlBoardA");
        fmcbsNames.push_back("fakeControlBoardB");
        fmcbsNames.push_back("fakeControlBoardC");

        std::vector<std::string> wrapperNetworks;
        wrapperNetworks.push_back("net_a");
        wrapperNetworks.push_back("net_b");
        wrapperNetworks.push_back("net_c");


        for(int i=0; i < 3; i++)
        {
            fmcbs[i] = std::make_unique<PolyDriver>();

            Property p;

            if(i==0) { p.fromConfig(fmcA_file_content); }
            if(i==1) { p.fromConfig(fmcB_file_content); }
            if(i==2) { p.fromConfig(fmcC_file_content); }

            REQUIRE(fmcbs[i]->open(p)); // fakeMotionControlBoard open reported successful

            IPositionControl *pos = nullptr;
            REQUIRE(fmcbs[i]->view(pos)); // interface position correctly opened
            REQUIRE(pos);

            size_t axes = 0;
            pos->getAxes(axes);
            CHECK(axes == fmcbsSizes[i]); // fakeMotionControlBoard seems functional

            // Open the wrapper
            wrappers[i] = std::make_unique<PolyDriver>();

            if(i==0) { p.fromConfig(wrapperA_file_content); }
            if(i==1) { p.fromConfig(wrapperB_file_content); }
            if(i==2) { p.fromConfig(wrapperC_file_content); }

            REQUIRE(wrappers[i]->open(p)); // controlBoard_nws_yarp open reported successful

            yarp::dev::IMultipleWrapper *iwrap = nullptr;
            REQUIRE(wrappers[i]->view(iwrap)); // interface for multiple wrapper correctly opened for the controlBoard_nws_yarp
            REQUIRE(iwrap);

            PolyDriverList pdList;
            pdList.push(fmcbs[i].get(), wrapperNetworks[i].c_str());

            REQUIRE(iwrap->attachAll(pdList)); // controlBoard_nws_yarp attached successfully to the device
        }

        // Create a list containing all the fake controlboards
        yarp::dev::PolyDriverList fmcList;

        for(int i=0; i < 3; i++)
        {
            fmcList.push(fmcbs[i].get(),fmcbsNames[i].c_str());
        }

        // Open the remotecontrolboardremapper
        PolyDriver ddRemoteRemapper;
        Property pRemoteRemapper;
        pRemoteRemapper.put("device","remotecontrolboardremapper");
        pRemoteRemapper.addGroup("axesNames");
        Bottle & remoteAxesList = pRemoteRemapper.findGroup("axesNames").addList();
        remoteAxesList.addString("axisA1");
        remoteAxesList.addString("axisB1");
        remoteAxesList.addString("axisC1_INVALID");
        remoteAxesList.addString("axisB3");
        remoteAxesList.addString("axisC3");
        remoteAxesList.addString("axisA2");
        size_t nrOfRemappedAxes = 6;

        Bottle remoteControlBoards;
        Bottle & remoteControlBoardsList = remoteControlBoards.addList();
        remoteControlBoardsList.addString("/testRemapperRobot/a");
        remoteControlBoardsList.addString("/testRemapperRobot/b");
        remoteControlBoardsList.addString("/testRemapperRobot/c");
        pRemoteRemapper.put("remoteControlBoards",remoteControlBoards.get(0));

        pRemoteRemapper.put("localPortPrefix","/test/remoteControlBoardRemapper");

        Property & opts = pRemoteRemapper.addGroup("REMOTE_CONTROLBOARD_OPTIONS");
        opts.put("writeStrict","on");

        bool openremapper_b = ddRemoteRemapper.open(pRemoteRemapper);
        //This must fail because of axisC1_INVALID which does not exists
        CHECK(!openremapper_b);

        // Close devices
        ddRemoteRemapper.close();

        for(int i=0; i < 3; i++)
        {
            wrappers[i]->close();
        }
        for(int i=0; i < 3; i++)
        {
            fmcbs[i]->close();
        }
    }

    Network::setLocalMode(false);
}

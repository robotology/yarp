/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Time.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IControlMode.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/PolyDriverList.h>
#include <yarp/dev/IMultipleWrapper.h>

#include <vector>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::dev;

const char *fmcA_file_content   = "device fakeMotionControl\n"
                                  "[GENERAL]\n"
                                  "Joints 2\n"
                                  "\n"
                                  "AxisName \"axisA1\" \"axisA2\" \n";

const char *fmcB_file_content   = "device fakeMotionControl\n"
                                  "[GENERAL]\n"
                                  "Joints 3\n"
                                  "\n"
                                  "AxisName \"axisB1\" \"axisB2\" \"axisB3\"\n";

const char *fmcC_file_content   =  "device fakeMotionControl\n"
                                  "[GENERAL]\n"
                                  "Joints 4\n"
                                  "\n"
                                  "AxisName \"axisC1\" \"axisC2\" \"axisC3\" \"axisC4\"  \n";

const char *wrapperA_file_content   = "device controlboardwrapper2\n"
                                      "name /testRemapperRobot/a\n"
                                      "period 10\n"
                                      "networks (net_a)\n"
                                      "joints 2\n"
                                      "net_a 0 1 0 1\n";

const char *wrapperB_file_content   = "device controlboardwrapper2\n"
                                      "name /testRemapperRobot/b\n"
                                      "period 10\n"
                                      "networks (net_b)\n"
                                      "joints 3\n"
                                      "net_b 0 2 0 2\n";

const char *wrapperC_file_content   = "device controlboardwrapper2\n"
                                      "name /testRemapperRobot/c\n"
                                      "period 10\n"
                                      "networks (net_c)\n"
                                      "joints 4\n"
                                      "net_c 0 3 0 3\n";


static void checkRemapper(yarp::dev::PolyDriver & ddRemapper, int rand, size_t nrOfRemappedAxes)
{
    IPositionControl *pos = nullptr;
    REQUIRE(ddRemapper.view(pos)); // interface position correctly opened
    int axes = 0;
    CHECK(pos->getAxes(&axes)); // getAxes returned correctly
    CHECK((size_t) axes == nrOfRemappedAxes); // remapper seems functional

    IPositionDirect *posdir = nullptr;
    REQUIRE(ddRemapper.view(posdir)); // direct position interface correctly opened

    IEncoders * encs = nullptr;
    REQUIRE(ddRemapper.view(encs)); // encoders interface correctly opened

    IControlMode *ctrlmode = nullptr;
    REQUIRE(ddRemapper.view(ctrlmode)); // control mode interface correctly opened

    // Vector used for setting/getting data from the controlboard
    std::vector<double> setPosition(nrOfRemappedAxes,-10),
                        setRefSpeeds(nrOfRemappedAxes,-15),
                        readedPosition(nrOfRemappedAxes,-20),
                        readedEncoders(nrOfRemappedAxes,-30);

    for(size_t i=0; i < nrOfRemappedAxes; i++)
    {
        setPosition[i]  = i*100.0+50.0 + rand;
        setRefSpeeds[i] = i*10.0+5 + rand;
        readedPosition[i] = -100 + rand;
    }

    // Set the control mode in position direct
    std::vector<int>    settedControlMode(nrOfRemappedAxes,VOCAB_CM_POSITION_DIRECT);
    std::vector<int>    readedControlMode(nrOfRemappedAxes,VOCAB_CM_POSITION);

    CHECK(ctrlmode->setControlModes(settedControlMode.data())); // setControlModes correctly called

    // Check that the readed control mode is actually position direct
    // Let's try 10 times because if the remapper is using some remotecontrolboards,
    // it is possible that this return false if it is called before the first message
    // has been received from the controlboardwrapper
    bool getControlModesOk = false;
    for(int wait=0; wait < 10 && !getControlModesOk; wait++)
    {
        getControlModesOk = ctrlmode->getControlModes(readedControlMode.data());
        yarp::os::Time::delay(0.001);
    }
    CHECK(getControlModesOk); // getControlModes correctly called

    for(size_t i=0; i < nrOfRemappedAxes; i++)
    {
        CHECK(settedControlMode[i] == readedControlMode[i]); // Setted control mode and readed control mode match
    }

    // Test position direct methods

    // Set position
    CHECK(posdir->setPositions(setPosition.data())); // setPositions correctly called

    // Set also the speeds in the mean time, so we are sure that we don't get
    // spurios successful set/get of position because of intermediate buffers
    CHECK(pos->setRefSpeeds(setRefSpeeds.data())); // setRefSpeeds correctly called

    // Wait some time to make sure that the vector has been correctly propagated
    // back and forth
    yarp::os::Time::delay(0.1);

    // Read position
    CHECK(posdir->getRefPositions(readedPosition.data())); // getRefPositions correctly called

    // Check that the two vector match
    for(size_t i=0; i < nrOfRemappedAxes; i++)
    {
        CHECK(setPosition[i] == readedPosition[i]); // Setted position and readed ref position match
    }

    // Do a similar test for the encoders
    // in fakeMotionControl their value is the one setted with setPosition
    CHECK(encs->getEncoders(readedEncoders.data())); // getEncoders correctly called

    // Check that the two vector match
    for(size_t i=0; i < nrOfRemappedAxes; i++)
    {
        CHECK(setPosition[i] == readedEncoders[i]); // Setted position and readed encoders match
    }
}


TEST_CASE("dev::ControlBoardRemapperTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("fakeMotionControl", "device");

#if defined(DISABLE_FAILING_TESTS)
    YARP_SKIP_TEST("Skipping failing tests")
#endif

    Network::setLocalMode(true);

    SECTION("Test the controlboard remapper")
    {
        // We first allocate three fakeMotionControl boards
        // and their wrappers that we will remap using the remapper
        std::vector<PolyDriver *> fmcbs;
        std::vector<PolyDriver *> wrappers;
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
            fmcbs[i] = new PolyDriver();

            Property p;

            if(i==0) { p.fromConfig(fmcA_file_content); }
            if(i==1) { p.fromConfig(fmcB_file_content); }
            if(i==2) { p.fromConfig(fmcC_file_content); }

            REQUIRE(fmcbs[i]->open(p)); // fakeMotionControlBoard open reported successful

            IPositionControl *pos = nullptr;
            REQUIRE(fmcbs[i]->view(pos)); // interface position correctly opened
            int axes = 0;
            pos->getAxes(&axes);
            CHECK(axes == fmcbsSizes[i]); // fakeMotionControlBoard seems functional

            // Open the wrapper
            wrappers[i] = new PolyDriver();

            if(i==0) { p.fromConfig(wrapperA_file_content); }
            if(i==1) { p.fromConfig(wrapperB_file_content); }
            if(i==2) { p.fromConfig(wrapperC_file_content); }

            REQUIRE(wrappers[i]->open(p)); // controlboardwrapper2 open reported successful

            yarp::dev::IMultipleWrapper *iwrap = nullptr;
            REQUIRE(wrappers[i]->view(iwrap)); // interface for multiple wrapper correctly opened for the controlboardwrapper2

            PolyDriverList pdList;
            pdList.push(fmcbs[i],wrapperNetworks[i].c_str());

            CHECK(iwrap->attachAll(pdList)); // controlboardwrapper2 attached successfully to the device
        }

        // Create a list containing all the fake controlboards
        yarp::dev::PolyDriverList fmcList;

        for(int i=0; i < 3; i++)
        {
            fmcList.push(fmcbs[i],fmcbsNames[i].c_str());
        }

        // Open a controlboardremapper with the wrong axisName,
        // and make sure that if fails during attachAll
        PolyDriver ddRemapperWN;
        Property pRemapperWN;
        pRemapperWN.put("device","controlboardremapper");
        pRemapperWN.addGroup("axesNames");
        Bottle & axesListWN = pRemapperWN.findGroup("axesNames").addList();
        axesListWN.addString("axisA1");
        axesListWN.addString("axisB1");
        axesListWN.addString("axisC1");
        axesListWN.addString("axisB3");
        axesListWN.addString("axisC3");
        axesListWN.addString("axisA2");
        axesListWN.addString("thisIsAnAxisNameThatDoNotExist");

        REQUIRE(ddRemapperWN.open(pRemapperWN)); // controlboardremapper with wrong names open reported successful

        yarp::dev::IMultipleWrapper *imultwrapWN = nullptr;
        REQUIRE(ddRemapperWN.view(imultwrapWN)); // interface for multiple wrapper with wrong names correctly opened


        CHECK_FALSE(imultwrapWN->attachAll(fmcList)); // attachAll for controlboardremapper with wrong names successful

        // Make sure that a controlboard in which attachAll is not successfull
        // closes correctly
        CHECK(ddRemapperWN.close()); // controlboardremapper with wrong names close was successful

        // Open the controlboardremapper
        PolyDriver ddRemapper;
        Property pRemapper;
        pRemapper.put("device","controlboardremapper");
        pRemapper.addGroup("axesNames");
        Bottle & axesList = pRemapper.findGroup("axesNames").addList();
        axesList.addString("axisA1");
        axesList.addString("axisB1");
        axesList.addString("axisC1");
        axesList.addString("axisB3");
        axesList.addString("axisC3");
        axesList.addString("axisA2");
        size_t nrOfRemappedAxes = 6;

        REQUIRE(ddRemapper.open(pRemapper)); // controlboardremapper open reported successful

        yarp::dev::IMultipleWrapper *imultwrap = nullptr;
        REQUIRE(ddRemapper.view(imultwrap)); // interface for multiple wrapper correctly opened

        CHECK(imultwrap->attachAll(fmcList)); // attachAll for controlboardremapper successful


        // Test the controlboardremapper
        checkRemapper(ddRemapper,200,nrOfRemappedAxes);

        // Open the remotecontrolboardremapper
        PolyDriver ddRemoteRemapper;
        Property pRemoteRemapper;
        pRemoteRemapper.put("device","remotecontrolboardremapper");
        pRemoteRemapper.addGroup("axesNames");
        Bottle & remoteAxesList = pRemoteRemapper.findGroup("axesNames").addList();
        remoteAxesList.addString("axisA1");
        remoteAxesList.addString("axisB1");
        remoteAxesList.addString("axisC1");
        remoteAxesList.addString("axisB3");
        remoteAxesList.addString("axisC3");
        remoteAxesList.addString("axisA2");

        Bottle remoteControlBoards;
        Bottle & remoteControlBoardsList = remoteControlBoards.addList();
        remoteControlBoardsList.addString("/testRemapperRobot/a");
        remoteControlBoardsList.addString("/testRemapperRobot/b");
        remoteControlBoardsList.addString("/testRemapperRobot/c");
        pRemoteRemapper.put("remoteControlBoards",remoteControlBoards.get(0));

        pRemoteRemapper.put("localPortPrefix","/test/remoteControlBoardRemapper");

        Property & opts = pRemoteRemapper.addGroup("REMOTE_CONTROLBOARD_OPTIONS");
        opts.put("writeStrict","on");

        REQUIRE(ddRemoteRemapper.open(pRemoteRemapper)); // remotecontrolboardremapper open reported successful, testing it

        // Test the remotecontrolboardremapper
        checkRemapper(ddRemoteRemapper,100,nrOfRemappedAxes);

        // Close devices
        imultwrap->detachAll();
        ddRemapper.close();
        ddRemoteRemapper.close();

        for(int i=0; i < 3; i++)
        {
            wrappers[i]->close();
            delete wrappers[i];
            wrappers[i] = nullptr;
            fmcbs[i]->close();
            delete fmcbs[i];
            fmcbs[i] = nullptr;
        }
    }

    Network::setLocalMode(false);
}

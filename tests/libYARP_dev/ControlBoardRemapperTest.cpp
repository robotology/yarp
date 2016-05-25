/**
 * Copyright (C) 2016 RobotCub Consortium
 * Authors: Silvio Traversaro
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <vector>

#include <yarp/os/impl/UnitTest.h>
#include <yarp/os/Time.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IControlMode2.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/PolyDriverList.h>
#include <yarp/dev/Wrapper.h>

using namespace yarp::os::impl;
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





class ControlBoardRemapperTest : public UnitTest
{
public:
    virtual ConstString getName() { return "ControlBoardRemapperTest"; }

    void checkRemapper(yarp::dev::PolyDriver & ddRemapper, int rand, size_t nrOfRemappedAxes)
    {
        IPositionControl2 *pos = NULL;
        bool ok = ddRemapper.view(pos);
        checkTrue(ok, "interface position correctly opened");
        int axes = 0;
        ok = pos->getAxes(&axes);
        checkTrue(ok, "getAxes returned correctly");
        checkEqual(axes, nrOfRemappedAxes, "remapper seems functional");

        IPositionDirect *posdir = 0;
        ok = ddRemapper.view(posdir);
        checkTrue(ok, "direct position interface correctly opened");

        IEncoders * encs = 0;
        ok = ddRemapper.view(encs);
        checkTrue(ok, "encoders interface correctly opened");

        IControlMode2 *ctrlmode = NULL;
        ok = ddRemapper.view(ctrlmode);
        checkTrue(ok, "control mode interface correctly opened");

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

        ok = ctrlmode->setControlModes(settedControlMode.data());
        checkTrue(ok, "setControlModes correctly called");

        // Check that the readed control mode is actually position direct
        // Let's try 10 times because if the remapper is using some remotecontrolboards,
        // it is possible that this return false if it is called before the first message
        // has been received from the controlboardwrapper
        ok = false;
        for(int wait=0; wait < 10 && !ok; wait++)
        {
            ok = ctrlmode->getControlModes(readedControlMode.data());
            yarp::os::Time::delay(0.001);
        }

        checkTrue(ok, "getControlModes correctly called");

        for(size_t i=0; i < nrOfRemappedAxes; i++)
        {
            checkEqual(settedControlMode[i],readedControlMode[i],"Setted control mode and readed control mode match");
        }

        // Test position direct methods

        // Set position
        ok = posdir->setPositions(setPosition.data());
        checkTrue(ok, "setPositions correctly called");

        // Set also the speeds in the mean time, so we are sure that we don't get
        // spurios successful set/get of position because of intermediate buffers
        ok = pos->setRefSpeeds(setRefSpeeds.data());
        checkTrue(ok, "setRefSpeeds correctly called");

        // Wait some time to make sure that the vector has been correctly propagated
        // back and forth
        yarp::os::Time::delay(0.1);

        // Read position
        ok = posdir->getRefPositions(readedPosition.data());
        checkTrue(ok, "getRefPositions correctly called");

        // Check that the two vector match
        for(size_t i=0; i < nrOfRemappedAxes; i++)
        {
            checkEqual(setPosition[i],readedPosition[i],"Setted position and readed ref position match");
        }

        // Do a similar test for the encoders
        // in fakeMotionControl their value is the one setted with setPosition
        ok = encs->getEncoders(readedEncoders.data());
        checkTrue(ok, "getEncoders correctly called");

        // Check that the two vector match
        for(size_t i=0; i < nrOfRemappedAxes; i++)
        {
            checkEqual(setPosition[i],readedEncoders[i],"Setted position and readed encoders match");
        }
    }

    void testControlBoardRemapper() {
        report(0,"\ntest the controlboard remapper");

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

            bool result;
            result = fmcbs[i]->open(p);
            checkTrue(result, "fakeMotionControlBoard open reported successful");


            if(result)
            {
                IPositionControl *pos = NULL;
                result = fmcbs[i]->view(pos);
                checkTrue(result, "interface position correctly opened");
                int axes = 0;
                pos->getAxes(&axes);
                checkEqual(axes, fmcbsSizes[i], "fakeMotionControlBoard seems functional");
            }

            // Open the wrapper
            wrappers[i] = new PolyDriver();

            if(i==0) { p.fromConfig(wrapperA_file_content); }
            if(i==1) { p.fromConfig(wrapperB_file_content); }
            if(i==2) { p.fromConfig(wrapperC_file_content); }

            result = wrappers[i]->open(p);
            checkTrue(result, "controlboardwrapper2 open reported successful");

            yarp::dev::IMultipleWrapper *iwrap = 0;
            result = wrappers[i]->view(iwrap);
            checkTrue(result, "interface for multiple wrapper correctly opened for the controlboardwrapper2");

            PolyDriverList pdList;
            pdList.push(fmcbs[i],wrapperNetworks[i].c_str());

            result = iwrap->attachAll(pdList);
            checkTrue(result, "controlboardwrapper2 attached successfully to the device");
        }

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

        bool ok = ddRemapper.open(pRemapper);
        checkTrue(ok,"controlboardremapper open reported successful");

        // Attach the fake motion boards to the remapper
        yarp::dev::PolyDriverList fmcList;

        for(int i=0; i < 3; i++)
        {
            fmcList.push(fmcbs[i],fmcbsNames[i].c_str());
        }

        yarp::dev::IMultipleWrapper *imultwrap = 0;
        ok = ddRemapper.view(imultwrap);
        checkTrue(ok, "interface for multiple wrapper correctly opened");

        ok = imultwrap->attachAll(fmcList);

        checkTrue(ok, "attachAll for controlboardremapper successful");

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


        ok = ddRemoteRemapper.open(pRemoteRemapper);
        checkTrue(ok,"remotecontrolboardremapper open reported successful, testing it");

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
            wrappers[i] = 0;

            fmcbs[i]->close();
            delete fmcbs[i];
            fmcbs[i] = 0;
        }
    }

    virtual void runTests() {
        Network::setLocalMode(true);
        testControlBoardRemapper();
        Network::setLocalMode(false);
    }
};

static ControlBoardRemapperTest theControlBoardRemapperTest;

UnitTest& getControlBoardRemapperTest() {
    return theControlBoardRemapperTest;
}

/**
 * Copyright (C) 2016 RobotCub Consortium
 * Authors: Silvio Traversaro
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <vector>

#include <yarp/os/impl/UnitTest.h>

#include <yarp/dev/ControlBoardInterfaces.h>
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


class ControlBoardRemapperTest : public UnitTest
{
public:
    virtual String getName() { return "ControlBoardRemapperTest"; }

    void testControlBoardRemapper() {
        report(0,"\ntest the controlboard remapper");

        // We first allocate three fakeMotionControl boards,
        // that we will remap using the remapper
        std::vector<PolyDriver *> fmcbs;
        fmcbs.resize(3);

        std::vector<int> fmcbsSizes;
        fmcbsSizes.push_back(2);
        fmcbsSizes.push_back(3);
        fmcbsSizes.push_back(4);

        std::vector<std::string> fmcbsNames;
        fmcbsNames.push_back("fakeControlBoardA");
        fmcbsNames.push_back("fakeControlBoardB");
        fmcbsNames.push_back("fakeControlBoardC");


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
        }

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

        bool result = ddRemapper.open(pRemapper);
        checkTrue(result,"controlboardremapper open reported successful");

        // Attach the fake motion boards to the remapper
        yarp::dev::PolyDriverList fmcList;

        for(int i=0; i < 3; i++)
        {
            fmcList.push(fmcbs[i],fmcbsNames[i].c_str());
        }

        yarp::dev::IMultipleWrapper *imultwrap = 0;
        result = ddRemapper.view(imultwrap);
        checkTrue(result, "interface for multiple wrapper correctly opened");

        result = imultwrap->attachAll(fmcList);

        checkTrue(result, "attachAll successful");

        IPositionControl *pos = NULL;
        result = ddRemapper.view(pos);
        checkTrue(result, "interface position correctly opened");
        int axes = 0;
        result = pos->getAxes(&axes);
        checkTrue(result, "getAxes returned correctly");
        checkEqual(axes, 6, "remapper seems functional");

        IPositionDirect *posdir = 0;
        result = ddRemapper.view(posdir);
        checkTrue(result, "direct position interface correctly opened");

        std::vector<double> setPosition(nrOfRemappedAxes), readedPosition(nrOfRemappedAxes);

        for(size_t i=0; i < nrOfRemappedAxes; i++)
        {
            setPosition[i]  = i*100.0;
        }

        // Set position
        posdir->setPositions(setPosition.data());

        // Read position
        posdir->getRefPositions(readedPosition.data());

        // Check that the two vector match
        for(size_t i=0; i < nrOfRemappedAxes; i++)
        {
            checkEqual(setPosition[i],readedPosition[i],"Setted position and readed position match");
        }

        // Close devices
        imultwrap->detachAll();
        ddRemapper.close();

        for(int i=0; i < 3; i++)
        {
            fmcbs[i]->close();
            delete fmcbs[i];
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

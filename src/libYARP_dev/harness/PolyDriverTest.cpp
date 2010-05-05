// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/os/impl/String.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/FrameGrabberInterfaces.h>

#include "TestList.h"

using namespace yarp::os;
using namespace yarp::os::impl;
using namespace yarp::sig;
using namespace yarp::dev;

class PolyDriverTest : public UnitTest {
public:
    virtual String getName() { return "PolyDriverTest"; }

    void testBasic() {
        report(0,"a very basic driver instantiation test");
        PolyDriver dd;
        Property p;
        p.put("device","test_grabber");
        bool result;
        result = dd.open(p);
        checkTrue(result,"open reported successful");
        IFrameGrabberImage *grabber = NULL;
        result = dd.view(grabber);
        checkTrue(result,"interface reported");
        ImageOf<PixelRgb> img;
        grabber->getImage(img);
        checkTrue(img.width()>0,"interface seems functional");
        result = dd.close();
        checkTrue(result,"close reported successful");
    }

    void testMonitor() {
        report(0,"test monitoring");
        Property p;
        p.put("device","grabber");
        p.put("subdevice","test_grabber");
        p.put("verbose",1);
        p.put("wrapped",1);
        PolyDriver dd(p);
        Bottle opts = dd.getOptions();
        checkTrue(opts.size()>0,"some options reported");
        //printf("Opts: %s\n", opts.toString().c_str());
        dd.close();
    }

    virtual void runTests() {
        Network::setLocalMode(true);
        testBasic();
        testMonitor();
        Network::setLocalMode(false);
    }
};

static PolyDriverTest thePolyDriverTest;

UnitTest& getPolyDriverTest() {
    return thePolyDriverTest;
}


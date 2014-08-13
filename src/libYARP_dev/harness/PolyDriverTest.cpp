// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/impl/String.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/ControlBoardInterfaces.h>

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

    void testPropertyBug() {
        // guard against a bug reported by Martin Peniak
        report(0,"test Property bug reported by Martin Peniak");
        Property p;
        p.put("device","grabber");
        p.put("subdevice","test_grabber");
        p.put("verbose",1);
        p.put("wrapped",1);
        for (int i=0; i<5; i++) {
            PolyDriver *dd = new PolyDriver(p);
            delete dd;
            // bug may cause crash due to Monitor reporting to deallocated
            // driver
        }
    }


    void testGroup() {
        report(0,"make sure groups of devices can be instantiated correctly");
        Property p;
        p.fromConfig("\
device group\n\
\n\
[part mymotor]\n\
device test_motor\n\
axes 10\n\
\n\
[part mycam]\n\
device test_grabber\n\
\n\
[part broadcast]\n\
device controlboard\n\
subdevice mymotor\n\
name /mymotor\n\
");
        p.put("verbose",1);
        PolyDriver dd(p);
        Bottle cmd("get axes"), reply;
        Network::write(Contact::byName("/mymotor/rpc:i"),cmd,reply);
        checkEqual(reply.get(2).asInt(),10,"axis count is correct");
    }

    void testGrabber() {
        report(0,"test the grabber wrapper");
        PolyDriver dd;
        Property p;
        p.put("device","grabber");
        p.put("subdevice","test_grabber");
        p.put("name","/grabber");
        bool result;
        result = dd.open(p);
        checkTrue(result,"server open reported successful");

        PolyDriver dd2;
        Property p2;
        p2.put("device","remote_grabber");
        p2.put("remote","/grabber");
        p2.put("local","/grabber/client");
        result = dd2.open(p2);
        checkTrue(result,"client open reported successful");

        IFrameGrabberImage *grabber = NULL;
        result = dd2.view(grabber);
        checkTrue(result,"interface reported");
        ImageOf<PixelRgb> img;
        grabber->getImage(img);
        checkTrue(img.width()>0,"interface seems functional");
        result = dd.close() && dd2.close();
        checkTrue(result,"close reported successful");
    }

    void testControlBoard() {
        report(0,"test the controlboard wrapper");
        PolyDriver dd;
        Property p;
        p.put("device","controlboard");
        p.put("subdevice","test_motor");
        p.put("name","/motor");
        p.put("axes",16);
        bool result;
        result = dd.open(p);
        checkTrue(result,"server open reported successful");

        PolyDriver dd2;
        Property p2;
        p2.put("device","remote_controlboard");
        p2.put("remote","/motor");
        p2.put("local","/motor/client");
        p2.put("carrier","tcp");
        result = dd2.open(p2);
        checkTrue(result,"client open reported successful");

        IPositionControl *pos = NULL;
        result = dd2.view(pos);
        checkTrue(result,"interface reported");
        int axes = 0;
        pos->getAxes(&axes);
        checkEqual(axes,16,"interface seems functional");
        result = dd.close() && dd2.close();
        checkTrue(result,"close reported successful");
    }

    virtual void runTests() {
        Network::setLocalMode(true);
        testBasic();
        testMonitor();
        testPropertyBug();
        testGroup();
        testGrabber();
        testControlBoard();
        Network::setLocalMode(false);
    }
};

static PolyDriverTest thePolyDriverTest;

UnitTest& getPolyDriverTest() {
    return thePolyDriverTest;
}


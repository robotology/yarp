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
#include <yarp/dev/Wrapper.h>

#include "TestList.h"

using namespace yarp::os;
using namespace yarp::os::impl;
using namespace yarp::sig;
using namespace yarp::dev;


class DeviceDriverTest : public DeviceDriver
{
};

class DeprecatedDeviceDriverTest : public DeprecatedDeviceDriver
{
};

class PolyDriverTest : public UnitTest {
public:
    virtual String getName() { return "PolyDriverTest"; }

    void testDeprecated() {
        report(0,"deprecated device test");
        {
            PolyDriver dd;
            Property p;
            p.put("device", "devicedrivertest");
            bool result;
            result = dd.open(p);
            checkTrue(result,"open reported successful");
        }
        {
            PolyDriver dd;
            Property p;
            p.put("device", "deprecateddevicedrivertest");
            bool result;
            result = dd.open(p);
            checkFalse(result,"open failed as expected");
            p.put("allow-deprecated-devices", "1");
            result = dd.open(p);
            checkTrue(result,"open reported successful");
       }
    }
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
device controlboardwrapper2\n\
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

#ifndef YARP_NO_DEPRECATED
    void testControlBoard() {
        report(0,"test the controlboard wrapper");
        PolyDriver dd;
        Property p;
        p.put("device","controlboard");
        p.put("subdevice","test_motor");
        p.put("name","/motor");
        p.put("axes",16);
        p.put("allow-deprecated-devices", "1");
        bool result;
        result = dd.open(p);
        checkTrue(result,"server open reported successful");

        PolyDriver dd2;
        Property p2;
        p2.put("device","clientcontrolboard");
        p2.put("remote","/motor");
        p2.put("local","/motor/client");
        p2.put("carrier","tcp");
        p2.put("ignoreProtocolCheck","true");
        p2.put("allow-deprecated-devices", "1");
        result = dd2.open(p2);
        checkTrue(result,"client open reported successful");

        if(!result)   return;  // cannot go on if the device was not opened

        IPositionControl *pos = NULL;
        result = dd2.view(pos);
        checkTrue(result,"interface reported");
        int axes = 0;
        pos->getAxes(&axes);
        checkEqual(axes,16,"interface seems functional");
        result = dd.close() && dd2.close();
        checkTrue(result,"close reported successful");
    }
#endif // YARP_NO_DEPRECATED

    void testControlBoard2() {
        report(0,"\ntest the controlboard wrapper 2");
        PolyDriver dd;
        Property p;
        p.put("device","controlboardwrapper2");
        p.put("subdevice","test_motor");
        p.put("name","/motor");
        p.put("axes",16);
        bool result;
        result = dd.open(p);
        checkTrue(result,"controlboardwrapper open reported successful");

        // Check if IMultipleWrapper interface is correctly found
        yarp::dev::IMultipleWrapper * i_mwrapper=0;
        result = dd.view(i_mwrapper);
        checkTrue(result,"IMultipleWrapper view reported successful");
        checkTrue(i_mwrapper!=0,"IMultipleWrapper pointer not null");

        PolyDriver dd2;
        Property p2;
        p2.put("device","remote_controlboard");
        p2.put("remote","/motor");
        p2.put("local","/motor/client");
        p2.put("carrier","tcp");
        p2.put("ignoreProtocolCheck","true");
        result = dd2.open(p2);
        checkTrue(result,"remote_controlboard open reported successful");

        if(!result)   return;  // cannot go on if the device was not opened

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
        Drivers::factory().add(new DriverCreatorOf<DeviceDriverTest>("devicedrivertest",
                                                                     "devicedrivertest",
                                                                     "DeviceDriverTest"));
        Drivers::factory().add(new DriverCreatorOf<DeprecatedDeviceDriverTest>("deprecateddevicedrivertest",
                                                                               "deprecateddevicedrivertest",
                                                                               "DeprecatedDeviceDriverTest"));
        testDeprecated();
        testBasic();
        testMonitor();
        testPropertyBug();
        testGroup();
        testGrabber();
#ifndef YARP_NO_DEPRECATED
        testControlBoard();
#endif // YARP_NO_DEPRECATED
        testControlBoard2();
        Network::setLocalMode(false);
    }
};

static PolyDriverTest thePolyDriverTest;

UnitTest& getPolyDriverTest() {
    return thePolyDriverTest;
}


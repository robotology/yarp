/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/dev/PolyDriver.h>

#include <yarp/os/Network.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/Wrapper.h>

#include <string>

#if defined(USE_SYSTEM_CATCH)
#include <catch.hpp>
#else
#include "catch.hpp"
#endif

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;


class DeviceDriverTest : public DeviceDriver,
                         public IFrameGrabberImage
{
public:
    virtual bool open(yarp::os::Searchable& config) override
    {
        w = config.check("width",yarp::os::Value(128),
                         "desired width of test image").asInt32();
        h = config.check("height",yarp::os::Value(128),
                         "desired height of test image").asInt32();
        return true;

    }

    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) override
    {
        image.resize(w, h);
        image.zero();
        return true;
    }

    virtual int height() const override { return h; }

    virtual int width() const override { return w; }

private:
    int w, h;
};

class DeprecatedDeviceDriverTest : public DeprecatedDeviceDriver
{
};




TEST_CASE("dev::PolyDriverTest", "[yarp::dev]") {

    Network::setLocalMode(true);

    Drivers::factory().add(new DriverCreatorOf<DeviceDriverTest>("devicedrivertest",
                                                                 "devicedrivertest",
                                                                 "DeviceDriverTest"));
    Drivers::factory().add(new DriverCreatorOf<DeprecatedDeviceDriverTest>("deprecateddevicedrivertest",
                                                                           "deprecateddevicedrivertest",
                                                                           "DeprecatedDeviceDriverTest"));

    SECTION("deprecated device test (test 1)")
    {
        PolyDriver dd;
        Property p;
        p.put("device", "devicedrivertest");
        bool result;
        result = dd.open(p);
        REQUIRE(result); // open reported successful
    }

    SECTION("deprecated device test (test 1)")
    {
        PolyDriver dd;
        Property p;
        p.put("device", "deprecateddevicedrivertest");
        bool result;
        result = dd.open(p);
        CHECK_FALSE(result); // open failed as expected
        p.put("allow-deprecated-devices", "1");
        result = dd.open(p);
        CHECK(result); // open reported successful
    }

    SECTION("a very basic driver instantiation test")
    {
        PolyDriver dd;
        Property p;
        p.put("device","devicedrivertest");
        bool result;
        result = dd.open(p);
        REQUIRE(result); // open reported successful
        IFrameGrabberImage *grabber = nullptr;
        result = dd.view(grabber);
        REQUIRE(result); // interface reported
        ImageOf<PixelRgb> img;
        grabber->getImage(img);
        CHECK(img.width() > 0); // interface seems functional
        result = dd.close();
        CHECK(result); // close reported successful
    }

    SECTION("test monitoring")
    {
        Property p;
        p.put("device","grabber");
        p.put("subdevice","devicedrivertest");
        p.put("verbose",1);
        p.put("wrapped",1);
        PolyDriver dd(p);
        Bottle opts = dd.getOptions();
        CHECK(opts.size() > 0); // some options reported
        //printf("Opts: %s\n", opts.toString().c_str());
        dd.close();
    }

    SECTION("test Property bug")
    {
        Property p;
        p.put("device","grabber");
        p.put("subdevice","devicedrivertest");
        p.put("verbose",1);
        p.put("wrapped",1);
        for (int i=0; i<5; i++) {
            PolyDriver *dd = new PolyDriver(p);
            delete dd;
            // bug may cause crash due to Monitor reporting to deallocated
            // driver
        }
    }

    SECTION("make sure groups of devices can be instantiated correctly")
    {
        Property p;
        p.fromConfig("\
device group\n\
\n\
[part mymotor]\n\
device test_motor\n\
axes 10\n\
\n\
[part broadcast]\n\
device controlboardwrapper2\n\
subdevice mymotor\n\
name /mymotor\n\
");
        p.put("verbose",1);
        PolyDriver dd(p);
        Bottle cmd("get axes"), reply;
        Network::write(Contact("/mymotor/rpc:i"), cmd, reply);
        CHECK(reply.get(2).asInt32() == 10); // axis count is correct
    }

    SECTION("test the controlboard wrapper 2")
    {
        PolyDriver dd;
        Property p;
        p.put("device","controlboardwrapper2");
        p.put("subdevice","test_motor");
        p.put("name","/motor");
        p.put("axes",16);
        bool result;
        result = dd.open(p);
        REQUIRE(result); // controlboardwrapper open reported successful

        // Check if IMultipleWrapper interface is correctly found
        yarp::dev::IMultipleWrapper * i_mwrapper=nullptr;
        result = dd.view(i_mwrapper);
        REQUIRE(result); // IMultipleWrapper view reported successful
        REQUIRE(i_mwrapper != nullptr); // IMultipleWrapper pointer not null

        PolyDriver dd2;
        Property p2;
        p2.put("device","remote_controlboard");
        p2.put("remote","/motor");
        p2.put("local","/motor/client");
        p2.put("carrier","tcp");
        p2.put("ignoreProtocolCheck","true");
        result = dd2.open(p2);
        REQUIRE(result); // remote_controlboard open reported successful

        IPositionControl *pos = nullptr;
        result = dd2.view(pos);
        REQUIRE(result); // interface reported
        int axes = 0;
        pos->getAxes(&axes);
        CHECK(axes == 16); // interface seems functional
        CHECK(dd.close()); // close dd reported successful
        CHECK(dd2.close()); // close dd2 reported successful
    }

    Network::setLocalMode(false);
}

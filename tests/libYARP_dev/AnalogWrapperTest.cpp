/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <string>
#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/PolyDriver.h>

#include "TestList.h"

using namespace yarp::os;
using namespace yarp::os::impl;
using namespace yarp::dev;



class AnalogWrapperTest : public UnitTest
{
public:
    virtual std::string getName() override { return "AnalogWrapperTest"; }

    void testAnalogWrapper() {
        report(0,"\ntest the Analogwrapper ");
        bool result;

        yInfo() << "Test 1";
        //First test: I try to make it open a single port and I check its existence and of the relative rpc port
        {
            PolyDriver dd;
            Property p;
            p.put("device","analogServer");
            p.put("name","/testAnalogWrapper");
            p.put("period",20);
            p.put("channels",10);
            p.put("subdevice","fakeAnalogSensor");
            result = dd.open(p);
            Network::sync("/testAnalogWrapper/rpc:i");
            Network::sync("/testAnalogWrapper");
            checkTrue(result,"Open of AnalogWrapper1 with one port open reported successful");
            result = Network::exists("/testAnalogWrapper/rpc:i");
            result &= Network::exists("/testAnalogWrapper");
            checkTrue(result,"ports created succefully");
            result =  dd.close();
            checkTrue(result,"close reported successful");
        }


        yDebug("\n\n---------------------------------------\n\n");

        yInfo() << "Test 2";
        //Second test: I try to open a device after opening the same port that it will open
        {
            BufferedPort<Bottle> portrpc;
            result = portrpc.open("/testAnalogWrapper2/rpc:i");
            checkTrue(result,"open reported successful");
            result = Network::sync("/testAnalogWrapper2/rpc:i");
            checkTrue(result,"sync reported successful");

            BufferedPort<Bottle> port;
            result = port.open("/testAnalogWrapper2");
            checkTrue(result,"open reported successful");
            result = Network::sync("/testAnalogWrapper2");
            checkTrue(result,"sync reported successful");

            Property p;
            p.put("device","analogServer");
            p.put("name","/testAnalogWrapper2");
            p.put("period",20);
            p.put("channels",10);
            p.put("subdevice","fakeAnalogSensor");

            PolyDriver dd2;
            result = dd2.open(p);
            checkFalse(result,"Open of AnalogWrapper2 with one port open failed as expected(address conflict)");
            result = dd2.isValid();
            checkFalse(result,"isValid of AnalogWrapper2 with one port failed as expected open(address conflict)");
            portrpc.close();
            port.close();
            result = dd2.close();
            checkTrue(result,"close reported successful");
        }

        yDebug("\n\n---------------------------------------\n\n");

        yInfo() << "Test 3";
        //Third test: the same of before but with multiple ports
        {
            PolyDriver dd3;
            Property p2;
            p2.put("device","analogServer");
            p2.put("name","/testAnalogWrapperMultip");
            p2.put("period",20);
            p2.put("channels",3);
            p2.put("subdevice","fakeAnalogSensor");
            p2.fromString("(ports (left_hand left_forearm left_arm)) (left_hand 0 0 0 0) (left_forearm 0 0 0 0) (left_arm 0 0 0 0)", false);
            result = dd3.open(p2);
            Network::sync("/testAnalogWrapperMultip/left_hand");
            Network::sync("/testAnalogWrapperMultip/left_hand/rpc:i");
            Network::sync("/testAnalogWrapperMultip/left_forearm");
            Network::sync("/testAnalogWrapperMultip/left_forearm/rpc:i");
            Network::sync("/testAnalogWrapperMultip/left_arm");
            Network::sync("/testAnalogWrapperMultip/left_arm/rpc:i");
            checkTrue(result,"Open of AnalogWrapper with multiple port open reported successful");
            result = dd3.isValid();
            checkTrue(result,"isValid of AnalogWrapper with multiple port open reported successful");
            checkFalse(Network::exists("/rpc:i") || Network::exists("/rpc:o"),"/rpc:i and /rpc:o has not been opened");
            result=Network::exists("/testAnalogWrapperMultip/left_hand");
            result&=Network::exists("/testAnalogWrapperMultip/left_hand/rpc:i");
            result&=Network::exists("/testAnalogWrapperMultip/left_forearm");
            result&=Network::exists("/testAnalogWrapperMultip/left_forearm/rpc:i");
            result&=Network::exists("/testAnalogWrapperMultip/left_arm");
            result&=Network::exists("/testAnalogWrapperMultip/left_arm/rpc:i");
            checkTrue(result,"All the ports required has been opened succesfully");
            result = dd3.close();
            checkTrue(result,"close reported successful");
        }

        yDebug("\n\n---------------------------------------\n\n");

        yInfo() << "Test 4";
        //Fourth test: test for address confict with multiple ports
        {
            PolyDriver dd4;
            Property p2;
            BufferedPort<Bottle> port2,portrpc2;
            port2.open("/testAnalogWrapperMultip2/left_hand");
            portrpc2.open("/testAnalogWrapperMultip2/left_forearm/rpc:i");
            Network::sync("/testAnalogWrapperMultip2/left_hand");
            Network::sync("/testAnalogWrapperMultip2/left_forearm/rpc:i");
            p2.put("device","analogServer");
            p2.put("name","/testAnalogWrapperMultip2");
            p2.put("period",20);
            p2.put("channels",3);
            p2.put("subdevice","fakeAnalogSensor");
            p2.fromString("(ports (left_hand left_forearm left_arm)) (left_hand 0 0 0 0) (left_forearm 0 0 0 0) (left_arm 0 0 0 0)", false);
            result = dd4.open(p2);
            checkFalse(result,"Open of AnalogWrapper2 with multiple ports open failed as expected(address conflict)");
            result = dd4.isValid();
            checkFalse(result,"isValid of AnalogWrapper2 with multiple ports failed as expected open(address conflict)");
            portrpc2.close();
            port2.close();
            result = dd4.close();
            checkTrue(result,"close reported successful");
        }
    }

    virtual void runTests() override {
        Network::setLocalMode(true);
        testAnalogWrapper();
        Network::setLocalMode(false);
    }
};

static AnalogWrapperTest theAnalogWrapperTest;

UnitTest& getAnalogWrapperTest() {
    return theAnalogWrapperTest;
}

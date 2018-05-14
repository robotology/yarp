/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/PortablePair.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>

#include <yarp/os/impl/UnitTest.h>
//#include "TestList.h"

using namespace yarp::os::impl;
using namespace yarp::os;

class PortablePairTest : public UnitTest {
public:
    virtual std::string getName() override { return "PortablePairTest"; }
    
    void checkStandard() {
        PortablePair<Bottle,Bottle> pp;
        pp.head.fromString("1 2 3");
        pp.body.fromString("yes no");
        BufferedConnectionWriter writer;
        pp.write(writer);
        std::string s = writer.toString();
        Bottle bot;
        bot.fromBinary(s.c_str(),s.length());
        checkEqual(bot.size(),2,"it is a pair");  
        checkEqual(bot.get(0).asList()->size(),3,"head len is right");  
        checkEqual(bot.get(1).asList()->size(),2,"body len is right");  
    }



    void checkTransmit() {
        report(0,"testing pair transmission...");

        PortablePair<Bottle,Bottle> pp;
        pp.head.fromString("1 2 3");
        pp.body.fromString("yes no");

        PortReaderBuffer< PortablePair<Bottle,Bottle> > buf;

        Port input, output;
        input.open("/in");
        output.open("/out");
        buf.setStrict();
        buf.attach(input);
        Network::connect("/out","/in");

        report(0,"writing...");
        output.write(pp);
        report(0,"reading...");
        PortablePair<Bottle,Bottle> *result = buf.read();
        
        checkTrue(result!=nullptr,"got something check");
        if (result!=nullptr) {
            checkEqual(result->head.size(),3,"head len is right");  
            checkEqual(result->body.size(),2,"body len is right");  
        }

        output.close();
        input.close();
    }

    
    virtual void runTests() override {
        checkStandard();
        bool netMode = Network::setLocalMode(true);
        checkTransmit();
        Network::setLocalMode(netMode);
    }
};

static PortablePairTest thePortablePairTest;

UnitTest& getPortablePairTest() {
    return thePortablePairTest;
}

/*
 * Copyright (C) 2007 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <math.h>

#include <yarp/os/Stamp.h>
#include <yarp/os/all.h>

#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/impl/StreamConnectionReader.h>

#include <yarp/os/impl/UnitTest.h>
//#include "TestList.h"

using namespace yarp::os::impl;
using namespace yarp::os;

class StampTest : public UnitTest {
public:
    virtual ConstString getName() { return "StampTest"; }

    void checkFormat() {
        report(0, "checking Stamp can serialize ok...");

        for (int i=0; i<=1; i++) {
            DummyConnector con;

            bool textMode = (i==0);
            if (textMode) {
                report(0, "checking in text mode");
            } else {
                report(0, "checking in binary mode");
            }
            con.setTextMode(textMode);

            Stamp stamp(55,1.0);
            stamp.write(con.getWriter());
            Bottle bot;
            bot.read(con.getReader());
            checkEqual(bot.get(0).asInt(),55,"sequence number write");
            checkTrue(fabs(bot.get(1).asDouble()-1)<0.0001,"time stamp write");

            stamp.write(con.getCleanWriter());
            Stamp outStamp;
            outStamp.read(con.getReader());

            checkEqual(outStamp.getCount(),55,"sequence number read");
            checkTrue(fabs(outStamp.getTime()-1)<0.0001,"time stamp read");
        }

    }

    void checkEnvelope(const char *mode) {

        BufferedPort<Bottle> in;
        BufferedPort<Bottle> out;

        in.setStrict();
        in.open("/in");
        out.open("/out");
        Network::connect("/out","/in",mode);

        Bottle& outBot1 = out.prepare();   // Get the object
        outBot1.fromString("hello world"); // Set it up the way we want
        Stamp stamp(55,1.0);
        out.setEnvelope(stamp);
        out.write();                       // Now send it on its way

        Bottle& outBot2 = out.prepare();
        outBot2.fromString("2 3 5 7 11");
        Stamp stamp2(55,4.0);
        out.setEnvelope(stamp2);
        out.writeStrict();                 // writeStrict() will wait for any

        do {
            Time::delay(0.1);
        } while (in.getPendingReads()<2);

        // Read the first object
        in.read();
        Stamp inStamp;
        in.getEnvelope(inStamp);
        checkEqualish(inStamp.getTime(),1,"time stamp 1 read");

        // Read the second object
        in.read();
        in.getEnvelope(inStamp);
        checkEqualish(inStamp.getTime(),4,"time stamp 2 read");
    }

    void checkString() {
        report(0,"check string serialization");
        {
            Stamp env(42,3.0);
            BufferedConnectionWriter buf(true);
            env.write(buf);
            ConstString str = buf.toString();
            Bottle bot(str.c_str());
            checkEqual(bot.get(0).asInt(),42,"sequence ok");
            checkEqualish(bot.get(1).asDouble(),3,"time ok");
        }
    }

    virtual void runTests() {
        // add tests here
        Network::setLocalMode(true);
        checkFormat();
        report(0, "checking envelopes work...");
        checkEnvelope("tcp");
        report(0, "checking envelopes work (text mode)...");
        checkEnvelope("text");
        checkString();
        Network::setLocalMode(false);
    }
};

static StampTest theStampTest;

UnitTest& getStampTest() {
    return theStampTest;
}

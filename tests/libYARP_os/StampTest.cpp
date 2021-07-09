/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Stamp.h>

#include <yarp/os/BufferedPort.h>
#include <yarp/os/DummyConnector.h>
#include <yarp/os/Network.h>

#include <yarp/os/impl/BufferedConnectionWriter.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;
using yarp::os::impl::BufferedConnectionWriter;

static void checkEnvelope(const char *mode)
{
    BufferedPort<Bottle> in;
    BufferedPort<Bottle> out;

    in.setStrict();
    in.open("/in");
    out.open("/out");
    Network::connect("/out", "/in", mode);

    Bottle& outBot1 = out.prepare();   // Get the object
    outBot1.fromString("hello world"); // Set it up the way we want
    Stamp stamp(55, 1.0);
    out.setEnvelope(stamp);
    out.write();                       // Now send it on its way

    Bottle& outBot2 = out.prepare();
    outBot2.fromString("2 3 5 7 11");
    Stamp stamp2(55, 4.0);
    out.setEnvelope(stamp2);
    out.writeStrict();                 // writeStrict() will wait for any

    do {
        Time::delay(0.1);
    } while (in.getPendingReads() < 2);

    // Read the first object
    in.read();
    Stamp inStamp;
    in.getEnvelope(inStamp);
    CHECK(inStamp.getTime() == Approx(1)); // time stamp 1 read

    // Read the second object
    in.read();
    in.getEnvelope(inStamp);
    CHECK(inStamp.getTime() == Approx(4)); // time stamp 2 read
}


TEST_CASE("os::StampTest", "[yarp::os]")
{
    Network::setLocalMode(true);

    SECTION("checking Stamp can serialize ok")
    {
        for (int i=0; i<=1; i++) {
            DummyConnector con;

            bool textMode = (i==0);
            if (textMode) {
                INFO("checking in text mode");
            } else {
                INFO("checking in binary mode");
            }
            con.setTextMode(textMode);

            Stamp stampToWrite(55, 1.0);
            Stamp stampRead;

            stampToWrite.write(con.getWriter());
            Bottle bot;
            bot.read(con.getReader());

            CHECK(bot.get(0).asInt32() == 55); // sequence number write
            CHECK(bot.get(1).asFloat64() == Approx(1).epsilon(0.0001)); // time stamp write


            stampToWrite.write(con.getCleanWriter());
            stampRead.read(con.getReader());

            CHECK(stampRead.getCount() == 55); // sequence number read
            CHECK(stampRead.getTime() == Approx(1).epsilon(0.0001)); // time stamp read

            // Test extreme numbers as timestamp
            double exp = -DBL_DIG;
            double smallest = pow(10.0, exp);

            // Create a number like 0.123456789012345... using the maximum number of digits
            // the platform can support.

            double timeValue = smallest;
            INFO(timeValue);
            for(int i=2; i<DBL_DIG+1; i++)
            {
                timeValue = timeValue*10 + (i%10)*smallest;
            }
            stampToWrite.update(timeValue);

            stampToWrite.write(con.getCleanWriter());
            stampRead.read(con.getReader());

            // Check sequence number is updated automatically
            CHECK(stampRead.getCount() == 56); // sequence number read
            // Check the number is read back with error smaller than machine epsilon
            CHECK(stampRead.getTime() == Approx(timeValue).epsilon(DBL_EPSILON)); // time stamp read

            // Create a realistic timestamp with tenth of millisecond as granularity,
            // like 1234567890.12345
            timeValue = 1234567890.12345;
            stampToWrite.update(timeValue);

            stampToWrite.write(con.getCleanWriter());
            stampRead.read(con.getReader());

            // Check sequence number is updated automatically
            CHECK(stampRead.getCount() == 57); // sequence number read
            // Check the number is read back with error smaller than machine epsilon
            CHECK(stampRead.getTime() == Approx(timeValue).epsilon(DBL_EPSILON)); // time stamp read
        }
    }

    SECTION("checking envelopes work...")
    {
        checkEnvelope("tcp");
    }

    SECTION("checking envelopes work (text mode)")
    {
        checkEnvelope("text");
    }

    SECTION("check string serialization")
    {
        Stamp env(42, 3.0);
        BufferedConnectionWriter buf(true);
        env.write(buf);
        std::string str = buf.toString();
        Bottle bot(str.c_str());
        CHECK(bot.get(0).asInt32() == 42); // sequence ok
        CHECK(bot.get(1).asFloat64() == Approx(3)); // time ok
    }

    Network::setLocalMode(false);
}

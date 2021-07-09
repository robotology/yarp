/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Header.h>

#include <yarp/os/BufferedPort.h>
#include <yarp/os/DummyConnector.h>
#include <yarp/os/Network.h>
#include <yarp/os/Stamp.h>

#include <yarp/os/impl/BufferedConnectionWriter.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;
using yarp::os::impl::BufferedConnectionWriter;

// This cannot be constexpr because pow is not constexpr
static yarp::conf::float64_t extreme()
{
    constexpr yarp::conf::float64_t exp = -DBL_DIG;
    yarp::conf::float64_t smallest = pow(10.0, exp);

    // Create a number like 0.123456789012345... using the maximum number of digits
    // the platform can support.
    yarp::conf::float64_t timeValue = smallest;
    for(int i=2; i<DBL_DIG+1; i++) {
        timeValue = timeValue*10 + (i%10)*smallest;
    }
    return timeValue;
}

static constexpr yarp::conf::float64_t realistic()
{
    // Create a realistic timestamp with tenth of millisecond as granularity,
    // like 1234567890.12345
    return 1234567890.12345;
}

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
    Header header(54, 1.0, "firstFrameId");
    out.setEnvelope(header);
    out.write();                       // Now send it on its way

    Bottle& outBot2 = out.prepare();
    outBot2.fromString("2 3 5 7 11");
    Header header2(55, 4.0, "secondFrameId");
    out.setEnvelope(header2);
    out.writeStrict();                 // writeStrict() will wait for any

    do {
        Time::delay(0.1);
    } while (in.getPendingReads() < 2);

    // Read the first object
    in.read();
    Header inHeader;
    in.getEnvelope(inHeader);
    CHECK(inHeader.count() == 54);
    CHECK(inHeader.timeStamp() == Approx(1.0));
    CHECK(inHeader.frameId() == "firstFrameId");

    // Read the second object
    in.read();
    in.getEnvelope(inHeader);
    CHECK(inHeader.count() == 55);
    CHECK(inHeader.timeStamp() == Approx(4.0));
    CHECK(inHeader.frameId() == "secondFrameId");
}


TEST_CASE("os::HeaderTest", "[yarp::os]")
{
    Network::setLocalMode(true);

    SECTION("checking Header can serialize ok (with frameId)")
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

            Header headerToWrite(55, 1.0, "theFrameId");
            Header headerRead;

            headerToWrite.write(con.getWriter());
            Bottle bot;
            bot.read(con.getReader());

            CHECK(bot.size() == 3);
            CHECK(bot.get(0).asInt32() == 55); // sequence number write
            CHECK(bot.get(1).asFloat64() == Approx(1.0).epsilon(0.0001)); // time stamp write
            CHECK(bot.get(2).asString() == "theFrameId"); // frame id write


            headerToWrite.write(con.getCleanWriter());
            headerRead.read(con.getReader());

            CHECK(headerRead.count() == 55); // sequence number read
            CHECK(headerRead.timeStamp() == Approx(1.0).epsilon(0.0001)); // time stamp read
            CHECK(headerRead.frameId() == "theFrameId"); // frame id read

            // Test extreme numbers as timestamp
            yarp::conf::float64_t timeValue = extreme();
            INFO(timeValue);
            headerToWrite.update(timeValue);

            headerToWrite.write(con.getCleanWriter());
            headerRead.read(con.getReader());

            // Check sequence number is updated automatically
            CHECK(headerRead.count() == 56); // sequence number read
            // Check the number is read back with error smaller than machine epsilon
            CHECK(headerRead.timeStamp() == Approx(timeValue).epsilon(DBL_EPSILON)); // time stamp read
            // Check the frameId should not be changed
            CHECK(headerRead.frameId() == "theFrameId"); // frame id read

            // Test a realistic timestamp
            timeValue = realistic();
            headerToWrite.update(timeValue);

            headerToWrite.write(con.getCleanWriter());
            headerRead.read(con.getReader());

            // Check sequence number is updated automatically
            CHECK(headerRead.count() == 57); // sequence number read
            // Check the number is read back with error smaller than machine epsilon
            CHECK(headerRead.timeStamp() == Approx(timeValue).epsilon(DBL_EPSILON)); // time stamp read
            // Check the frameId should not be changed
            CHECK(headerRead.frameId() == "theFrameId"); // frame id read


            // Change the frame id
            headerToWrite.setFrameId("theNewFrameId");

            headerToWrite.write(con.getCleanWriter());
            headerRead.read(con.getReader());

            // Check sequence number is not changed
            CHECK(headerRead.count() == 57); // sequence number read
            // Check the number is read back with error smaller than machine epsilon
            CHECK(headerRead.timeStamp() == Approx(timeValue).epsilon(DBL_EPSILON)); // time stamp read
            // Check the frameId should be changed to the new value
            CHECK(headerRead.frameId() == "theNewFrameId"); // frame id read

        }
    }

    SECTION("checking Header can serialize ok (without frameId)")
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

            Header headerToWrite(55, 1.0);
            Header headerRead;

            headerToWrite.write(con.getWriter());
            Bottle bot;
            bot.read(con.getReader());

            CHECK(bot.size() == 2);
            CHECK(bot.get(0).asInt32() == 55); // sequence number write
            CHECK(bot.get(1).asFloat64() == Approx(1.0).epsilon(0.0001)); // time stamp write


            headerToWrite.write(con.getCleanWriter());
            headerRead.read(con.getReader());

            CHECK(headerRead.count() == 55); // sequence number read
            CHECK(headerRead.timeStamp() == Approx(1.0).epsilon(0.0001)); // time stamp read

            // Test extreme numbers as timestamp
            yarp::conf::float64_t timeValue = extreme();
            INFO(timeValue);
            headerToWrite.update(timeValue);

            headerToWrite.write(con.getCleanWriter());
            headerRead.read(con.getReader());

            // Check sequence number is updated automatically
            CHECK(headerRead.count() == 56); // sequence number read
            // Check the number is read back with error smaller than machine epsilon
            CHECK(headerRead.timeStamp() == Approx(timeValue).epsilon(DBL_EPSILON)); // time stamp read

            // Test a realistic timestamp
            timeValue = realistic();
            headerToWrite.update(timeValue);

            headerToWrite.write(con.getCleanWriter());
            headerRead.read(con.getReader());

            // Check sequence number is updated automatically
            CHECK(headerRead.count() == 57); // sequence number read
            // Check the number is read back with error smaller than machine epsilon
            CHECK(headerRead.timeStamp() == Approx(timeValue).epsilon(DBL_EPSILON)); // time stamp read
        }
    }

    SECTION("checking Header (with frame id) can serialize to a Stamp")
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

            Header headerToWrite(55, realistic(), "theFrameId");
            Stamp stampRead;

            headerToWrite.write(con.getWriter());
            stampRead.read(con.getReader());

            CHECK(stampRead.getCount() == 55); // sequence number read
            CHECK(stampRead.getTime() == Approx(realistic()).epsilon(0.0001)); // time stamp read
        }
    }

    SECTION("checking Header (without frame id) can serialize to a Stamp")
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

            Header headerToWrite(55, realistic());
            Stamp stampRead;

            headerToWrite.write(con.getWriter());
            stampRead.read(con.getReader());

            CHECK(stampRead.getCount() == 55); // sequence number read
            CHECK(stampRead.getTime() == Approx(realistic()).epsilon(0.0001)); // time stamp read
        }
    }

    SECTION("checking Stamp can serialize to a Header")
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

            Stamp stampToWrite(55, realistic());
            Header headerRead(42, 1.0, "theFrameId");

            stampToWrite.write(con.getWriter());
            headerRead.read(con.getReader());

            CHECK(headerRead.count() == 55); // sequence number read
            CHECK(headerRead.timeStamp() == Approx(realistic()).epsilon(0.0001)); // time stamp read
            CHECK(headerRead.frameId().empty()); // frame id read is empty
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
        Header env(42, 3.0);
        BufferedConnectionWriter buf(true);
        env.write(buf);
        std::string str = buf.toString();
        Bottle bot(str.c_str());
        CHECK(bot.get(0).asInt32() == 42); // sequence ok
        CHECK(bot.get(1).asFloat64() == Approx(3)); // time ok
    }

    Network::setLocalMode(false);
}

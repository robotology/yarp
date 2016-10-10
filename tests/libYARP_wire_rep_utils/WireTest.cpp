/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <cstdio>
#include <cstdlib>

#include "WireTwiddler.h"

#include <yarp/os/StringInputStream.h>
#include <yarp/os/Route.h>
#include <yarp/os/InputStream.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/impl/UnitTest.h>

using namespace yarp::os;
using namespace yarp::os::impl;


class WireTest : public UnitTest
{
public:
    virtual ConstString getName() { return "WireTest"; }

    void checkWire()
    {
        {
            report(0, "checking vector int32 1 *");
            char seq[] = {42, 0, 0, 0};
            testSequence(seq, sizeof(seq),"vector int32 1 *", Bottle("42"));
        }
        {
            report(0, "checking vector string 1 *");
            char seq[] = {5, 0, 0, 0, 'h', 'e', 'l', 'l', 'o'};
            testSequence(seq, sizeof(seq),"vector string 1 *", Bottle("hello"));
        }
        {
            report(0, "checking vector int32 *");
            char seq[] = {2, 0, 0, 0, 42, 0, 0, 0, 55, 0, 0, 0};
            testSequence(seq, sizeof(seq),"vector int32 *", Bottle("42 55"));
        }
        {
            report(0, "checking vector string *");
            char seq[] = {2, 0, 0, 0, 3, 0, 0, 0, 'f', 'o', 'o', 3, 0, 0, 0, 'b', 'a', 'r'};
            testSequence(seq, sizeof(seq),"vector string *", Bottle("foo bar"));
        }
        {
            report(0, "checking list 2 vector string * int32 *");
            char seq[] = {2, 0, 0, 0, 3, 0, 0, 0, 'f', 'o', 'o', 3, 0, 0, 0, 'b', 'a', 'r', 12, 0, 0, 0};
            testSequence(seq, sizeof(seq),"list 2 vector string * int32 *", Bottle("(foo bar) 12"));
        }
        {
            report(0, "checking list 3 vector string * int32 * vector int32 *");
            char seq[] = {2, 0, 0, 0, 3, 0, 0, 0, 'f', 'o', 'o', 3, 0, 0, 0, 'b', 'a', 'r', 12, 0, 0, 0, 2, 0, 0, 0, 42, 0, 0, 0, 24, 0, 0, 0};
            testSequence(seq, sizeof(seq),"list 3 vector string * int32 * vector int32 *", Bottle("(foo bar) 12 (42 24)"));
        }
        {
            report(0, "checking vector int32 2 *");
            char seq[] = {42, 0, 0, 0, 24, 0, 0, 0};
            testSequence(seq, sizeof(seq),"vector int32 2 *", Bottle("42 24"));
        }
        {
            report(0, "checking list 2 string * int32 *");
            char seq[] = {5, 0, 0, 0, 'h', 'e', 'l', 'l', 'o', 12, 0, 0, 0};
            testSequence(seq, sizeof(seq),"list 2 string * int32 *", Bottle("hello 12"));
        }
        {
            report(0, "checking list 3 string * int32 * int32 *");
            char seq[] = {5, 0, 0, 0, 'h', 'e', 'l', 'l', 'o', 12, 0, 0, 0, 42, 0, 0, 0};
            testSequence(seq, sizeof(seq),"list 3 string * int32 * int32 *", Bottle("hello 12 42"));
        }
        {
            report(0, "checking list 2 string * skip int32 * int32 *");
            char seq[] = {5, 0, 0, 0, 'h', 'e', 'l', 'l', 'o', 12, 0, 0, 0, 42, 0, 0, 0};
            testSequence(seq, sizeof(seq),"list 2 string * skip int32 * int32 *", Bottle("hello 42"), false);
        }
        {
            report(0, "checking list 1 skip int32 * int32 *");
            char seq[] = {33, 0, 0, 0, 42, 0, 0, 0};
            testSequence(seq, sizeof(seq),"list 1 skip int32 * int32 *", Bottle("42"), false);
        }
        {
            report(0, "checking skip int32 * list 1 int32 *");
            char seq[] = {99, 0, 0, 0, 42, 0, 0, 0};
            testSequence(seq, sizeof(seq),"skip int32 * list 1 int32 *", Bottle("42"), false);
        }
        {
            report(0, "checking skip int32 *");
            char seq[] = {99, 0, 0, 0};
            testSequence(seq, sizeof(seq),"skip int32 *", Bottle(), false);
        }

    }

    virtual void runTests()
    {
        NetworkBase::setLocalMode(true);

        checkWire();

        NetworkBase::setLocalMode(false);

    }

private:
    void testSequence(char *seq,
                      size_t len,
                      const char *fmt,
                      Bottle ref,
                      bool testWrite = true)
    {
        char err[1024];
        printf("\n");
        printf("================================================\n");
        printf(" READ %s\n", fmt);
        Bytes b1(seq, len);
        WireTwiddler tt;
        tt.configure(fmt, fmt);
        printf(">>> %s\n", tt.toString().c_str());
        Bottle bot;

        checkTrue(tt.read(bot, b1), "Read failed");
        snprintf(err, 1024, "%s: read %s, expected %s", fmt, bot.toString().c_str(), ref.toString().c_str());
        checkTrue(bot == ref, err);
        printf("[1] %s: read %s as expected\n", fmt, bot.toString().c_str());

        StringInputStream sis;
        sis.add(b1);
        sis.add(b1);
        WireTwiddlerReader twiddled_input(sis, tt);
        Route route;
        bot.clear();
        twiddled_input.reset();
        ConnectionReader::readFromStream(bot, twiddled_input);

        snprintf(err, 1024, "%s: read %s, expected %s", fmt, bot.toString().c_str(), ref.toString().c_str());
        checkTrue(bot == ref, err);
        printf("[2] %s: read %s as expected\n", fmt, bot.toString().c_str());

        bot.clear();
        twiddled_input.reset();
        ConnectionReader::readFromStream(bot, twiddled_input);

        snprintf(err, 1024, "%s: read %s, expected %s", fmt, bot.toString().c_str(), ref.toString().c_str());
        checkTrue(bot == ref, err);
        printf("[3] %s: read %s as expected\n", fmt, bot.toString().c_str());

        if (testWrite) {

            printf("\n");
            printf("================================================\n");
            printf(" WRITE %s\n", fmt);
            ManagedBytes output;
            checkTrue(tt.write(ref, output), "WRITE FAILED");
            snprintf(err, 1024, "WRITE MISMATCH, length %zd, expected %zd", output.length(), len);
            checkTrue(output.length() == len, err);

            for (size_t i = 0; i < output.length(); i++) {
                snprintf(err, 1024, "WRITE MISMATCH, at %zd, have [%d:%c] expected [%d:%c]\n", i, output.get()[i], output.get()[i], seq[i], seq[i]);
                checkTrue(output.get()[i] == seq[i], err);
            }
            printf("[4] %s: wrote %s as expected\n", fmt, bot.toString().c_str());
        }
    }

};


static WireTest theWireTest;

UnitTest& getWireTest() {
    return theWireTest;
}

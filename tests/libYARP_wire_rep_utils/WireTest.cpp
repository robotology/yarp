/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <yarp/wire_rep_utils/WireTwiddler.h>

#include <yarp/os/StringInputStream.h>
#include <yarp/os/Route.h>
#include <yarp/os/InputStream.h>
#include <yarp/os/ConnectionReader.h>

#include <cstdio>
#include <cstdlib>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::os::impl;
using namespace yarp::wire_rep_utils;


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

    CHECK(tt.read(bot, b1)); // Read failed
    std::snprintf(err, 1024, "%s: read %s, expected %s", fmt, bot.toString().c_str(), ref.toString().c_str());
    CHECK(bot == ref);
    INFO(err);
    printf("[1] %s: read %s as expected\n", fmt, bot.toString().c_str());

    StringInputStream sis;
    sis.add(b1);
    sis.add(b1);
    WireTwiddlerReader twiddled_input(sis, tt);
    Route route;
    bot.clear();
    twiddled_input.reset();
    ConnectionReader::readFromStream(bot, twiddled_input);

    std::snprintf(err, 1024, "%s: read %s, expected %s", fmt, bot.toString().c_str(), ref.toString().c_str());
    CHECK(bot == ref);
    INFO(err);
    printf("[2] %s: read %s as expected\n", fmt, bot.toString().c_str());

    bot.clear();
    twiddled_input.reset();
    ConnectionReader::readFromStream(bot, twiddled_input);

    std::snprintf(err, 1024, "%s: read %s, expected %s", fmt, bot.toString().c_str(), ref.toString().c_str());
    CHECK(bot == ref);
    INFO(err);
    printf("[3] %s: read %s as expected\n", fmt, bot.toString().c_str());

    if (testWrite) {

        printf("\n");
        printf("================================================\n");
        printf(" WRITE %s\n", fmt);
        ManagedBytes output;
        CHECK(tt.write(ref, output)); // WRITE FAILED
        std::snprintf(err, 1024, "WRITE MISMATCH, length %zd, expected %zd", output.length(), len);
        CHECK(output.length() == len);
        INFO(err);

        for (size_t i = 0; i < output.length(); i++) {
            std::snprintf(err, 1024, "WRITE MISMATCH, at %zd, have [%d:%c] expected [%d:%c]\n", i, output.get()[i], output.get()[i], seq[i], seq[i]);
            CHECK(output.get()[i] == seq[i]);
            INFO(err);
        }
        printf("[4] %s: wrote %s as expected\n", fmt, bot.toString().c_str());
    }
}


TEST_CASE("wire_rep_utils::WireTest", "[yarp::wire_rep_utils]")
{
    SECTION("check wire")
    {
        NetworkBase::setLocalMode(true);
        {
            INFO("checking vector int32 1 *");
            char seq[] = {42, 0, 0, 0};
            testSequence(seq, sizeof(seq),"vector int32 1 *", Bottle("42"));
        }
        {
            INFO("checking vector string 1 *");
            char seq[] = {5, 0, 0, 0, 'h', 'e', 'l', 'l', 'o'};
            testSequence(seq, sizeof(seq),"vector string 1 *", Bottle("hello"));
        }
        {
            INFO("checking vector int32 *");
            char seq[] = {2, 0, 0, 0, 42, 0, 0, 0, 55, 0, 0, 0};
            testSequence(seq, sizeof(seq),"vector int32 *", Bottle("42 55"));
        }
        {
            INFO("checking vector string *");
            char seq[] = {2, 0, 0, 0, 3, 0, 0, 0, 'f', 'o', 'o', 3, 0, 0, 0, 'b', 'a', 'r'};
            testSequence(seq, sizeof(seq),"vector string *", Bottle("foo bar"));
        }
        {
            INFO("checking list 2 vector string * int32 *");
            char seq[] = {2, 0, 0, 0, 3, 0, 0, 0, 'f', 'o', 'o', 3, 0, 0, 0, 'b', 'a', 'r', 12, 0, 0, 0};
            testSequence(seq, sizeof(seq),"list 2 vector string * int32 *", Bottle("(foo bar) 12"));
        }
        {
            INFO("checking list 3 vector string * int32 * vector int32 *");
            char seq[] = {2, 0, 0, 0, 3, 0, 0, 0, 'f', 'o', 'o', 3, 0, 0, 0, 'b', 'a', 'r', 12, 0, 0, 0, 2, 0, 0, 0, 42, 0, 0, 0, 24, 0, 0, 0};
            testSequence(seq, sizeof(seq),"list 3 vector string * int32 * vector int32 *", Bottle("(foo bar) 12 (42 24)"));
        }
        {
            INFO("checking vector int32 2 *");
            char seq[] = {42, 0, 0, 0, 24, 0, 0, 0};
            testSequence(seq, sizeof(seq),"vector int32 2 *", Bottle("42 24"));
        }
        {
            INFO("checking list 2 string * int32 *");
            char seq[] = {5, 0, 0, 0, 'h', 'e', 'l', 'l', 'o', 12, 0, 0, 0};
            testSequence(seq, sizeof(seq),"list 2 string * int32 *", Bottle("hello 12"));
        }
        {
            INFO("checking list 3 string * int32 * int32 *");
            char seq[] = {5, 0, 0, 0, 'h', 'e', 'l', 'l', 'o', 12, 0, 0, 0, 42, 0, 0, 0};
            testSequence(seq, sizeof(seq),"list 3 string * int32 * int32 *", Bottle("hello 12 42"));
        }
        {
            INFO("checking list 2 string * skip int32 * int32 *");
            char seq[] = {5, 0, 0, 0, 'h', 'e', 'l', 'l', 'o', 12, 0, 0, 0, 42, 0, 0, 0};
            testSequence(seq, sizeof(seq),"list 2 string * skip int32 * int32 *", Bottle("hello 42"), false);
        }
        {
            INFO("checking list 1 skip int32 * int32 *");
            char seq[] = {33, 0, 0, 0, 42, 0, 0, 0};
            testSequence(seq, sizeof(seq),"list 1 skip int32 * int32 *", Bottle("42"), false);
        }
        {
            INFO("checking skip int32 * list 1 int32 *");
            char seq[] = {99, 0, 0, 0, 42, 0, 0, 0};
            testSequence(seq, sizeof(seq),"skip int32 * list 1 int32 *", Bottle("42"), false);
        }
        {
            INFO("checking skip int32 *");
            char seq[] = {99, 0, 0, 0};
            testSequence(seq, sizeof(seq),"skip int32 *", Bottle(), false);
        }
        NetworkBase::setLocalMode(false);

    }

}

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-FileCopyrightText: 2006, 2008 Arjan Gijsberts
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/BottleImpl.h>

#include <yarp/os/ManagedBytes.h>
#include <yarp/os/Vocab.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os::impl;
using namespace yarp::os;

TEST_CASE("os::BottleImplTest", "[yarp::os::impl]")
{
    SECTION("testing binary representation")
    {
        BottleImpl bot;
        bot.addInt32(5);
        bot.addString("hello");
        CHECK(bot.isInt32(0)); // "type check"
        CHECK(bot.isString(1)); // "type check"
        ManagedBytes store(bot.byteCount());
        bot.toBytes(store.bytes());
        BottleImpl bot2;
        bot2.fromBytes(store.bytes());
        CHECK(bot2.size() == (size_t) 2); // "recovery binary, length"
        CHECK(bot2.isInt32(0) == bot.isInt32(0)); //"recovery binary, integer"
        CHECK(bot2.isString(1) == bot.isString(1)); //"recovery binary, integer"
        BottleImpl bot3;
        bot3.fromString("[go] (10 20 30 40)");
        ManagedBytes store2(bot3.byteCount());
        bot3.toBytes(store2.bytes());
        bot.fromBytes(store2.bytes());
        CHECK(bot.get(0).isVocab32()); // "type check"
        CHECK(bot.get(1).isList()); //"type check"

        Bottle bot4("0 1 2.2 3");
        size_t hsize;
        const char *hbuf = bot4.toBinary(&hsize);
        Bottle bot5;
        bot5.fromBinary(hbuf, hsize);
        CHECK(bot5.size() == (size_t) 4); // "player bug"
    }

    SECTION("testing types")
    {
        BottleImpl bot[3];
        bot[0].fromString("5 10.2 \"hello\" -0xF -15.0");
        CHECK(bot[0].get(3).asInt32() == -15); // "hex works"
        bot[1].addInt32(5);
        bot[1].addFloat64(10.2);
        bot[1].addString("hello");
        bot[1].addInt32(-15);
        bot[1].addFloat64(-15.0);
        ManagedBytes store(bot[0].byteCount());
        bot[0].toBytes(store.bytes());
        bot[2].fromBytes(store.bytes());

        for (int i=0; i<3; i++) {
            BottleImpl& b = bot[i];
            INFO("check for bottle number " << i);
            CHECK(b.isInt32(0));
            CHECK(b.get(0).asInt32() == 5);

            CHECK(b.isFloat64(1));
            CHECK(b.get(1).asFloat64() == Approx(10.2));

            CHECK(b.isString(2));
            CHECK(b.get(2).asString() == "hello");

            CHECK(b.isInt32(3));
            CHECK(b.get(3).asInt32() == -15);

            CHECK(b.isFloat64(4));
            CHECK(b.get(4).asFloat64() == Approx(-15.0));
        }
    }

    SECTION("testing lists")
    {
        BottleImpl bot, bot2;
        bot.fromString("(1 2) 4");
        ManagedBytes store(bot.byteCount());
        bot.toBytes(store.bytes());
        bot2.fromBytes(store.bytes());
        CHECK(bot2.size() == (size_t) 2); // "list test 3"
        INFO("bot2 is " << bot2.toString());
    }

    SECTION("testing nesting detection")
    {
        CHECK(!BottleImpl::isComplete("(1 2 3"));
        CHECK(BottleImpl::isComplete("(1 2 3)"));
    }
}

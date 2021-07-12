/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cstdio>

#include <yarp/rosmsg/Demo.h>
#include <yarp/rosmsg/Tennis.h>
#include <yarp/rosmsg/Rpc.h>
#include <yarp/rosmsg/SharedData.h>
#include <yarp/rosmsg/HeaderTest.h>
#include <yarp/rosmsg/HeaderTest2.h>
#include <yarp/os/all.h>

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#if defined(USE_SYSTEM_CATCH)
#include <catch.hpp>
#else
#include "catch.hpp"
#endif


using namespace yarp::os;
using namespace yarp::os::impl;


template <class T>
bool test_lists(T demo, std::string name) {
    INFO("test lists " << name);
    Port p1;
    p1.enableBackgroundWrite(true);
    if (!p1.open("/p1")) return false;
    BufferedPort<T> p2;
    if (!p2.open("/p2")) return false;
    if (!Network::connect(p1.getName(),p2.getName())) {
        INFO("Could not connect");
        return false;
    }
    demo.x = 10;
    demo.a_signed_byte = 0;
    demo.an_unsigned_byte = 0;
    demo.a_signed_byte2 = 0;
    demo.an_unsigned_byte2 = 0;
    demo.a_signed_byte4 = 0;
    demo.an_unsigned_byte4 = 0;
    demo.a_signed_byte8 = 0;
    demo.an_unsigned_byte8 = 0;
    demo.a_bool = false;
    for (int i=0; i<3; i++) {
        demo.fixed_string_list[i] = "";
        demo.fixed_byte_list[i] = 1;
    }
    demo.fixed_string_list[2] = "ping";
    demo.fixed_byte_list[1] = 99;
    p1.write(demo);
    T *in = p2.read();
    REQUIRE(in!=NULL); // read ok
    CHECK(demo.x == in->x); // int ok
    CHECK(demo.fixed_string_list[2] == in->fixed_string_list[2]); // string in list ok
    CHECK(demo.fixed_byte_list[1] == in->fixed_byte_list[1]); // byte in list ok
    p2.close();
    p1.close();
    return true;
}

TEST_CASE("RosMsg", "[yarp::idl::rosmsg]")
{
    Network yarp;
    yarp.setLocalMode(true);

    SECTION("test signs")
    {
        INFO("*** test_signs()");
        yarp::rosmsg::Demo demo;
        demo.an_unsigned_byte = 254;
        CHECK(demo.an_unsigned_byte>=0);
        INFO("Checking an_unsigned_byte is signed");
        demo.a_signed_byte = -1;
        CHECK(demo.a_signed_byte<=0);
        INFO("*** ok!");
    }

    SECTION("test serialization")
    {
        INFO("*** test_serialization()");
        yarp::rosmsg::SharedData data;
        data.text = "hello";
        data.content.push_back(1);
        data.content.push_back(2);
        Bottle bot;
        bot.read(data);
        CHECK(bot.get(0).asString() =="hello");
        INFO("Checking string");
        REQUIRE(bot.get(1).asList()!=nullptr);
        INFO("Checking list is not right");
        CHECK(bot.get(1).asList()->size() ==2);
        INFO("Checking lenght");
        INFO("*** " << bot.toString() << " ok!");
    }

    SECTION("test regular")
    {
        yarp::rosmsg::Demo demo1;
        CHECK(test_lists(demo1,"regular"));
    }

    SECTION("test bottle")
    {
        yarp::rosmsg::Demo::bottleStyle demo2;
        CHECK(test_lists(demo2,"bottle"));
    }

    SECTION("test ros")
    {
        yarp::rosmsg::Demo::rosStyle demo3;
        CHECK(test_lists(demo3,"ros"));
    }

    yarp.setLocalMode(false);
}

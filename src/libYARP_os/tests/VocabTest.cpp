/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string>
#include <yarp/os/Vocab.h>
#include <yarp/os/Vocab64.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::os;

TEST_CASE("os::VocabTest", "[yarp::os]")
{

    SECTION("checking vocabulary conversions 32 bits")
    {
        CHECK(yarp::os::createVocab32('h', 'i') == Vocab32::encode("hi")); //  encoding
        CHECK(Vocab32::decode(Vocab32::encode("hi")) == "hi"); // decoding
        CHECK(yarp::os::createVocab32('h', 'i', 'g', 'h') == Vocab32::encode("high")); // encoding
        CHECK(Vocab32::decode(Vocab32::encode("high")) == "high"); // decoding
        INFO("checking compile-time functions");
        NetInt32 code = Vocab32::encode("stop");
        CHECK_FALSE(code == yarp::os::createVocab32('s', 'e', 't')); // very strange error switching
        CHECK(code == yarp::os::createVocab32('s', 't', 'o', 'p')); // good switch
    }

    SECTION("checking vocabulary conversions 64 bits")
    {
        CHECK(yarp::os::createVocab64('p', 'r', 'o', 'g') == Vocab64::encode("prog"));                  //  encoding
        CHECK(Vocab64::decode(Vocab64::encode("prog")) == "prog"); // decoding
        CHECK(yarp::os::createVocab64('p', 'r', 'o', 'g', 'r', 'a', 'm') == Vocab64::encode("program")); // encoding
        CHECK(Vocab64::decode(Vocab64::encode("program")) == "program"); // decoding
        INFO("checking compile-time functions");
        NetInt64 code = Vocab64::encode("program");
        CHECK_FALSE(code == yarp::os::createVocab64('s', 'e', 't')); // very strange error switching
        CHECK(code == yarp::os::createVocab64('p', 'r', 'o', 'g', 'r', 'a','m')); // good switch
    }
}

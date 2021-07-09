/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <string>
#include <yarp/os/Vocab.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;

TEST_CASE("os::VocabTest", "[yarp::os]")
{

    SECTION("checking vocabulary conversions")
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

}

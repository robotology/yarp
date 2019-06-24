/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/StringInputStream.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;

TEST_CASE("OS::StringInputStreamTest", "[yarp::os]")
{
    SECTION("test reading")
    {
        StringInputStream sis;
        sis.add("Hello my friend");
        char buf[256];
        sis.check();
        Bytes b(buf, sizeof(buf));
        int len = sis.read(b, 0, 5);
        CHECK(len == 5); // len of first read
        buf[len] = '\0';
        CHECK(std::string(buf) == "Hello"); // first read
        char ch = sis.read();
        CHECK(ch == ' '); // the space
        len = sis.read(b, 0, 2);
        CHECK(len == 2); // len of second read
        buf[len] = '\0';
        CHECK(std::string(buf) == "my"); // second read
    }
}

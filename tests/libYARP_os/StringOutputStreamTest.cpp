/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/StringOutputStream.h>

#include <cstring>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;

TEST_CASE("os::StringOutputStreamTest", "[yarp::os]")
{
    SECTION("testing writing")
    {
        StringOutputStream sos;
        char txt[] = "Hello my friend";
        Bytes b(txt, strlen(txt));
        sos.write(b);
        CHECK(sos.toString() == txt); // single write
        StringOutputStream sos2;
        sos2.write('y');
        sos2.write('o');
        CHECK(sos2.toString() == "yo"); // multiple writes
    }
}

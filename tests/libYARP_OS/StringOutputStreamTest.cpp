/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/StringOutputStream.h>

#include <cstring>

#if defined(USE_SYSTEM_CATCH)
#include <catch.hpp>
#else
#include "catch.hpp"
#endif


using namespace yarp::os;

TEST_CASE("OS::StringOutputStreamTest", "[yarp::os]") {

    SECTION("testing writing") {
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

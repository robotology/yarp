/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Election.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;

TEST_CASE("OS::ElectionTest", "[yarp::os]")
{

    SECTION("testing the basics of elections")
    {
        std::string c1 = "Magnifico";
        std::string c2 = "Grasso";
        std::string c3 = "Bozo";
        ElectionOf<PeerRecord<std::string> > elector;
        elector.add("italy", &c1);
        elector.add("italy", &c2);
        elector.add("france", &c3);
        std::string *e1 = elector.getElect("italy");
        std::string *e2 = elector.getElect("france");
        REQUIRE(e1 != nullptr); // elected entity exists (1)
        REQUIRE(e2 != nullptr); // elected entity exists (2)
        CHECK((*e1==c1 || *e1==c2)); // elected entity is accurate (1)
        CHECK(*e2==c3); // elected entity is accurate (2)

        elector.remove("italy", &c2);
        elector.remove("france", &c3);
        e1 = elector.getElect("italy");
        e2 = elector.getElect("france");
        REQUIRE(e1!=nullptr); // elected entity exists (1)
        CHECK(e2==nullptr); // elected entity does not exist (2)");
        CHECK(*e1==c1); // elected entity is accurate (1)");
    }
}

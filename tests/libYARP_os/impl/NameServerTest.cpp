/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

// first include: stuff under test
#include <yarp/os/impl/NameServer.h>

#include <yarp/os/Network.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::os::impl;

int safePort() {
    return Network::getDefaultPortRange()+100;
}

void checkCompanion(bool fake) {
    NetworkBase::setLocalMode(fake);
    Contact address("/foo2", "tcp", "127.0.0.1", safePort());
    NetworkBase::registerContact(address);
    NetworkBase::connect("/junk", "/junk2", nullptr, true);
    NetworkBase::connect("/foo2", "/junk2", nullptr, true);
    for (int i=0; i<5; i++) {
        char buf[100];
        sprintf(buf, "/many/foo/%d", i);
        NetworkBase::connect(buf, "/junk2", nullptr, true);
    }
    NetworkBase::setLocalMode(false);
}

TEST_CASE("os::impl::NameServerTest", "[yarp::os][yarp::os::impl]")
{
    SECTION( "check register")
    {
        Contact address("tcp", "127.0.0.1", safePort());
        NameServer ns;
        ns.registerName("/foo", address);
        Contact a1 = ns.queryName("/foo");
        CHECK(a1.isValid() == true); // recover address
        CHECK(a1.getHost() == "127.0.0.1"); // machine name matches
        Contact a2 = ns.queryName("/bar");
        CHECK(a2.isValid() == false); // non-existent address
    }

    SECTION("checkClientInterface")
    {
        NetworkBase::setLocalMode(true);
        Contact address("/foo2", "tcp", "127.0.0.1", safePort());
        NetworkBase::registerContact(address);
        Contact a1 = NetworkBase::queryName("/foo2");
        CHECK(a1.isValid() == true); // recover address
        CHECK(a1.getHost() == "127.0.0.1"); // machine name matches
        Contact a2 = NetworkBase::queryName("/bar2");
        CHECK(a2.isValid() == false); // non-existent address
        NetworkBase::setLocalMode(false);
    }

    SECTION("checkCompanion")
    {
        checkCompanion(true);
        // checkCompanion(false);
    }
}

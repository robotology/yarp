/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Route.h>
#include <yarp/os/Contact.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;

TEST_CASE("os::RouteTest", "[yarp::os]")
{
    SECTION("checking constructor with no parameters")
    {
        Route r;
        CHECK(r.getFromName() == ""); // empty from name
        CHECK(r.getToName() == ""); // empty to name
        CHECK(r.getCarrierName() == ""); // empty carrier name
        CHECK_FALSE(r.getToContact().isValid()); // invalid to contact
    }

    SECTION("checking constructor")
    {
        Route r("/foo", "/bar", "baz");
        CHECK(r.getFromName() == "/foo"); // from name is set
        CHECK(r.getToName() == "/bar"); // to name is set
        CHECK(r.getCarrierName() == "baz"); // carrier name is set
        CHECK_FALSE(r.getToContact().isValid()); // invalid to contact
    }

    SECTION("checking copy assignment operator with empty Route")
    {
        Route r1;
        Route r2;
        r1 = r2;
        CHECK(r1.getFromName() == ""); // empty from name
        CHECK(r1.getToName() == ""); // empty to name
        CHECK(r1.getCarrierName() == ""); // empty carrier name
        CHECK_FALSE(r1.getToContact().isValid()); // invalid to contact
    }

    SECTION("checking copy assignment operator with non empty Route")
    {
        Route r1;
        Route r2("/foo", "/bar", "baz");
        r1 = r2;
        CHECK(r1.getFromName() == "/foo"); // from name is set
        CHECK(r1.getToName() == "/bar"); // to name is set
        CHECK(r1.getCarrierName() == "baz"); // carrier name is set
        CHECK_FALSE(r1.getToContact().isValid()); // invalid to contact
    }

    SECTION("checking copy constructor")
    {
        Route r1("/foo", "/bar", "baz");
        Route r2(r1);
        CHECK(r2.getFromName() == "/foo"); // from name is set
        CHECK(r2.getToName() == "/bar"); // to name is set
        CHECK(r2.getCarrierName() == "baz"); // carrier name is set
        CHECK_FALSE(r2.getToContact().isValid()); // invalid to contact
    }

    SECTION("checking move constructor")
    {
        Route r1("/foo", "/bar", "baz");
        Route r2 = std::move(r1);
        CHECK(r2.getFromName() == "/foo"); // from name is set
        CHECK(r2.getToName() == "/bar"); // to name is set
        CHECK(r2.getCarrierName() == "baz"); // carrier name is set
        CHECK_FALSE(r2.getToContact().isValid()); // invalid to contact
    }

    SECTION("checking move assignment operator")
    {
        Route r;
        r = Route("/foo", "/bar", "baz");
        CHECK(r.getFromName() == "/foo"); // from name is set
        CHECK(r.getToName() == "/bar"); // to name is set
        CHECK(r.getCarrierName() == "baz"); // carrier name is set
        CHECK_FALSE(r.getToContact().isValid()); // invalid to contact
    }

    SECTION("checking setFromName")
    {
        Route r("/foo", "/bar", "baz");
        CHECK(r.getFromName() == "/foo"); // from name is set
        CHECK(r.getToName() == "/bar"); // to name is set
        CHECK(r.getCarrierName() == "baz"); // carrier name is set
        CHECK_FALSE(r.getToContact().isValid()); // invalid to contact
        r.setFromName("/zap");
        CHECK(r.getFromName() == "/zap"); // from name is updated
        CHECK(r.getToName() == "/bar"); // to name is not updated
        CHECK(r.getCarrierName() == "baz"); // carrier name is not updated
        CHECK_FALSE(r.getToContact().isValid()); // invalid to contact
    }

    SECTION("checking setToName")
    {
        Route r("/foo", "/bar", "baz");
        CHECK(r.getFromName() == "/foo"); // from name is set
        CHECK(r.getToName() == "/bar"); // from name is set
        CHECK(r.getCarrierName() == "baz"); // carrier name is set
        CHECK_FALSE(r.getToContact().isValid()); // invalid to contact
        r.setToName("/zap");
        CHECK(r.getFromName() == "/foo"); // from name is not updated
        CHECK(r.getToName() == "/zap"); // to name is updated
        CHECK(r.getCarrierName() == "baz"); // carrier name is not updated
        CHECK_FALSE(r.getToContact().isValid()); // invalid to contact
    }

    SECTION("checking setCarrierName")
    {
        Route r("/foo", "/bar", "baz");
        CHECK(r.getFromName() == "/foo"); // from name is set
        CHECK(r.getToName() == "/bar"); // from name is set
        CHECK(r.getCarrierName() == "baz"); // carrier name is set
        CHECK_FALSE(r.getToContact().isValid()); // invalid to contact
        r.setCarrierName("zap");
        CHECK(r.getFromName() == "/foo"); // from name is not updated
        CHECK(r.getToName() == "/bar"); // to name is not updated
        CHECK(r.getCarrierName() == "zap"); // carrier name is updated
        CHECK_FALSE(r.getToContact().isValid()); // invalid to contact
    }

    SECTION("checking setToContact()")
    {
        Route r("/foo", "/bar", "baz");
        CHECK(r.getFromName() == "/foo"); // from name is set
        CHECK(r.getToName() == "/bar"); // from name is set
        CHECK(r.getCarrierName() == "baz"); // carrier name is set
        CHECK_FALSE(r.getToContact().isValid()); // invalid to contact
        r.setToContact(Contact("tcp", "127.0.0.1", 10000));
        CHECK(r.getFromName() == "/foo"); // from name is not updated
        CHECK(r.getToName() == "/bar"); // to name is not updated
        CHECK(r.getCarrierName() == "baz"); // carrier name is not updated
        CHECK(r.getToContact().toURI() == "tcp://127.0.0.1:10000/"); // to contact is updated
    }

    SECTION("checking swapNames")
    {
        Route r("/foo", "/bar", "baz");
        CHECK(r.getFromName() == "/foo"); // from name is set
        CHECK(r.getToName() == "/bar"); // to name is set
        CHECK(r.getCarrierName() == "baz"); // carrier name is set
        CHECK_FALSE(r.getToContact().isValid()); // invalid to contact
        r.swapNames();
        CHECK(r.getFromName() == "/bar"); // from name is swapped
        CHECK(r.getToName() == "/foo"); // to name is swapped
        CHECK(r.getCarrierName() == "baz"); // carrier name is not updated
        CHECK_FALSE(r.getToContact().isValid()); // invalid to contact
    }

    SECTION("checking toString no parameters")
    {
        Route r;
        CHECK(r.toString() == "->->"); // toString() is correct
    }

    SECTION("checking constructor")
    {
        Route r("/foo", "/bar", "baz");
        CHECK(r.toString() == "/foo->baz->/bar"); // toString() is correct
    }
}

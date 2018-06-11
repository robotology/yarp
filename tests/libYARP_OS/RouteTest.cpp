/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/impl/UnitTest.h>
#include <yarp/os/Route.h>
#include <yarp/os/Contact.h>

using namespace yarp::os::impl;
using namespace yarp::os;

class RouteTest : public UnitTest
{
public:
    virtual std::string getName() const override
    {
        return "RouteTest";
    }

    void testRoute()
    {
        {
            report(0, "checking constructor with no parameters");
            Route r;
            checkEqual(r.getFromName().c_str(), "", "empty from name");
            checkEqual(r.getToName().c_str(), "", "empty to name");
            checkEqual(r.getCarrierName().c_str(), "", "empty carrier name");
            checkFalse(r.getToContact().isValid(), "invalid to contact");
        }

        {
            report(0, "checking constructor");
            Route r("/foo", "/bar", "baz");
            checkEqual(r.getFromName().c_str(), "/foo", "from name is set");
            checkEqual(r.getToName().c_str(), "/bar", "to name is set");
            checkEqual(r.getCarrierName().c_str(), "baz", "carrier name is set");
            checkFalse(r.getToContact().isValid(), "invalid to contact");
        }

        {
            report(0, "checking copy assignment operator with empty Route");
            Route r1;
            Route r2;
            r1 = r2;
            checkEqual(r1.getFromName().c_str(), "", "empty from name");
            checkEqual(r1.getToName().c_str(), "", "empty to name");
            checkEqual(r1.getCarrierName().c_str(), "", "empty carrier name");
            checkFalse(r1.getToContact().isValid(), "invalid to contact");
        }

        {
            report(0, "checking copy assignment operator with non empty Route");
            Route r1;
            Route r2("/foo", "/bar", "baz");
            r1 = r2;
            checkEqual(r1.getFromName().c_str(), "/foo", "from name is set");
            checkEqual(r1.getToName().c_str(), "/bar", "to name is set");
            checkEqual(r1.getCarrierName().c_str(), "baz", "carrier name is set");
            checkFalse(r1.getToContact().isValid(), "invalid to contact");
        }

        {
            report(0, "checking copy constructor");
            Route r1("/foo", "/bar", "baz");
            Route r2(r1);
            checkEqual(r2.getFromName().c_str(), "/foo", "from name is set");
            checkEqual(r2.getToName().c_str(), "/bar", "to name is set");
            checkEqual(r2.getCarrierName().c_str(), "baz", "carrier name is set");
            checkFalse(r2.getToContact().isValid(), "invalid to contact");
        }

        {
            report(0, "checking move constructor");
            Route r1("/foo", "/bar", "baz");
            Route r2 = std::move(r1);
            checkEqual(r2.getFromName().c_str(), "/foo", "from name is set");
            checkEqual(r2.getToName().c_str(), "/bar", "to name is set");
            checkEqual(r2.getCarrierName().c_str(), "baz", "carrier name is set");
            checkFalse(r2.getToContact().isValid(), "invalid to contact");
        }

        {
            report(0, "checking move assignment operator");
            Route r;
            r = Route("/foo", "/bar", "baz");
            checkEqual(r.getFromName().c_str(), "/foo", "from name is set");
            checkEqual(r.getToName().c_str(), "/bar", "to name is set");
            checkEqual(r.getCarrierName().c_str(), "baz", "carrier name is set");
            checkFalse(r.getToContact().isValid(), "invalid to contact");
        }
    }

    void testSetter()
    {
        {
            report(0, "checking setFromName");
            Route r("/foo", "/bar", "baz");
            checkEqual(r.getFromName().c_str(), "/foo", "from name is set");
            checkEqual(r.getToName().c_str(), "/bar", "to name is set");
            checkEqual(r.getCarrierName().c_str(), "baz", "carrier name is set");
            checkFalse(r.getToContact().isValid(), "invalid to contact");
            r.setFromName("/zap");
            checkEqual(r.getFromName().c_str(), "/zap", "from name is updated");
            checkEqual(r.getToName().c_str(), "/bar", "to name is not updated");
            checkEqual(r.getCarrierName().c_str(), "baz", "carrier name is not updated");
            checkFalse(r.getToContact().isValid(), "invalid to contact");
        }

        {
            report(0, "checking setToName");
            Route r("/foo", "/bar", "baz");
            checkEqual(r.getFromName().c_str(), "/foo", "from name is set");
            checkEqual(r.getToName().c_str(), "/bar", "from name is set");
            checkEqual(r.getCarrierName().c_str(), "baz", "carrier name is set");
            checkFalse(r.getToContact().isValid(), "invalid to contact");
            r.setToName("/zap");
            checkEqual(r.getFromName().c_str(), "/foo", "from name is not updated");
            checkEqual(r.getToName().c_str(), "/zap", "to name is updated");
            checkEqual(r.getCarrierName().c_str(), "baz", "carrier name is not updated");
            checkFalse(r.getToContact().isValid(), "invalid to contact");
        }

        {
            report(0, "checking setCarrierName");
            Route r("/foo", "/bar", "baz");
            checkEqual(r.getFromName().c_str(), "/foo", "from name is set");
            checkEqual(r.getToName().c_str(), "/bar", "from name is set");
            checkEqual(r.getCarrierName().c_str(), "baz", "carrier name is set");
            checkFalse(r.getToContact().isValid(), "invalid to contact");
            r.setCarrierName("zap");
            checkEqual(r.getFromName().c_str(), "/foo", "from name is not updated");
            checkEqual(r.getToName().c_str(), "/bar", "to name is not updated");
            checkEqual(r.getCarrierName().c_str(), "zap", "carrier name is updated");
            checkFalse(r.getToContact().isValid(), "invalid to contact");
        }

        {
            report(0, "checking setToContact()");
            Route r("/foo", "/bar", "baz");
            checkEqual(r.getFromName().c_str(), "/foo", "from name is set");
            checkEqual(r.getToName().c_str(), "/bar", "from name is set");
            checkEqual(r.getCarrierName().c_str(), "baz", "carrier name is set");
            checkFalse(r.getToContact().isValid(), "invalid to contact");
            r.setToContact(Contact("tcp", "127.0.0.1", 10000));
            checkEqual(r.getFromName().c_str(), "/foo", "from name is not updated");
            checkEqual(r.getToName().c_str(), "/bar", "to name is not updated");
            checkEqual(r.getCarrierName().c_str(), "baz", "carrier name is not updated");
            checkEqual(r.getToContact().toURI().c_str(), "tcp://127.0.0.1:10000/", "to contact is updated");
        }

    }

    void testSwapNames()
    {
        {
            report(0, "checking swapNames");
            Route r("/foo", "/bar", "baz");
            checkEqual(r.getFromName().c_str(), "/foo", "from name is set");
            checkEqual(r.getToName().c_str(), "/bar", "to name is set");
            checkEqual(r.getCarrierName().c_str(), "baz", "carrier name is set");
            checkFalse(r.getToContact().isValid(), "invalid to contact");
            r.swapNames();
            checkEqual(r.getFromName().c_str(), "/bar", "from name is swapped");
            checkEqual(r.getToName().c_str(), "/foo", "to name is swapped");
            checkEqual(r.getCarrierName().c_str(), "baz", "carrier name is not updated");
            checkFalse(r.getToContact().isValid(), "invalid to contact");
        }
    }

    void testToString()
    {
        {
            report(0, "checking toString no parameters");
            Route r;
            checkEqual(r.toString().c_str(), "->->", "toString() is correct");
        }

        {
            report(0, "checking constructor");
            Route r("/foo", "/bar", "baz");
            checkEqual(r.toString().c_str(), "/foo->baz->/bar", "toString() is correct");
        }

    }

    virtual void runTests() override {
        testRoute();
        testSetter();
        testSwapNames();
        testToString();
    }
};

static RouteTest theRouteTest;

UnitTest& getRouteTest() {
    return theRouteTest;
}


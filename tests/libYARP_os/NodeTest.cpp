/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Node.h>
#include <yarp/os/Nodes.h>
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Time.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/impl/NameClient.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::os::impl;

template <class T>
class Publisher : public BufferedPort<T> {
public:
    Publisher() {
        BufferedPort<T>::setWriteOnly();
    }
};

template <class T>
class Subscriber : public BufferedPort<T> {
public:
    Subscriber() {
        BufferedPort<T>::setReadOnly();
    }
};

static void parseName(const std::string& arg,
                      const std::string& node,
                      const std::string& nested,
                      const std::string& cat) {
    NestedContact nc(arg);
    CHECK(nc.getNodeName() == node);
    CHECK(nc.getNestedName() == nested);
    CHECK(nc.getCategory() == cat);
}

static bool waitConnect(const std::string& n1,
                        const std::string& n2,
                        double timeout) {
    double start = Time::now();
    while (Time::now()-start<timeout) {
        if (NetworkBase::isConnected(n1, n2)) {
            return true;
        }
        Time::delay(0.1);
    }
    return false;
}

TEST_CASE("os::NodeTest", "[yarp::os]")
{
    NetworkBase::setLocalMode(true);

    SECTION("check support for various experimental node/topic/service schemes")
    {
        parseName("/foo", "/foo", "", "");
        parseName("/topic@/foo", "/foo", "/topic", "");
        parseName("/topic-@/foo", "/foo", "/topic", "-");
        parseName("/topic-1@/foo", "/foo", "/topic", "-1");
        parseName("/foo=/topic", "/foo", "/topic", "");
        parseName("/foo=+/topic", "/foo", "/topic", "+");
        parseName("/foo=+1/topic", "/foo", "/topic", "+1");
        parseName("/foo-#/topic", "/foo", "/topic", "-");
        parseName("/foo+#/topic", "/foo", "/topic", "+");
    }

    SECTION("most basic node test")
    {
        NameClient::getNameClient().getNodes().enable(false);
        Node n;
        Port p1;
        Port p2;
        p1.open("/test=-/p1");
        p2.open("/test=+/p2");
        n.add(p1);
        n.add(p2);
        Contact c = n.query("/p1");
        CHECK(c.isValid()); // found /p1
        c = n.query("/p9");
        CHECK_FALSE(c.isValid()); // failed to find non-existent /p9
        n.remove(p2);
        n.remove(p1);
        NameClient::getNameClient().getNodes().enable(true);
    }

    SECTION("most basic nodes test")
    {
        NameClient::getNameClient().getNodes().enable(false);
        Nodes n;
        Port p1;
        Port p2;
        Port p3;
        p1.open("/test=-/p1");
        p2.open("/test=-/p2");
        p3.open("/test2=+/p3");
        n.add(p1);
        n.add(p2);
        n.add(p3);
        Contact c = n.query("/p1");
        CHECK(c.isValid()); // found /p1
        c = n.query("/p9");
        CHECK_FALSE(c.isValid()); // failed to find non-existent /p9
        c = n.query("/p3");
        CHECK(c.isValid()); // found /p3

        Network::connect(n.query("/p1").getName(), n.query("/p3").getName());
        Network::sync(n.query("/p1").getName());
        Network::sync(n.query("/p3").getName());
        CHECK(Network::isConnected("/test=-/p1", "/test2=+/p3")); // good contacts

        n.remove(p3);
        n.remove(p2);
        n.remove(p1);
        NameClient::getNameClient().getNodes().enable(true);
    }

    SECTION("ypes coevolving with nodes")
    {
        Bottle b;
        Type typ = b.getType();
        CHECK_FALSE(typ.hasName()); // Bottle type is not named yet
    }

    SECTION("check that auto node works")
    {
        NameClient::getNameClient().getNodes().clear();
        {
            CHECK_FALSE(NetworkBase::exists("/test")); // node does not exist yet
            Port p1;
            Port p2;
            p1.open("/test=-/p1");
            p2.open("/test=+/p2");
            CHECK(NetworkBase::exists("/test")); // node exists now
        }
        NameClient::getNameClient().getNodes().clear();
    }

    SECTION("check node api")
    {
        NameClient::getNameClient().getNodes().clear();
        Port p1;
        Port p2;
        p1.open("/test=-/p1");
        p2.open("/test=+/p2");
        Bottle cmd, reply;
        cmd.fromString("getSubscriptions dummy");
        NetworkBase::write(Contact("/test"), cmd, reply);
        CHECK(reply.get(0).asInt32() == 1); // getSubscriptions api success
        cmd.fromString("requestTopic dummy /p2 (TCPROS)");
        NetworkBase::write(Contact("/test"), cmd, reply);
        CHECK(reply.get(0).asInt32() == 1); // found /p2
        cmd.fromString("requestTopic dummy /p3 (TCPROS)");
        NetworkBase::write(Contact("/test"), cmd, reply);
        CHECK(reply.get(0).asInt32() == 0); // failed to find /p3
        p1.close();
        p2.close();
        NameClient::getNameClient().getNodes().clear();
    }

    SECTION("check port node combo")
    {
        NameClient::getNameClient().getNodes().clear();
        Port p1;
        Port p2;
        // Node names were identical, but changed for ROS compatibility
        p1.open("/test1=+/p1");
        p2.open("/test2=-/p1");
        CHECK(waitConnect(p1.getName(), p2.getName(), 20)); // auto connect working
        NameClient::getNameClient().getNodes().clear();
    }

    SECTION("direction test")
    {
        NameClient::getNameClient().getNodes().clear();
        Port p1;
        Port p2;
        p1.setWriteOnly();
        p2.setReadOnly();
        p1.open("/node1=/a/topic");
        p2.open("/node2=/a/topic");
        CHECK(waitConnect(p1.getName(), p2.getName(), 20)); // direction working
        Port p3, p4;
        p3.setWriteOnly();
        p4.setReadOnly();
        p3.open("/node3=/a/topic");
        p4.open("/node4=/a/topic");
        CHECK(waitConnect(p1.getName(), p4.getName(), 20)); // p1->p4 ok
        CHECK(waitConnect(p3.getName(), p4.getName(), 20)); // p3->p4 ok
        CHECK(waitConnect(p3.getName(), p2.getName(), 20)); // p3->p2 ok
        NameClient::getNameClient().getNodes().clear();
    }

    SECTION("single name test")
    {
        Node n("/ntest");
        Port p1;
        p1.setWriteOnly();
        p1.open("p1");
        CHECK(p1.getName() == "/p1+@/ntest"); // node name goes viral
        Publisher<Bottle> p2;
        p2.open("p2");
        CHECK(p2.getName() == "/p2+@/ntest"); // publisher name ok
        Subscriber<Bottle> p3;
        p3.open("p3");
        CHECK(p3.getName() == "/p3-@/ntest"); // subscriber name ok
        Port p4;
        p4.open("/p4");
        CHECK(p4.getName() == "/p4"); // rooted name ok
        Bottle cmd, reply;
        cmd.fromString("requestTopic dummy /p1 (TCPROS)");
        NetworkBase::write(Contact("/ntest"), cmd, reply);
        CHECK(reply.get(0).asInt32() == 1); // found /p1
    }

    SECTION("type property test")
    {
        Type t;
        CHECK_FALSE(t.readProperties().check("test")); // property absent
        t.addProperty("test", Value("foo"));
        CHECK(t.readProperties().check("test")); // property present

        Type t1(t);
        CHECK(t1.readProperties().check("test")); // property present in copy
        t = t1;
        CHECK(t.readProperties().check("test")); // property present in double copy
    }

#if defined(ENABLE_BROKEN_TESTS)
    SECTION("trying to open a port without '/' after Node creation")
    {
        INFO("trying to open a port without '/' after Node creation.");
        Node n("/mynode");
        BufferedPort<Bottle> p;
        CHECK_FALSE(p.open("nameWithoutSlash+")); // open port with wrong name should fail
    }
#endif // ENABLE_BROKEN_TESTS

    NetworkBase::setLocalMode(false);
}

// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/Node.h>
#include <yarp/os/Nodes.h>
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Time.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/impl/UnitTest.h>
#include <yarp/os/impl/NameClient.h>

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

class NodeTest : public UnitTest {
public:
    virtual String getName() { return "NodeTest"; }

    void basicNodeTest();
    void basicNodesTest();
    void basicTypeTest();
    void builtinNodeTest();
    void basicApiTest();
    void portTopicCombo();
    void directionTest();
    void singleNameTest();

    virtual void runTests();
};

void NodeTest::basicNodeTest() {
    report(0,"most basic node test");
    NameClient::getNameClient().getNodes().enable(false);
    Node n;
    Port p1;
    Port p2;
    p1.open("/test=-/p1");
    p2.open("/test=+/p2");
    n.add(p1);
    n.add(p2);
    Contact c = n.query("/p1");
    checkTrue(c.isValid(),"found /p1");
    c = n.query("/p9");
    checkFalse(c.isValid(),"failed to find non-existent /p9");
    n.remove(p2);
    n.remove(p1);
    NameClient::getNameClient().getNodes().enable(true);
}

void NodeTest::basicNodesTest() {
    report(0,"most basic nodes test");
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
    checkTrue(c.isValid(),"found /p1");
    c = n.query("/p9");
    checkFalse(c.isValid(),"failed to find non-existent /p9");
    c = n.query("/p3");
    checkTrue(c.isValid(),"found /p3");

    Network::connect(n.query("/p1").getName(),n.query("/p3").getName());
    Network::sync(n.query("/p1").getName());
    Network::sync(n.query("/p3").getName());
    checkTrue(Network::isConnected("/test=-/p1","/test2=+/p3"),"good contacts");

    n.remove(p3);
    n.remove(p2);
    n.remove(p1);
    NameClient::getNameClient().getNodes().enable(true);
}

void NodeTest::basicTypeTest() {
    report(0,"types coevolving with nodes");
    Bottle b;
    Type typ = b.getType();
    checkFalse(typ.hasName(),"Bottle type is not named yet");
}

void NodeTest::builtinNodeTest() {
    report(0,"check that auto node works");
    NameClient::getNameClient().getNodes().clear();
    {
        checkFalse(NetworkBase::exists("/test"),"node does not exist yet");
        Port p1;
        Port p2;
        p1.open("/test=-/p1");
        p2.open("/test=+/p2");
        checkTrue(NetworkBase::exists("/test"),"node exists now");
    }
    NameClient::getNameClient().getNodes().clear();
}

void NodeTest::basicApiTest() {
    report(0,"check node api");
    NameClient::getNameClient().getNodes().clear();
    Port p1;
    Port p2;
    p1.open("/test=-/p1");
    p2.open("/test=+/p2");
    Bottle cmd, reply;
    cmd.fromString("getSubscriptions dummy");
    NetworkBase::write(Contact::byName("/test"),cmd,reply);
    checkEqual(reply.get(0).asInt(),1,"getSubscriptions api success");
    cmd.fromString("requestTopic dummy /p2 (TCPROS)");
    NetworkBase::write(Contact::byName("/test"),cmd,reply);
    checkEqual(reply.get(0).asInt(),1,"found /p2");
    cmd.fromString("requestTopic dummy /p3 (TCPROS)");
    NetworkBase::write(Contact::byName("/test"),cmd,reply);
    checkEqual(reply.get(0).asInt(),0,"failed to find /p3");
    p1.close();
    p2.close();
    NameClient::getNameClient().getNodes().clear();
}

static bool waitConnect(const ConstString& n1,
                        const ConstString& n2,
                        double timeout) {
    double start = Time::now();
    while (Time::now()-start<timeout) {
        if (NetworkBase::isConnected(n1,n2)) {
            return true;
        }
        Time::delay(0.1);
    }
    return false;
}

void NodeTest::portTopicCombo() {
    report(0,"check port node combo");
    NameClient::getNameClient().getNodes().clear();
    Port p1;
    Port p2;
    p1.open("/test=+/p1");
    p2.open("/test=-/p1");
    checkTrue(waitConnect(p1.getName(),p2.getName(),20), 
              "auto connect working");
    NameClient::getNameClient().getNodes().clear();
}

void NodeTest::directionTest() {
    report(0,"direction test");
    NameClient::getNameClient().getNodes().clear();
    Port p1;
    Port p2;
    p1.setWriteOnly();
    p2.setReadOnly();
    p1.open("/node1=/a/topic");
    p2.open("/node2=/a/topic");
    checkTrue(waitConnect(p1.getName(),p2.getName(),20), 
              "direction working");
    Port p3, p4;
    p3.setWriteOnly();
    p4.setReadOnly();
    p3.open("/node3=/a/topic");
    p4.open("/node4=/a/topic");
    checkTrue(waitConnect(p1.getName(),p4.getName(),20), 
              "p1->p4 ok");
    checkTrue(waitConnect(p3.getName(),p4.getName(),20), 
              "p3->p4 ok");
    checkTrue(waitConnect(p3.getName(),p2.getName(),20), 
              "p3->p2 ok");
    NameClient::getNameClient().getNodes().clear();    
}

void NodeTest::singleNameTest() {
    report(0,"single name test");
    Node n("/ntest");
    Port p1;
    p1.setWriteOnly();
    p1.open("p1");
    checkEqual(p1.getName(),"/p1+@/ntest","node name goes viral");
    Publisher<Bottle> p2;
    p2.open("p2");
    checkEqual(p2.getName(),"/p2+@/ntest","publisher name ok");
    Subscriber<Bottle> p3;
    p3.open("p3");
    checkEqual(p3.getName(),"/p3-@/ntest","subscriber name ok");
    Port p4;
    p4.open("/p4");
    checkEqual(p4.getName(),"/p4","rooted name ok");
    Bottle cmd, reply;
    cmd.fromString("requestTopic dummy /p1 (TCPROS)");
    NetworkBase::write(Contact::byName("/ntest"),cmd,reply);
    checkEqual(reply.get(0).asInt(),1,"found /p1");
}

void NodeTest::runTests() {
    NetworkBase::setLocalMode(true);
    basicNodeTest();
    basicNodesTest();
    basicTypeTest();
    builtinNodeTest();
    basicApiTest();
    portTopicCombo();
    directionTest();
    singleNameTest();
    NetworkBase::setLocalMode(false);
}

static NodeTest theNodeTest;

UnitTest& getNodeTest() {
    return theNodeTest;
}


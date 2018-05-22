/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Semaphore.h>
#include <string>
#include <yarp/os/Time.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/QosStyle.h>

#include <yarp/os/impl/UnitTest.h>
#include <yarp/os/impl/TcpFace.h>
//#include "TestList.h"

using namespace yarp::os::impl;
using namespace yarp::os;

class NetworkTestWorker1 : public Thread {
public:
    Semaphore fini;
    std::string name;
    Port p;

    NetworkTestWorker1() : fini(0) {
    }

    void run() override {
        Time::delay(0.5);
        p.open(name.c_str());
        fini.wait();
    }
};


class NetworkServiceProvider : public PortReader {
public:

    virtual bool read(ConnectionReader& connection) override {
        Bottle receive;
        //printf("service provider reading data\n");
        receive.read(connection);
        //printf("service provider read data\n");
        receive.addInt32(5);
        ConnectionWriter *writer = connection.getWriter();
        if (writer!=nullptr) {
            //printf("service provider replying\n");
            receive.write(*writer);
            //printf("service provider replied\n");
        }
        return true;
    }
};

class SlowResponder : public Thread {
public:
    Port p;

    SlowResponder(const char *name) {
        p.open(name);
    }

    virtual void run() override {
        Bottle b1,b2;
        while (!isStopping()) {
            p.read(b1,true);
            //printf("READ %s\n", b1.toString().c_str());
            for (int i=0; i<10 && !isStopping(); i++) {
                Time::delay(1);
            }
            b2.addString("you should not be seeing this");
            p.reply(b2);
        }
    }

    virtual void onStop() override {
        p.interrupt();
        p.close();
    }
};

class NetworkTest : public UnitTest {
public:
    virtual std::string getName() override { return "NetworkTest"; }

    void checkConnect() {
        report(0,"checking return value of connect method");
        Port p1;
        Port p2;
        bool ok1 = p1.open("/p1");
        bool ok2 = p2.open("/p2");
        checkTrue(ok1&&ok2,"ports opened ok");
        if (!(ok1&&ok2)) {
            return;
        }
        Network::sync("/p1");
        Network::sync("/p2");
        checkTrue(Network::connect("/p1","/p2"),"good connect");
        checkFalse(Network::connect("/p1","/p3"),"bad connect, not existing destination");
        checkFalse(Network::connect("/p1","/p2 /p3"),"bad connect, invalid destination");
        checkFalse(Network::connect("/p1 /p2","/p2"),"bad connect, invalid source");
        checkFalse(Network::connect("/p1/", "/p2"),"bad connect, source with ending '/'");
        checkFalse(Network::connect("/p1", "/p2/"),"bad connect, destination with ending '/'");
        checkFalse(Network::connect("p1", "/p2"),"bad connect, source without starting '/'");
        checkFalse(Network::connect("/p1", "p2"),"bad connect, destination without starting '/'");
        p2.close();
        p1.close();
    }


    void checkSync() {
        report(0,"checking port synchronization");
        Port p1;
        Port p2;
        p1.open("/p1");
        NetworkTestWorker1 worker;
        worker.name = "/p2";
        worker.start();
        Network::sync("/p2");
        checkTrue(Network::connect("/p1","/p2"),"good connect");
        worker.fini.post();
        worker.stop();
        //p2.close();
        p1.close();
    }


    void checkComms() {
        report(0,"checking basic communications");
        Port server;
        NetworkServiceProvider provider;
        server.setReader(provider);
        server.open("/server");

        Network::sync("/server");
        Bottle cmd("10"), reply;
        Network::write("/server",cmd,reply);
        checkEqual(reply.size(),2,"got append");
        checkEqual(reply.get(1).asInt32(),5,"append is correct");
        server.close();
    }


    void checkPropertySetGet() {
        report(0,"checking property storage on name server");
        Network::registerName("/foo");
        Network::setProperty("/foo","my_prop",Value(15));
        Value *v = Network::getProperty("/foo","my_prop");
        checkTrue(v!=nullptr,"got property");
        if (v!=nullptr) {
            checkEqual(v->asInt32(),15,"recover property");
            delete v;
        }
        Network::unregisterName("/foo");
    }

    void checkTimeoutNetworkWrite() {
        report(0,"checking Network::write timeout");
        SlowResponder sr("/slow");
        sr.start();
        Bottle cmd("hello"), reply;
        ContactStyle style;
        style.timeout = 2.0;
        double start = Time::now();
        Network::write(Contact("/slow"), cmd, reply, style);
        double duration = Time::now()-start;
        bool goodTime = duration<9;
        checkEqual(reply.get(0).asString().c_str(),"","check timeout happened");
        checkTrue(goodTime,"timing looks ok");
        sr.stop();
    }

    void checkTimeoutNetworkExists() {
        ContactStyle style;
        style.timeout = 2.0;
        report(0,"checking Network::exists timeout");
        Port p;
        p.open("/tcp");
        Contact c = p.where();
        bool ok = Network::exists("/tcp",style);
        checkTrue(ok,"a yarp port");
        p.close();
        TcpFace face;
        Contact address(c.getHost(),c.getPort());
        checkTrue(face.open(address),"open server socket, timeout check proceeds");
        Network::registerContact(c);
        ok = Network::exists("/tcp",style);
        Network::unregisterContact(c);
        checkFalse(ok,"not a yarp port");
        face.close();
    }

    static bool waitConnect(const std::string& n1,
                            const std::string& n2,
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

    void checkTopics() {
        report(0,"checking topics are effective");
        Network::connect("/NetworkTest/checkTopic/p1","topic://NetworkTest/checkTopic");
        Network::connect("topic://NetworkTest/checkTopic","/NetworkTest/checkTopic/p2");
        Port p1;
        Port p2;
        p1.open("/NetworkTest/checkTopic/p1");
        p2.open("/NetworkTest/checkTopic/p2");
        checkTrue(waitConnect(p1.getName(),p2.getName(),20), 
                  "auto connect working");
        Network::disconnect("/NetworkTest/checkTopic/p1","topic://NetworkTest/checkTopic");
        Network::disconnect("topic://NetworkTest/checkTopic","/NetworkTest/checkTopic/p2");
    }

    void checkPersistence() {
        report(0,"checking non-topic persistence is effective");
        ContactStyle style;
        style.persistent = true;
        Network::connect("/NetworkTest/checkPersistence/p1","/NetworkTest/checkPersistence/p2",style);
        Port p1;
        Port p2;
        p1.open("/NetworkTest/checkPersistence/p1");
        p2.open("/NetworkTest/checkPersistence/p2");
        checkTrue(waitConnect(p1.getName(),p2.getName(),20), 
                  "auto connect working");
        Network::disconnect("/NetworkTest/checkPersistence/p1","NetworkTest/checkPersistence/p2",style);
    }

    void checkConnectionQos() {
        report(0,"checking connection qos");
        BufferedPort<Bottle> p1;
        BufferedPort<Bottle> p2;
        p1.open("/NetworkTest/checkConnectionQos/p1");
        p2.open("/NetworkTest/checkConnectionQos/p2");
        Network::connect(p1.getName(), p2.getName());
        Bottle &bt = p1.prepare();
        bt.addString("test");
        p1.write();
        yarp::os::QosStyle style;
        style.setThreadPriority(0);
        style.setThreadPolicy(0);
        style.setPacketPriorityByLevel(yarp::os::QosStyle::PacketPriorityNormal);
        checkTrue(Network::setConnectionQos(p1.getName(), p2.getName(), style, style, false),
                  "connection Qos working");
        Network::disconnect(p1.getName(), p2.getName());
        p1.close();
        p2.close();
    }

    virtual void runTests() override {
        Network::setLocalMode(true);
        checkConnect();
        checkSync();
        checkComms();
        checkPropertySetGet();
        checkTimeoutNetworkWrite();
        checkTimeoutNetworkExists();
        checkTopics();
        checkPersistence();
        checkConnectionQos();
        Network::setLocalMode(false);
    }
};

static NetworkTest theNetworkTest;

UnitTest& getNetworkTest() {
    return theNetworkTest;
}

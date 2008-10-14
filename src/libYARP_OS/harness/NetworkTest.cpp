// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/ConstString.h>
#include <yarp/os/Time.h>
#include <yarp/os/Bottle.h>

#include <yarp/os/impl/UnitTest.h>
//#include "TestList.h"

using namespace yarp::os::impl;
using namespace yarp::os;

class NetworkTestWorker1 : public Thread {
public:
    Semaphore fini;
    ConstString name;
    Port p;

    NetworkTestWorker1() : fini(0) {
    }

    void run() {
        Time::delay(0.5);
        p.open(name.c_str());
        fini.wait();
    }
};


class NetworkServiceProvider : public PortReader {
public:

    virtual bool read(ConnectionReader& connection) {
        Bottle receive;
        //printf("service provider reading data\n");
        receive.read(connection);
        //printf("service provider read data\n");
        receive.addInt(5);
        ConnectionWriter *writer = connection.getWriter();
        if (writer!=NULL) {
            //printf("service provider replying\n");
            receive.write(*writer);
            //printf("service provider replied\n");
        }
        return true;
    }
};


class NetworkTest : public UnitTest {
public:
    virtual String getName() { return "NetworkTest"; }

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
        checkFalse(Network::connect("/p1","/p3"),"bad connect");
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
        checkEqual(reply.get(1).asInt(),5,"append is correct");
        server.close();
    }


    void checkPropertySetGet() {
        report(0,"checking property storage on name server");
        Network::registerName("/foo");
        Network::setProperty("/foo","my_prop",Value(15));
        Value *v = Network::getProperty("/foo","my_prop");
        checkTrue(v!=NULL,"got property");
        if (v!=NULL) {
            checkEqual(v->asInt(),15,"recover property");
            delete v;
        }
        Network::unregisterName("/foo");
    }

    virtual void runTests() {
        Network::setLocalMode(true);
        checkConnect();
        checkSync();
        checkComms();
        checkPropertySetGet();
        Network::setLocalMode(false);
    }
};

static NetworkTest theNetworkTest;

UnitTest& getNetworkTest() {
    return theNetworkTest;
}

// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2007 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/Port.h>
#include <yarp/os/impl/Companion.h>
#include <yarp/os/Time.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/PortReaderBuffer.h>
#include <yarp/os/PortWriterBuffer.h>
#include <yarp/os/PortablePair.h>
#include <yarp/os/BinPortable.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/NetType.h>
#include <yarp/os/impl/UnitTest.h>

#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>
#include <yarp/os/PortReport.h>

#include <yarp/os/RpcClient.h>
#include <yarp/os/RpcServer.h>
#include <yarp/os/PortInfo.h>

//#include "TestList.h"

using namespace yarp::os;
using namespace yarp::os::impl;


#ifndef DOXYGEN_SHOULD_SKIP_THIS

class ServiceProvider : public PortReader {
public:

    virtual bool read(ConnectionReader& connection) {
        Bottle receive;
        receive.read(connection);
        receive.addInt(5);
        ConnectionWriter *writer = connection.getWriter();
        if (writer!=NULL) {
            receive.write(*writer);
        }
        return true;
    }
};

class ServiceTester : public Portable {
public:
    UnitTest& owner;
    Bottle send, receive;
    int ct;

    ServiceTester(UnitTest& owner) : owner(owner) {}

    virtual bool write(ConnectionWriter& connection) {
        ct = 0;
        send.write(connection);
        connection.setReplyHandler(*this);
        return true;
    }

    virtual bool read(ConnectionReader& connection) {
        receive.read(connection);
        ct++;
        return true;
    }

    void finalCheck() {
        owner.checkEqual(receive.size(),send.size()+1,"size incremented");
        owner.checkEqual(ct,1,"just one read");
    }
};


class DelegatedReader : public Thread {
public:
    bool faithful;
    Port p;

    DelegatedReader(bool faithful = true) {
        p.open("/reader");
        this->faithful = faithful;
    }

    virtual void run() {
        for (int i=0; i<3; i++) {
            Bottle b,b2;
            p.read(b,true);
            b2.addInt(b.get(0).asInt()+1);
            if ((!faithful)&&i==1) {
                // no reply
            } else {
                p.reply(b2);
            }
        }
    }
};

class DelegatedWriter : public Thread {
public:
    Port p;
    int total;

    DelegatedWriter() {
        p.open("/writer");
        Network::connect("/writer","/reader");
    }

    virtual void run() {
        total = 0;
        for (int i=0; i<3; i++) {
            Bottle b, b2;
            b.addInt(i);
            p.write(b,b2);
            total += b2.get(0).asInt(); // should be i+1
        }
        // total should be 1+2+3 = 6
    }
};


class DelegatedCallback : public TypedReaderCallback<Bottle> {
public:
    Bottle saved;
    Semaphore produce;

    DelegatedCallback() : produce(0) {}

    virtual void onRead(Bottle& bot) {
        saved = bot;
        produce.post();
    }
};


class MyReport : public PortReport {
public:
    int ct;
    int oct;
    int ict;

    MyReport() {
        ict = oct = ct = 0;
    }

    virtual void report(const PortInfo& info) {
        if (info.tag == PortInfo::PORTINFO_CONNECTION) {
            if (info.incoming == false) {
                oct++;
            } else {
                ict++;
            }
        }
        ct++;
    }
};

class WriteReader : public Thread {
public:
    Port p;
    bool done;

    WriteReader() {
        done = false;
        p.open("/write");
    }

    void finish() {
        done = true;
        stop();
    }

    virtual void run() {
        while (!done) {
            Bottle msg("1 \"end of terminal\"");
            p.write(msg);
            Time::delay(0.25);
        }
    }
};

class ServiceUser : public Thread {
public:
    Port p;

    ServiceUser(const char *name) {
        p.open(name);
    }

    virtual void run() {
        Bottle cmd, reply;
        cmd.fromString("[add] 1 2");
        p.write(cmd,reply);
    }
};

class StreamUser : public Thread {
public:
    Port p;

    StreamUser(const char *name) {
        p.setTimeout(2);
        p.open(name);
    }

    virtual void run() {
        Bottle cmd;
        cmd.fromString("[add] 1 2");
        p.write(cmd);
    }
};

class DataPort : public BufferedPort<Bottle> {
public:
    int ct;

    DataPort() {
        ct = 0;
    }

    virtual void onRead(Bottle& b) {
        ct++;
     }
};

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/


class PortTest : public UnitTest {
public:
    int safePort() { return Network::getDefaultPortRange()+100; }

    virtual String getName() { return "PortTest"; }

    void testOpen() {
        report(0,"checking opening and closing ports");
        Port out, in;

        in.open("/in");
        out.open(Contact::bySocket("tcp","",safePort()));

        checkTrue(in.isOpen(), "/in port is open");
        checkTrue(out.isOpen(), "/out port is open");

        Contact conIn = in.where();
        Contact conOut = out.where();

        checkTrue(conIn.isValid(),"valid address for /in");
        checkTrue(conOut.isValid(),"valid address for /out");

        out.addOutput(Contact::byName("/in").addCarrier("tcp"));
        //Time::delay(0.2);

        checkEqual(conIn.getName().c_str(),"/in","name is recorded");

        checkTrue(conOut.getName().find("/tmp")==0,
                  "name is created");

        Bottle bot1, bot2;
        bot1.fromString("5 10 \"hello\"");
        out.enableBackgroundWrite(true);
        out.write(bot1);
        in.read(bot2);
        checkEqual(bot1.get(0).asInt(),5,"check bot[0]");
        checkEqual(bot1.get(1).asInt(),10,"check bot[1]");
        checkEqual(bot1.get(2).asString().c_str(),"hello","check bot[2]");

        while (out.isWriting()) {
            printf("Waiting...\n");
            Time::delay(0.1);
        }

        bot1.fromString("18");
        out.write(bot1);
        in.read(bot2);
        checkEqual(bot1.get(0).asInt(),18,"check one more send/receive");

        in.close();
        out.close();
    }


    void testReadBuffer() {
        report(0,"checking read buffering");
        Bottle bot1;
        PortReaderBuffer<Bottle> buf;
        buf.setStrict(true);

        bot1.fromString("1 2 3");
        for (int i=0; i<10000; i++) {
            bot1.addInt(i);
        }

        Port input, output;
        input.open("/in");
        output.open("/out");

        buf.setStrict();
        buf.attach(input);

        output.addOutput(Contact::byName("/in").addCarrier("udp"));
        //Time::delay(0.2);

        report(0,"writing...");
        output.write(bot1);
        output.write(bot1);
        output.write(bot1);
        report(0,"reading...");
        Bottle *result = buf.read();

        for (int j=0; j<3; j++) {
            if (j!=0) {
                result = buf.read();
            }
            checkTrue(result!=NULL,"got something check");
            if (result!=NULL) {
                checkEqual(bot1.size(),result->size(),"size check");
                YARP_INFO(Logger::get(),String("size is in fact ") +
                          NetType::toString(result->size()));
            }
        }

        output.close();
        input.close();
    }

    void testUdp() {
        report(0,"checking udp");

        Bottle bot1;
        PortReaderBuffer<Bottle> buf;

        bot1.fromString("1 2 3");
        for (int i=0; i<10000; i++) {
            bot1.addInt(i);
        }

        Port input, output;
        input.open("/in");
        output.open("/out");

        buf.setStrict();
        buf.attach(input);

        output.addOutput(Contact::byName("/in").addCarrier("udp"));
        //Time::delay(0.2);

        report(0,"writing/reading three times...");

        report(0,"checking for whatever got through...");
        int ct = 0;
        while (buf.check()) {
            ct++;
            Bottle *result = buf.read();
            checkTrue(result!=NULL,"got something check");
            if (result!=NULL) {
                checkEqual(bot1.size(),result->size(),"size check");
                YARP_INFO(Logger::get(),String("size is in fact ") +
                          NetType::toString(result->size()));
            }
        }
        if (ct==0) {
            report(0,"NOTHING got through - possible but sad");
        }

        output.close();
        input.close();
    }


    void testHeavy() {
        report(0,"checking heavy udp");

        Bottle bot1;
        PortReaderBuffer<Bottle> buf;

        bot1.fromString("1 2 3");
        for (int i=0; i<100000; i++) {
            bot1.addInt(i);
        }

        Port input, output;
        input.open("/in");
        output.open("/out");

        buf.setStrict();
        buf.attach(input);

        output.addOutput(Contact::byName("/in").addCarrier("udp"));
        //Time::delay(0.2);


        for (int j=0; j<3; j++) {
            report(0,"writing/reading three times...");
            output.write(bot1);
        }

        report(0,"checking for whatever got through...");
        int ct = 0;
        while (buf.check()) {
            ct++;
            Bottle *result = buf.read();
            checkTrue(result!=NULL,"got something check");
            if (result!=NULL) {
                checkEqual(bot1.size(),result->size(),"size check");
                YARP_INFO(Logger::get(),String("size is in fact ") +
                          NetType::toString(result->size()));
            }
        }
        if (ct==0) {
            report(0,"NOTHING got through - possible but sad");
        }

        output.close();
        input.close();
    }


    void testPair() {
        report(0,"checking paired send/receive");
        PortReaderBuffer<PortablePair<Bottle,Bottle> > buf;

        Port input, output;
        input.open("/in");
        output.open("/out");

        buf.setStrict();
        buf.attach(input);

        output.addOutput(Contact::byName("/in").addCarrier("tcp"));
        //Time::delay(0.2);


        PortablePair<Bottle,Bottle> bot1;
        bot1.head.fromString("1 2 3");
        bot1.body.fromString("4 5 6 7");

        report(0,"writing...");
        output.write(bot1);
        bool ok = output.write(bot1);
        checkTrue(ok,"output proceeding");
        report(0,"reading...");
        PortablePair<Bottle,Bottle> *result = buf.read();

        checkTrue(result!=NULL,"got something check");
        checkEqual(bot1.head.size(),result->head.size(),"head size check");
        checkEqual(bot1.body.size(),result->body.size(),"body size check");

        output.close();
        input.close();
    }


    void testReply() {
        report(0,"checking reply processing");
        ServiceProvider provider;

        Port input, output;
        input.open("/in");
        output.open("/out");

        input.setReader(provider);

        output.addOutput(Contact::byName("/in").addCarrier("tcp"));
        Time::delay(0.1);
        ServiceTester tester(*this);
        output.write(tester);
        Time::delay(0.1);
        tester.finalCheck();
        Time::delay(0.1);
        output.close();
        input.close();
    }


    virtual void testBackground() {
        report(0,"test communication in background mode");

        Port input, output;
        input.open("/in");
        output.open("/out");
        output.enableBackgroundWrite(true);

        BinPortable<int> bin, bout;
        bout.content() = 42;
        bin.content() = 20;

        output.addOutput("/in");

        report(0,"writing...");
        output.write(bout);
        report(0,"reading...");
        input.read(bin);

        checkEqual(bout.content(),bin.content(),"successful transmit");

        while (output.isWriting()) {
            report(0,"waiting for port to stabilize");
            Time::delay(0.2);
        }

        bout.content() = 88;

        report(0,"writing...");
        output.write(bout);
        report(0,"reading...");
        input.read(bin);

        checkEqual(bout.content(),bin.content(),"successful transmit");

        output.close();
        input.close();
    }

    void testWriteBuffer() {
        report(0,"testing write buffering");

        Port input, output, altInput;
        input.open("/in");
        altInput.open("/in2");
        output.open("/out");
        output.addOutput("/in");

        report(0,"beginning...");

        BinPortable<int> bin;
        PortWriterBuffer<BinPortable<int> > binOut;
        binOut.attach(output);

        int val1 = 15;
        int val2 = 30;

        BinPortable<int>& active = binOut.get();
        active.content() = val1;
        binOut.write();

        output.addOutput("/in2");
        BinPortable<int>& active2 = binOut.get();
        active2.content() = val2;
        binOut.write();

        input.read(bin);
        checkEqual(val1,bin.content(),"successful transmit");

        altInput.read(bin);
        checkEqual(val2,bin.content(),"successful transmit");

        while (output.isWriting()) {
            report(0,"waiting for port to stabilize");
            Time::delay(0.2);
        }

        report(0,"port stabilized");
        output.close();
        report(0,"shut down output buffering");
    }

    void testBufferedPort() {
        report(0,"checking buffered port");
        BufferedPort<BinPortable<int> > output, input;
        output.open("/out");
        input.open("/in");

        report(0,"is write a no-op when no connection exists?...");
        BinPortable<int>& datum0 = output.prepare();
        datum0.content() = 123;
        report(0,"writing...");
        output.write();

        output.addOutput("/in");
        report(0,"now with a connection...");
        BinPortable<int>& datum = output.prepare();
        datum.content() = 999;
        report(0,"writing...");
        output.write();
        report(0,"reading...");
        BinPortable<int> *bin = input.read();
        checkEqual(bin->content(),999,"good send");
    }

    void testCloseOrder() {
        report(0,"check that port close order doesn't matter...");

        for (int i=0; i<4; i++) {
            // on OSX there is a problem only tickled upon repetition
            {
                Port input, output;
                input.open("/in");
                output.open("/out");
                output.addOutput("/in");

                report(0,"closing in");
                input.close();

                report(0,"closing out");
                output.close();
            }

            {
                Port input, output;
                input.open("/in");
                output.open("/out");
                output.addOutput("/in");

                report(0,"closing out");
                output.close();

                report(0,"closing in");
                input.close();
            }
        }
    }


    virtual void testDelegatedReadReply() {
        report(0,"check delegated read and reply...");
        DelegatedReader reader;
        DelegatedWriter writer;
        reader.start();
        writer.start();
        writer.stop();
        reader.stop();
        checkEqual(writer.total,6,"read/replies give right checksum");
    }

    virtual void testReaderHandler() {
        report(0,"check reader handler...");
        Port in;
        Port out;
        DelegatedCallback callback;
        out.open("/out");
        in.open("/in");
        Network::connect("/out","/in");
        PortReaderBuffer<Bottle> reader;
        reader.setStrict();
        reader.attach(in);
        reader.useCallback(callback);
        Bottle src("10 10 20");
        out.write(src);
        callback.produce.wait();
        checkEqual(callback.saved.size(),3,"object came through");
        reader.disableCallback();
        out.close();
        in.close();
    }

    virtual void testReaderHandler2() {
        report(0,"check reader handler, bufferedport style...");
        BufferedPort<Bottle> in;
        Port out;
        DelegatedCallback callback;
        in.setStrict();
        out.open("/out");
        in.open("/in");
        Network::connect("/out","/in");
        in.useCallback(callback);
        Bottle src("10 10 20");
        out.write(src);
        callback.produce.wait();
        checkEqual(callback.saved.size(),3,"object came through");
        in.disableCallback();
    }

    virtual void testReaderHandlerNoOpen() {
        report(0,"check reader handler without open...");
        {
            report(0, "test 1");
            Port in;
            DelegatedCallback callback;
            PortReaderBuffer<Bottle> reader;
            reader.setStrict();
            reader.attach(in);
            reader.useCallback(callback);
            reader.disableCallback();
            in.close();
        }
        {
            report(0, "test 2");
            Port in;
            DelegatedCallback callback;
            PortReaderBuffer<Bottle> reader;
            reader.setStrict();
            reader.attach(in);
            reader.useCallback(callback);
            reader.disableCallback();
        }
        {
            report(0, "test 3");
            Port in;
            DelegatedCallback callback;
            PortReaderBuffer<Bottle> reader;
            reader.setStrict();
            reader.attach(in);
            reader.useCallback(callback);
            in.close();
        }
        {
            report(0, "test 4");
            Port in;
            DelegatedCallback callback;
            PortReaderBuffer<Bottle> reader;
            reader.setStrict();
            reader.attach(in);
            reader.useCallback(callback);
        }
    }

    virtual void testStrictWriter() {
        report(0,"check strict writer...");
        BufferedPort<Bottle> in;
        BufferedPort<Bottle> out;
        in.setStrict();
        in.open("/in");
        out.open("/out");

        Network::connect("/out","/in");

        Bottle& outBot1 = out.prepare();
        outBot1.fromString("hello world");
        printf("Writing bottle 1: %s\n", outBot1.toString().c_str());
        out.write(true);

        Bottle& outBot2 = out.prepare();
        outBot2.fromString("2 3 5 7 11");
        printf("Writing bottle 2: %s\n", outBot2.toString().c_str());
        out.write(true);

        Bottle *inBot1 = in.read();
        checkTrue(inBot1!=NULL,"got 1 of 2 items");
        if (inBot1!=NULL) {
            printf("Bottle 1 is: %s\n", inBot1->toString().c_str());
            checkEqual(inBot1->size(),2,"match for item 1");
        }
        Bottle *inBot2 = in.read();
        checkTrue(inBot2!=NULL,"got 2 of 2 items");
        if (inBot2!=NULL) {
            printf("Bottle 2 is: %s\n", inBot2->toString().c_str());
            checkEqual(inBot2->size(),5,"match for item 1");
        }
    }


    virtual void testRecentReader() {
        report(0,"check recent reader...");
        BufferedPort<Bottle> in;
        BufferedPort<Bottle> out;
        in.setStrict(false);
        in.open("/in");
        out.open("/out");

        Network::connect("/out","/in");

        Bottle& outBot1 = out.prepare();
        outBot1.fromString("hello world");
        printf("Writing bottle 1: %s\n", outBot1.toString().c_str());
        out.write(true);

        Bottle& outBot2 = out.prepare();
        outBot2.fromString("2 3 5 7 11");
        printf("Writing bottle 2: %s\n", outBot2.toString().c_str());
        out.write(true);

        Time::delay(0.25);

        Bottle *inBot2 = in.read();
        checkTrue(inBot2!=NULL,"got 2 of 2 items");
        if (inBot2!=NULL) {
            printf("Bottle 2 is: %s\n", inBot2->toString().c_str());
            checkEqual(inBot2->size(),5,"match for item 1");
        }
    }


    virtual void testUnbufferedClose() {
        report(0,"check that ports that receive data and do not read it can close...");
        BufferedPort<Bottle> sender;
        Port receiver;
        sender.open("/sender");
        receiver.open("/receiver");
        Network::connect("/sender","/receiver");
        Time::delay(0.25);
        Bottle& bot = sender.prepare();
        bot.clear();
        bot.addInt(1);
        sender.write();
        Time::delay(0.25);
        report(0,"if this hangs, PortTest::testUnbufferedClose is unhappy");
        receiver.close();
        sender.close();
    }

    virtual void testCloseOpenRepeats() {
        report(0,"check that opening-closing-opening etc is ok...");
        report(0,"non-buffered port:");
        Port p;
        p.open("/test1");
        p.open("/test2");
        p.open("/test3");
        p.close();
        p.open("/test4");
        p.close();
        report(0,"buffered port:");
        BufferedPort<Bottle> p2, p3;
        p2.open("/test1");
        p2.open("/test2");
        p2.open("/in");
        p3.open("/out");
        Network::connect("/out","/in");
        p3.prepare().fromString("10 20 30");
        p3.write();
        report(0,"wait for input...");
        p2.read(true);
        report(0,"... got it");
        p3.prepare().fromString("10 20 30");
        p3.write();
        p2.open("/test1");
        p3.open("/test2");
        Network::connect("/test2","/test1");
        p3.prepare().fromString("10 20 30");
        p3.write();
        report(0,"wait for input...");
        p2.read(true);
        report(0,"... got it");

        report(0,"fast loop on temporary port");
        for (int i=0; i<20; i++) {
            Port p;
            Port p2;
            p.open("...");
            p2.open("...");
            p.close();
        }
        Port pa;
        pa.open("...");
        for (int i=0; i<20; i++) {
            Port p;
            p.enableBackgroundWrite(true);
            p.open("...");
            NetworkBase::connect(p.getName(),pa.getName());
            Bottle b("10 20 30");
            p.write(b);
            pa.read(b);
            p.close();
        }
    }


    virtual void testCounts() {

        report(0,"check that input/output counts are accurate...");
        int top = 3;
        Port p[3];
        p[0].open("/a");
        p[1].open("/b");
        p[2].open("/c");
        for (int i=0; i<top; i++) {
            checkEqual(p[i].getInputCount(),0,"no input connections");
            checkEqual(p[i].getOutputCount(),0,"no output connections");
        }
        Network::connect("/a","/b");
        Network::connect("/a","/c");

        Network::sync("/a");
        Network::sync("/b");
        Network::sync("/c");

        checkEqual(p[0].getInputCount(),0,"input connections");
        checkEqual(p[0].getOutputCount(),2,"output connections");
        checkEqual(p[1].getInputCount(),1,"input connections");
        checkEqual(p[1].getOutputCount(),0,"output connections");
        checkEqual(p[2].getInputCount(),1,"input connections");
        checkEqual(p[2].getOutputCount(),0,"output connections");

        Network::disconnect("/a","/c");

        Network::sync("/a");
        Network::sync("/b");
        Network::sync("/c");

        checkEqual(p[0].getInputCount(),0,"input connections");
        checkEqual(p[0].getOutputCount(),1,"output connections");
        checkEqual(p[1].getInputCount(),1,"input connections");
        checkEqual(p[1].getOutputCount(),0,"output connections");
        checkEqual(p[2].getInputCount(),0,"input connections");
        checkEqual(p[2].getOutputCount(),0,"output connections");

    }


    virtual void testReadNoReply() {
        report(0,"check that we survive if no reply() made when promised...");

        Port p1;
        DelegatedReader reader(false);
        reader.start();
        p1.open("/writer");
        Network::connect("/writer","/reader");
        Network::sync("/writer");
        Network::sync("/reader");
        Bottle bsend, breply;
        bsend.addInt(10);
        p1.write(bsend, breply);
        p1.write(bsend, breply);
        p1.write(bsend, breply);
        reader.stop();
    }


    virtual void testReports() {
        report(0,"check port status report...");


        {
            Port p1;
            Port p2;
            p1.open("/foo");
            p2.open("/bar");
            Network::connect("/foo","/bar");
            Network::sync("/foo");
            Network::sync("/bar");
            MyReport report;
            p1.getReport(report);
            checkTrue(report.ct>0,"got some report");
            checkEqual(report.oct,1,"exactly one output");
            p1.close();
            p2.close();
        }

        {
            Port p1;
            Port p2;
            MyReport report1, report2;
            p1.setReporter(report1);
            p2.setReporter(report2);
            p1.open("/foo");
            p2.open("/bar");
            Network::connect("/foo","/bar");
            Network::sync("/foo");
            Network::sync("/bar");
            checkTrue(report1.ct>0,"sender got report callback");
            checkEqual(report1.oct,1,"exactly one output callback");
            checkEqual(report1.ict,0,"exactly zero input callbacks");
            checkTrue(report2.ct>0,"receiver got report callback");
            checkEqual(report2.oct,0,"exactly zero output callbacks");
            checkEqual(report2.ict,1,"exactly one input callback");
            p1.close();
            p2.close();
        }

    }


    virtual void testReportsWithRpcClient() {
        report(0,"check port status report with rpc client...");

        {
            RpcClient p1;
            RpcServer p2;
            p1.open("/foo");
            p2.open("/bar");
            Network::connect("/foo","/bar");
            Network::sync("/foo");
            Network::sync("/bar");
            MyReport report;
            p1.getReport(report);
            checkTrue(report.ct>0,"got some report");
            checkEqual(report.oct,1,"exactly one output");
            p1.close();
            p2.close();
        }

        {
            RpcClient p1;
            RpcServer p2;
            MyReport report1, report2;
            p1.setReporter(report1);
            p2.setReporter(report2);
            p1.open("/foo");
            p2.open("/bar");
            Network::connect("/foo","/bar");
            Network::sync("/foo");
            Network::sync("/bar");
            checkTrue(report1.ct>0,"sender got report callback");
            checkEqual(report1.oct,1,"exactly one output callback");
            checkEqual(report1.ict,0,"exactly zero input callbacks");
            checkTrue(report2.ct>0,"receiver got report callback");
            checkEqual(report2.oct,0,"exactly zero output callbacks");
            checkEqual(report2.ict,1,"exactly one input callback");
            p1.close();
            p2.close();
        }

    }


    virtual void testAdmin() {
        report(0,"check port admin interface...");

        BufferedPort<Bottle> p1;
        Port p2;
        p1.open("/p1");
        p2.open("/p2");
        Network::connect("/p2","/p1");
        Network::sync("/p1");
        Network::sync("/p2");

        Bottle cmd("[help]"), reply;
        p2.setAdminMode();
        p2.write(cmd,reply);

        checkTrue(reply.size()>=1, "got a reply");

        p1.close();
        p2.close();
    }

    virtual void testAcquire() {
        report(0, "checking acquire/release...");

        BufferedPort<Bottle> in;
        BufferedPort<Bottle> out;
        in.setStrict();
        out.setStrict();
        in.open("/in");
        out.open("/out");
        Network::connect("/out","/in");

        out.prepare().fromString("1");
        out.write(true);

        Bottle *bot = in.read();
        checkTrue(bot!=NULL,"Inserted message received");
        if (bot!=NULL) {
            checkEqual(bot->size(),1,"right length");
        }

        out.prepare().fromString("1 2");
        out.write(true);

        void *key = in.acquire();
        Bottle *bot2 = in.read();
        checkTrue(bot2!=NULL,"Inserted message received");
        if (bot2!=NULL) {
            checkEqual(bot2->size(),2,"right length");
        }

        out.prepare().fromString("1 2 3");
        out.write(true);

        void *key2 = in.acquire();
        Bottle *bot3 = in.read();
        checkTrue(bot3!=NULL,"Inserted message received");
        if (bot3!=NULL) {
            checkEqual(bot3->size(),3,"right length");
        }
        if (bot2!=NULL) {
            checkEqual(bot2->size(),2,"original (2) still ok");
        }
        if (bot!=NULL) {
            checkEqual(bot->size(),1,"original (1) still ok");
        }

        in.release(key);
        in.release(key2);
    }


    virtual void testTimeout() {
        report(0,"check N second timeout...");
        Port a;
        Port b;
        bool ok = a.setTimeout(0.5);
        checkTrue(a.setTimeout(0.5),"set timeout");
        if (!ok) return;
        a.open("/a");
        b.open("/b");
        NetworkBase::connect("/a","/b");
        Bottle msg("hello"), reply;
        ok = a.write(msg,reply);
        checkFalse(ok,"send failed correctly");
    }

    // regression test for bug reported by Andrea Del Prete and Ugo Pattacini
    virtual void testYarpRead() {
        report(0,"check yarp ... /write works ...");
        WriteReader writer;
        writer.start();
        int argc = 2;
        const char *argv[] = {"...","/write"};
        Companion::getInstance().cmdRead(argc,(char**)argv);
        writer.finish();
    }

    virtual void testMissingSlash() {
        report(0,"check behavior on missing slash...");
        Port p;
        const char *name = "something/without/slash";
        bool opened = p.open(name);
        checkFalse(opened,"correctly rejected port");
    }

    void testInterrupt() {
        report(0,"checking interrupt...");
        PortReaderBuffer<PortablePair<Bottle,Bottle> > buf;

        Port input, output;
        input.open("/in");
        output.open("/out");

        buf.setStrict();
        buf.attach(input);

        output.addOutput(Contact::byName("/in").addCarrier("tcp"));

        PortablePair<Bottle,Bottle> bot1;
        bot1.head.fromString("1 2 3");
        bot1.body.fromString("4 5 6 7");

        report(0,"interrupting...");
        output.interrupt();
        report(0,"writing...");
        bool ok = output.write(bot1);
        checkFalse(ok,"output rejected correctly");
        output.resume();
        ok = output.write(bot1);
        checkTrue(ok,"output goes through after resume");

        output.close();
        input.close();
    }


    void testInterruptInputReaderBuf() {
        report(0,"checking interrupt on input side...");
        PortReaderBuffer<Bottle> buf;
        buf.setStrict(true);

        Port input, output;
        input.open("/in");
        output.open("/out");

        buf.setStrict();
        buf.attach(input);

        output.addOutput(Contact::byName("/in").addCarrier("tcp"));

        Bottle bot1;
        bot1.fromString("1 2 3");

        output.write(bot1);
        for (int i=0; i<20 && buf.getPendingReads()<1; i++) {
            Time::delay(0.1);
        }
        checkEqual(buf.getPendingReads(),1,"first msg came through");
        Bottle *bot2 = buf.read();
        yAssert(bot2);
        checkEqual(bot2->size(),3,"data looks ok");

        bot1.addInt(4);

        report(0,"interrupting...");
        input.interrupt();

        output.write(bot1);
        for (int i=0; i<10 && buf.getPendingReads()<1; i++) {
            Time::delay(0.1);
        }
        checkEqual(buf.getPendingReads(),0,"msg after interrupt ignored");

        bot1.addInt(5);

        input.resume();
        output.write(bot1);
        for (int i=0; i<20 && buf.getPendingReads()<1; i++) {
            Time::delay(0.1);
        }
        checkEqual(buf.getPendingReads(),1,"next msg came through");
        bot2 = buf.read();
        yAssert(bot2);
        checkEqual(bot2->size(),5,"data looks ok");

        output.close();
        input.close();
    }


    void testInterruptInputNoBuf() {
        report(0,"checking interrupt on input side without buffering...");

        Port input, output;
        input.open("/in");
        output.enableBackgroundWrite(true);
        output.open("/out");

        output.addOutput(Contact::byName("/in").addCarrier("tcp"));

        Bottle bot1, bot2;
        bot1.fromString("1 2 3");

        output.write(bot1);
        bool ok = input.read(bot2);
        checkTrue(ok,"first msg came through");
        checkEqual(bot2.size(),3,"data looks ok");

        bot1.addInt(4);

        report(0,"interrupting...");
        input.interrupt();

        output.write(bot1);
        checkFalse(input.read(bot2),"msg after interrupt ignored");

        Time::delay(1);

        bot1.addInt(5);

        report(0,"resuming");
        input.resume();
        output.write(bot1);
        ok = input.read(bot2);
        checkTrue(ok,"next msg came through");
        checkEqual(bot2.size(),5,"data looks ok");

        output.close();
        input.close();
    }


    void testInterruptReply() {
        report(0,"checking interrupt for a port with pending reply...");
        PortReaderBuffer<PortablePair<Bottle,Bottle> > buf;

        ServiceUser output("/out");
        Port input;
        input.open("/in");
        Network::connect(output.p.getName(),input.getName());
        output.start();
        Bottle cmd, reply;
        input.read(cmd,true);
        reply.addInt(cmd.get(1).asInt()+cmd.get(2).asInt());
        checkEqual(cmd.toString().c_str(),"[add] 1 2","cmd received ok");
        input.interrupt();
        input.reply(reply);
        input.close();
        output.stop();
        output.p.close();
        report(0,"successfully closed");
    }

    void testInterruptWithBadReader() {
        report(0,"checking interrupt with bad reader...");

        StreamUser output("/out");
        Port input;
        input.open("/in");

        Network::connect("/out","/in");

        output.start();
        Time::delay(2);

        report(0,"interrupting output...");
        output.p.interrupt();
        output.stop();
        report(0,"made it through");

        input.close();
    }


    void testReopen() {
        report(0,"checking opening/closing/reopening ports...");

        BufferedPort<Bottle> port2;
        port2.open("/test2");

        BufferedPort<Bottle> port;
        port.open("/test");

        Network::connect("/test2", "/test");
        Network::sync("/test");
        Network::sync("/test2");
        port2.prepare().fromString("1 msg");
        port2.write();

        while (port.getPendingReads()<1) {
            Time::delay(0.1);
        }

        checkFalse(port.isClosed(),"port tagged as open");
        port.close();
        checkTrue(port.isClosed(),"port tagged as closed");
        port.open("/test");
        checkFalse(port.isClosed(),"port tagged as open");

        Bottle *bot = port.read(false);
        checkTrue(bot==NULL,"reader correctly reset");

        Network::connect("/test2", "/test");
        Network::sync("/test");
        Network::sync("/test2");
        port2.prepare().fromString("2 msg");
        port2.write();

        bot = port.read();
        checkFalse(bot==NULL,"reader working");
        if (bot) {
            checkEqual(bot->get(0).asInt(),2,"reader read correct message");
        }
    }

    virtual void testBufferedPortCallback() {
        report(0,"checking BufferedPort callback...");
        DataPort pin;
        pin.useCallback();
        BufferedPort<Bottle> pout;
        pout.open("/out");
        pin.open("/in");
        Network::connect("/out","/in");
        Network::sync("/out");
        Network::sync("/in");
        Bottle& msg = pout.prepare();
        msg.clear();
        msg.addInt(42);
        pout.write();
        pout.waitForWrite();
        pout.close();
        pin.close();
        checkEqual(pin.ct,1,"callback happened");
    }

    virtual void testBufferedPortCallbackNoOpen() {
        report(0,"checking BufferedPort callback without open...");
        {
            report(0, "test 1");
            DataPort pin;
            pin.useCallback();
            pin.disableCallback();
        }
        {
            report(0, "test 2");
            DataPort pin;
            pin.useCallback();
            pin.disableCallback();
            pin.close();
        }
        {
            report(0, "test 3");
            DataPort pin;
            pin.useCallback();
        }
        {
            report(0, "test 4");
            DataPort pin;
            pin.useCallback();
            pin.close();
        }
    }

    void testAdminReader() {
        report(0,"checking user-level admin message reads");
        Port pin;
        ServiceProvider admin_reader;
        pin.setAdminReader(admin_reader);
        pin.open("/in");
        Port pout;
        pout.setAdminMode();
        pout.open("/out");
        Network::connect("/out","/in");
        Bottle cmd("hello"), reply;
        pout.write(cmd,reply);
        checkEqual(reply.get(1).asInt(),5,"admin_reader was called");
        cmd.fromString("[ver]");
        pout.write(cmd,reply);
        checkTrue(reply.size()>=4,"yarp commands still work");
    }

    void testCallbackLock() {
        report(0,"checking callback locking");
        Port pin, pout;
        Bottle data;
        pin.setCallbackLock();
        pin.setReader(data);
        pout.enableBackgroundWrite(true);
        pin.open("/in");
        pout.open("/out");
        Network::connect("/out","/in");
        Bottle cmd("hello");
        pin.lockCallback();
        pout.write(cmd);
        Time::delay(0.25);
        checkEqual(data.size(),0,"data does not arrive too early");
        pin.unlockCallback();
        while (pout.isWriting()) {
            report(0,"waiting for port to stabilize");
            Time::delay(0.2);
        }
        checkEqual(data.size(),1,"data does eventually arrive");
        pin.close();
        pout.close();
    }

    virtual void runTests() {
        NetworkBase::setLocalMode(true);

        testOpen();
        //bbb testReadBuffer();
        testPair();
        testReply();
        testUdp();
        //testHeavy();

        testBackground();
        testWriteBuffer();
        testBufferedPort();
        testCloseOrder();
        testDelegatedReadReply();
        testReaderHandler();
        testReaderHandler2();
        testReaderHandlerNoOpen();
        testStrictWriter();
        testRecentReader();

        testUnbufferedClose(); //TODO

        testCloseOpenRepeats(); //bring this back soon

        //testCounts(); // bring this back soon

        testReadNoReply();
        testAdmin();
        testAcquire();

        testTimeout();

        testYarpRead();

        testMissingSlash();

        testReports();
        testReportsWithRpcClient();

        testInterrupt();

        testInterruptReply();

        testReopen();

        testInterruptInputReaderBuf();
        testInterruptInputNoBuf();
        testInterruptWithBadReader();

        testBufferedPortCallback();
        testBufferedPortCallbackNoOpen();

        testAdminReader();

        testCallbackLock();

        NetworkBase::setLocalMode(false);
    }
};

static PortTest thePortTest;

UnitTest& getPortTest() {
    return thePortTest;
}


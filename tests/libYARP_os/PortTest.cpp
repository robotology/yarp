/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Port.h>

#include <yarp/os/Time.h>
#include <yarp/os/Thread.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Carriers.h>
#include <yarp/os/ModifyingCarrier.h>
#include <yarp/os/PortReaderBuffer.h>
#include <yarp/os/PortWriterBuffer.h>
#include <yarp/os/PortablePair.h>
#include <yarp/os/BinPortable.h>
#include <yarp/os/NetType.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>
#include <yarp/os/PortReport.h>
#include <yarp/os/RpcClient.h>
#include <yarp/os/RpcServer.h>
#include <yarp/os/PortInfo.h>
#include <yarp/os/Log.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/Drivers.h>

#include <yarp/sig/Image.h>

#include <yarp/companion/impl/Companion.h>

#include <mutex>
#include <condition_variable>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::os::impl;

/**
 * @ingroup dev_impl_media
 *
 * A fake device for testing closure after a prepare of a closed port.
 */
class BrokenDevice :
        public yarp::dev::DeviceDriver,
        public yarp::os::PeriodicThread
{
    static constexpr double period = 0.03;

public:
    /**
     * Constructor.
     */
    BrokenDevice() :
            PeriodicThread(period)
    {
    }

    bool close() override
    {
        pImg.close();
        PeriodicThread::stop();
        return true;

    }

    bool open(yarp::os::Searchable& /*config*/) override
    {
        return PeriodicThread::start();
    }

    //RateThread
    bool threadInit() override
    {
        return true;
    }

    void threadRelease() override
    {
    }

    void run() override
    {
        img = &pImg.prepare();
        img->resize(10, 10);
        pImg.write();
    }


private:
    yarp::sig::ImageOf<yarp::sig::PixelRgb>* img {nullptr};
    yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb>> pImg;

};

class TcpTestServer :
        public PeriodicThread
{
    static constexpr double period = 0.02;

public:
    TcpTestServer() : PeriodicThread(period)
    {
    }

private:
    BufferedPort<Bottle> tcpPort;
    bool threadInit() override
    {
        return tcpPort.open("/TcpTestServer");
    }

    void threadRelease() override
    {
        tcpPort.interrupt();
        tcpPort.close();
    }

    void run() override
    {
        Bottle& b = tcpPort.prepare();
        b.clear();
        b.addString("tcpTest");
        tcpPort.write();
    }
};

class TcpTestClient
{
    BufferedPort<Bottle> tcpPort;

public:
    TcpTestClient()
    {
        tcpPort.open("/TcpTestClien");
        yarp::os::Network::connect("/TcpTestServer", "/TcpTestClien", "tcp");
    }

    ~TcpTestClient()
    {
        tcpPort.interrupt();
        tcpPort.close();
    }
};

class ServiceProvider :
        public PortReader
{
public:
    bool read(ConnectionReader& connection) override
    {
        Bottle receive;
        receive.read(connection);
        receive.addInt32(5);
        ConnectionWriter *writer = connection.getWriter();
        if (writer!=nullptr) {
            receive.write(*writer);
        }
        return true;
    }
};

class ServiceTester :
        public Portable
{
public:
    Bottle send;
    Bottle receive;
    mutable int ct;

     bool write(ConnectionWriter& connection) const override
     {
        ct = 0;
        send.write(connection);
        connection.setReplyHandler(const_cast<ServiceTester&>(*this));
        return true;
    }

     bool read(ConnectionReader& connection) override
     {
        receive.read(connection);
        ct++;
        return true;
    }

    void finalCheck()
    {
        CHECK(receive.size() == send.size()+1); // size incremented
        CHECK(ct == 1); // just one read
    }
};

class DelegatedReader :
        public Thread
{
public:
    bool faithful;
    Port p;

    DelegatedReader(bool faithful = true)
    {
        p.open("/reader");
        this->faithful = faithful;
    }

    void run() override
    {
        for (int i=0; i<3; i++)
        {
            Bottle b;
            Bottle b2;
            p.read(b, true);
            b2.addInt32(b.get(0).asInt32()+1);
            if ((!faithful)&&i==1) {
                // no reply
            } else {
                p.reply(b2);
            }
        }
    }
};

class DelegatedWriter :
        public Thread
{
public:
    Port p;
    int total;

    DelegatedWriter()
    {
        p.open("/writer");
        Network::connect("/writer", "/reader");
    }

     void run() override
     {
        total = 0;
        for (int i=0; i<3; i++) {
            Bottle b;
            Bottle b2;
            b.addInt32(i);
            p.write(b, b2);
            total += b2.get(0).asInt32(); // should be i+1
        }
        // total should be 1+2+3 = 6
    }
};


class DelegatedCallback :
        public TypedReaderCallback<Bottle>
{
public:
    Bottle saved;
    Semaphore produce;

    DelegatedCallback() : produce(0) {}

    using TypedReaderCallback<Bottle>::onRead;
     void onRead(Bottle& bot) override {
        saved = bot;
        produce.post();
    }
};


class MyReport :
        public PortReport
{
public:
    int ct {0};
    int oct {0};
    int ict {0};

    void report(const PortInfo& info) override
    {
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

class WriteReader :
        public Thread
{
    static constexpr double delay = 0.25;

public:
    Port p;
    bool done;

    WriteReader()
    {
        done = false;
        p.open("/write");
    }

    void finish()
    {
        done = true;
        stop();
    }

     void run() override
     {
        while (!done) {
            Bottle msg("1 \"end of terminal\"");
            p.write(msg);
            Time::delay(delay);
        }
    }
};

class ServiceUser :
        public Thread
{
public:
    Port p;

    ServiceUser(const char *name)
    {
        p.open(name);
    }

     void run() override
    {
        Bottle cmd;
        Bottle reply;
        cmd.fromString("[add] 1 2");
        p.write(cmd, reply);
    }
};

class StreamUser :
        public Thread
{
public:
    Port p;

    StreamUser(const char *name)
    {
        p.setTimeout(2);
        p.open(name);
    }

     void run() override
    {
        Bottle cmd;
        cmd.fromString("[add] 1 2");
        p.write(cmd);
    }
};

class DataPort :
        public BufferedPort<Bottle>
{
public:
    int ct;

    DataPort()
    {
        ct = 0;
    }

    using BufferedPort<Bottle>::onRead;
     void onRead(Bottle& /*b*/) override
    {
        ct++;
    }
};


class TestModifyingCarrier :
        public yarp::os::ModifyingCarrier
{
    public:
    TestModifyingCarrier() = default;
    TestModifyingCarrier(const TestModifyingCarrier&) = delete;
    TestModifyingCarrier(TestModifyingCarrier&&) = delete;
    TestModifyingCarrier& operator=(const TestModifyingCarrier&) = delete;
    TestModifyingCarrier& operator=(TestModifyingCarrier&&) = delete;

    ~TestModifyingCarrier() override = default;

    Carrier *create() const override
    {
        return new TestModifyingCarrier();
    }

    std::string getName() const override
    {
        return "test_mod";
    }

    std::string toString() const override
    {
        return "test_mod_carrier";
    }

    bool acceptIncomingData(yarp::os::ConnectionReader& reader) override
    {
        ++cnt_accept_in;
        return yarp::os::ModifyingCarrier::acceptIncomingData(reader);
    }

    yarp::os::ConnectionReader& modifyIncomingData(yarp::os::ConnectionReader& reader) override
    {
        ++cnt_modify_in;
        return yarp::os::ModifyingCarrier::modifyIncomingData(reader);
    }

    bool acceptOutgoingData(const yarp::os::PortWriter& writer) override
    {
        ++cnt_accept_out;
        return yarp::os::ModifyingCarrier::acceptOutgoingData(writer);
    }

    const yarp::os::PortWriter& modifyOutgoingData(const yarp::os::PortWriter& writer) override
    {
        ++cnt_modify_out;
        return yarp::os::ModifyingCarrier::modifyOutgoingData(writer);
    }

    yarp::os::PortReader& modifyReply(yarp::os::PortReader& reader) override
    {
        ++cnt_modify_reply;
        return yarp::os::ModifyingCarrier::modifyReply(reader);
    }

    static size_t cnt_accept_in;
    static size_t cnt_modify_in;
    static size_t cnt_accept_out;
    static size_t cnt_modify_out;
    static size_t cnt_modify_reply;

    static void reset()
    {
        cnt_accept_in = 0;
        cnt_modify_in = 0;
        cnt_accept_out = 0;
        cnt_modify_out = 0;
        cnt_modify_reply = 0;
    }
};

size_t TestModifyingCarrier::cnt_accept_in = 0;
size_t TestModifyingCarrier::cnt_modify_in = 0;
size_t TestModifyingCarrier::cnt_accept_out = 0;
size_t TestModifyingCarrier::cnt_modify_out = 0;
size_t TestModifyingCarrier::cnt_modify_reply = 0;

class BottleWithAck :
        public yarp::os::Bottle
{
public:
    bool read(yarp::os::ConnectionReader &reader) override
    {
        bool ok = yarp::os::Bottle::read(reader);
        if (!ok) { return false; }
        yarp::os::Bottle reply;
        reply.addString("hey");
        return reply.write(*reader.getWriter());
    }
};

// A minimal PortWriter to be used when using Port::enableBackgroundWrite(), to
// block until the write operation is over
class cvNotifier :
        public yarp::os::PortWriter
{
private:
    mutable std::mutex mtx;
    mutable std::condition_variable cv;
    mutable bool done {false};

public:
    bool write(ConnectionWriter& writer) const override
    {
        return false;
    }

    void onCompletion() const override
    {
        mtx.lock();
        done = true;
        mtx.unlock();
        cv.notify_all();
    }

    void wait()
    {
        // Wait until write is finished
        std::unique_lock<std::mutex> lk(mtx);
        cv.wait(lk, [&]{ return done; });
    }
};

static int safePort()
{
    return Network::getDefaultPortRange() + 100;
}

TEST_CASE("os::PortTest", "[yarp::os]")
{
    NetworkBase::setLocalMode(true);

    yarp::dev::Drivers::factory().add(new yarp::dev::DriverCreatorOf<BrokenDevice>("brokenDevice",
                                                                                   "brokenDevice",
                                                                                   "BrokenDevice"));

    yarp::os::Carriers::addCarrierPrototype(new TestModifyingCarrier);

    constexpr double duration_100ms = 0.1;
    constexpr double duration_200ms = 0.2;
    constexpr double duration_250ms = 0.25;
    constexpr double duration_500ms = 0.5;

    SECTION("checking opening and closing ports")
    {
        Port out;
        Port in;

        in.open("/in");
        out.open(Contact("tcp", "", safePort()));

        CHECK(in.isOpen()); // /in port is open
        CHECK(out.isOpen()); // /out port is open

        Contact conIn = in.where();
        Contact conOut = out.where();

        CHECK(conIn.isValid()); // valid address for /in
        CHECK(conOut.isValid()); // valid address for /out

        out.addOutput(Contact("/in", "tcp"));
        //Time::delay(duration_200ms);

        CHECK(conIn.getName() == "/in"); // name is recorded
        CHECK(conOut.getName().find("/tmp") == 0); // name is created

        Bottle bot1;
        Bottle bot2;
        bot1.fromString("5 10 \"hello\"");
        out.enableBackgroundWrite(true);
        out.write(bot1);
        in.read(bot2);
        CHECK(bot1.get(0).asInt32() == 5); // check bot[0]
        CHECK(bot1.get(1).asInt32() == 10); // check bot[1]
        CHECK(bot1.get(2).asString() == "hello"); // check bot[2]

        while (out.isWriting()) {
            printf("Waiting...\n");
            Time::delay(duration_100ms);
        }

        bot1.fromString("18");
        out.write(bot1);
        in.read(bot2);
        CHECK(bot1.get(0).asInt32() == 18); // check one more send/receive

        in.close();
        out.close();
    }

#if defined(ENABLE_BROKEN_TESTS)
    SECTION("checking read buffering")
    {
        Bottle bot1;
        PortReaderBuffer<Bottle> buf;
        buf.setStrict(true);

        bot1.fromString("1 2 3");
        for (int i=0; i<10000; i++) {
            bot1.addInt32(i);
        }

        Port input, output;
        input.open("/in");
        output.open("/out");

        buf.setStrict();
        buf.attach(input);

        output.addOutput(Contact("/in", "udp"));
        //Time::delay(duration_200ms);

        INFO("writing...");
        output.write(bot1);
        output.write(bot1);
        output.write(bot1);
        INFO("reading...");
        Bottle *result = buf.read();

        for (int j=0; j<3; j++) {
            if (j!=0) {
                result = buf.read();
            }
            REQUIRE(result!=nullptr); // got something check
            CHECK(bot1.size() == result->size()); // size check
            INFO("size is in fact " << result->size());
        }

        output.close();
        input.close();
    }
#endif // ENABLE_BROKEN_TESTS

    SECTION("checking udp")
    {
        Bottle bot1;
        PortReaderBuffer<Bottle> buf;

        bot1.fromString("1 2 3");
        for (int i=0; i<10000; i++) {
            bot1.addInt32(i);
        }

        Port input;
        Port output;
        input.open("/in");
        output.open("/out");

        buf.setStrict();
        buf.attach(input);

        output.addOutput(Contact("/in", "udp"));
        //Time::delay(duration_200ms);

        INFO("writing/reading three times...");

        INFO("checking for whatever got through...");
        int ct = 0;
        while (buf.check()) {
            ct++;
            Bottle *result = buf.read();
            REQUIRE(result!=nullptr); // got something check
            CHECK(bot1.size() == result->size()); // size check
            INFO("size is in fact " << result->size());
        }
        if (ct==0) {
            INFO("NOTHING got through - possible but sad");
        }

        output.close();
        input.close();
    }

#if defined(ENABLE_BROKEN_TESTS)
    SECTION("checking heavy udp")
    {
        Bottle bot1;
        PortReaderBuffer<Bottle> buf;

        bot1.fromString("1 2 3");
        for (int i=0; i<100000; i++) {
            bot1.addInt32(i);
        }

        Port input, output;
        input.open("/in");
        output.open("/out");

        buf.setStrict();
        buf.attach(input);

        output.addOutput(Contact("/in", "udp"));
        //Time::delay(duration_200ms);

        for (int j=0; j<3; j++) {
            INFO("writing/reading three times...");
            output.write(bot1);
        }

        INFO("checking for whatever got through...");
        int ct = 0;
        while (buf.check()) {
            ct++;
            Bottle *result = buf.read();
            REQUIRE(result!=nullptr); // got something check
            CHECK(bot1.size() == result->size()); // size check
            INFO("size is in fact " << result->size());
        }
        if (ct==0) {
            INFO("NOTHING got through - possible but sad");
        }

        output.close();
        input.close();
    }
#endif // ENABLE_BROKEN_TESTS

    SECTION("checking paired send/receive")
    {
        PortReaderBuffer<PortablePair<Bottle, Bottle> > buf;

        Port input;
        Port output;
        input.open("/in");
        output.open("/out");

        buf.setStrict();
        buf.attach(input);

        output.addOutput(Contact("/in", "tcp"));
        //Time::delay(duration_200ms);

        PortablePair<Bottle, Bottle> bot1;
        bot1.head.fromString("1 2 3");
        bot1.body.fromString("4 5 6 7");

        INFO("writing...");
        output.write(bot1);
        bool ok = output.write(bot1);
        CHECK(ok); // output proceeding
        INFO("reading...");
        PortablePair<Bottle, Bottle> *result = buf.read();

        REQUIRE(result!=nullptr); // got something check
        CHECK(bot1.head.size() == result->head.size()); // head size check
        CHECK(bot1.body.size() == result->body.size()); // body size check

        output.close();
        input.close();
    }

    SECTION ("checking reply processing")
    {
        ServiceProvider provider;

        Port input;
        Port output;
        input.open("/in");
        output.open("/out");

        input.setReader(provider);

        output.addOutput(Contact("/in", "tcp"));
        Time::delay(duration_100ms);
        ServiceTester tester;
        output.write(tester);
        Time::delay(duration_100ms);
        tester.finalCheck();
        Time::delay(duration_100ms);
        output.close();
        input.close();
    }

    SECTION("test communication in background mode")
    {
        Port input;
        Port output;
        input.open("/in");
        output.open("/out");
        output.enableBackgroundWrite(true);

        BinPortable<int> bin;
        BinPortable<int> bout;
        bout.content() = 42;
        bin.content() = 20;

        output.addOutput("/in");

        INFO("writing...");
        output.write(bout);
        INFO("reading...");
        input.read(bin);

        CHECK(bout.content() == bin.content()); // successful transmit

        while (output.isWriting()) {
            INFO("waiting for port to stabilize");
            Time::delay(duration_200ms);
        }

        bout.content() = 88;

        INFO("writing...");
        output.write(bout);
        INFO("reading...");
        input.read(bin);

        CHECK(bout.content() == bin.content()); // successful transmit

        output.close();
        input.close();
    }

    SECTION("testing write buffering")
    {
        Port input;
        Port output;
        Port altInput;
        input.open("/in");
        altInput.open("/in2");
        output.open("/out");
        output.addOutput("/in");

        INFO("beginning...");

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
        CHECK(val1 == bin.content()); // successful transmit

        altInput.read(bin);
        CHECK(val2 == bin.content()); // successful transmit

        while (output.isWriting()) {
            INFO("waiting for port to stabilize");
            Time::delay(duration_200ms);
        }

        INFO("port stabilized");
        output.close();
        INFO("shut down output buffering");
    }

    SECTION("checking buffered port")
    {
        BufferedPort<BinPortable<int>> output;
        BufferedPort<BinPortable<int>> input;
        output.open("/out");
        input.open("/in");

        INFO("is write a no-op when no connection exists?...");
        BinPortable<int>& datum0 = output.prepare();
        datum0.content() = 123;
        INFO("writing...");
        output.write();

        output.addOutput("/in");
        INFO("now with a connection...");
        BinPortable<int>& datum = output.prepare();
        datum.content() = 999;
        INFO("writing...");
        output.write();
        INFO("reading...");
        BinPortable<int> *bin = input.read();
        CHECK(bin->content() == 999); // good send
    }

    SECTION("check that port close order doesn't matter (test 1)")
    {
        for (int i=0; i<4; i++) {
            // on OSX there is a problem only tickled upon repetition
            Port input;
            Port output;
            input.open("/in");
            output.open("/out");
            output.addOutput("/in");

            INFO("closing in");
            input.close();

            INFO("closing out");
            output.close();
        }
    }

    SECTION("check that port close order doesn't matter (test 2)")
    {
        for (int i=0; i<4; i++) {
            Port input;
            Port output;
            input.open("/in");
            output.open("/out");
            output.addOutput("/in");

            INFO("closing out");
            output.close();

            INFO("closing in");
            input.close();
        }
    }

    SECTION("check delegated read and reply")
    {
        DelegatedReader reader;
        DelegatedWriter writer;
        reader.start();
        writer.start();
        writer.stop();
        reader.stop();
        CHECK(writer.total == 6); // read/replies give right checksum
    }

    SECTION("check reader handler")
    {
        Port in;
        Port out;
        DelegatedCallback callback;
        out.open("/out");
        in.open("/in");
        Network::connect("/out", "/in");
        PortReaderBuffer<Bottle> reader;
        reader.setStrict();
        reader.attach(in);
        reader.useCallback(callback);
        Bottle src("10 10 20");
        out.write(src);
        callback.produce.wait();
        CHECK(callback.saved.size() == 3); // object came through
        reader.disableCallback();
        out.close();
        in.close();
    }

    SECTION("check reader handler, bufferedport style")
    {
        BufferedPort<Bottle> in;
        Port out;
        DelegatedCallback callback;
        in.setStrict();
        out.open("/out");
        in.open("/in");
        Network::connect("/out", "/in");
        in.useCallback(callback);
        Bottle src("10 10 20");
        out.write(src);
        callback.produce.wait();
        CHECK(callback.saved.size() == 3); // object came through
        in.disableCallback();
    }

    SECTION("check reader handler without open (test 1)")
    {
        Port in;
        DelegatedCallback callback;
        PortReaderBuffer<Bottle> reader;
        reader.setStrict();
        reader.attach(in);
        reader.useCallback(callback);
        reader.disableCallback();
        in.close();
    }

    SECTION("check reader handler without open (test 2)")
    {
        Port in;
        DelegatedCallback callback;
        PortReaderBuffer<Bottle> reader;
        reader.setStrict();
        reader.attach(in);
        reader.useCallback(callback);
        reader.disableCallback();
    }

    SECTION("check reader handler without open (test 3)")
    {
        Port in;
        DelegatedCallback callback;
        PortReaderBuffer<Bottle> reader;
        reader.setStrict();
        reader.attach(in);
        reader.useCallback(callback);
        in.close();
    }

    SECTION("check reader handler without open (test 4)")
    {
        INFO( "test 4");
        Port in;
        DelegatedCallback callback;
        PortReaderBuffer<Bottle> reader;
        reader.setStrict();
        reader.attach(in);
        reader.useCallback(callback);
    }

    SECTION("check strict writer")
    {
        BufferedPort<Bottle> in;
        BufferedPort<Bottle> out;
        in.setStrict();
        in.open("/in");
        out.open("/out");

        Network::connect("/out", "/in");

        Bottle& outBot1 = out.prepare();
        outBot1.fromString("hello world");
        printf("Writing bottle 1: %s\n", outBot1.toString().c_str());
        out.write(true);

        Bottle& outBot2 = out.prepare();
        outBot2.fromString("2 3 5 7 11");
        printf("Writing bottle 2: %s\n", outBot2.toString().c_str());
        out.write(true);

        Bottle *inBot1 = in.read();
        CHECK(inBot1!=nullptr); // got 1 of 2 items
        if (inBot1!=nullptr) {
            printf("Bottle 1 is: %s\n", inBot1->toString().c_str());
            CHECK(inBot1->size() == 2); // match for item 1
        }
        Bottle *inBot2 = in.read();
        CHECK(inBot2!=nullptr); // got 2 of 2 items
        if (inBot2!=nullptr) {
            printf("Bottle 2 is: %s\n", inBot2->toString().c_str());
            CHECK(inBot2->size() == 5); // match for item 1
        }
    }

    SECTION("check recent reader")
    {
        BufferedPort<Bottle> in;
        BufferedPort<Bottle> out;
        in.setStrict(false);
        in.open("/in");
        out.open("/out");

        Network::connect("/out", "/in");

        Bottle& outBot1 = out.prepare();
        outBot1.fromString("hello world");
        printf("Writing bottle 1: %s\n", outBot1.toString().c_str());
        out.write(true);

        Bottle& outBot2 = out.prepare();
        outBot2.fromString("2 3 5 7 11");
        printf("Writing bottle 2: %s\n", outBot2.toString().c_str());
        out.write(true);

        Time::delay(duration_250ms);

        Bottle *inBot2 = in.read();
        CHECK(inBot2!=nullptr); // got 2 of 2 items
        if (inBot2!=nullptr) {
            printf("Bottle 2 is: %s\n", inBot2->toString().c_str());
            CHECK(inBot2->size() == 5); // match for item 1
        }
    }

    SECTION("check that ports that receive data and do not read it can close")
    {
        BufferedPort<Bottle> sender;
        Port receiver;
        sender.open("/sender");
        receiver.open("/receiver");
        Network::connect("/sender", "/receiver");
        Time::delay(duration_250ms);
        Bottle& bot = sender.prepare();
        bot.clear();
        bot.addInt32(1);
        sender.write();
        Time::delay(duration_250ms);
        INFO("if this hangs, PortTest::testUnbufferedClose is unhappy");
        receiver.close();
        sender.close();
    }

    SECTION("check that opening-closing-opening etc is ok")
    {
        INFO("non-buffered port");
        Port p;
        p.open("/test1");
        p.open("/test2");
        p.open("/test3");
        p.close();
        p.open("/test4");
        p.close();
        INFO("buffered port:");
        BufferedPort<Bottle> p2;
        BufferedPort<Bottle> p3;
        p2.open("/test1");
        p2.open("/test2");
        p2.open("/in");
        p3.open("/out");
        Network::connect("/out", "/in");
        p3.prepare().fromString("10 20 30");
        p3.write();
        INFO("wait for input...");
        p2.read(true);
        INFO("... got it");
        p3.prepare().fromString("10 20 30");
        p3.write();
        p2.open("/test1");
        p3.open("/test2");
        Network::connect("/test2", "/test1");
        p3.prepare().fromString("10 20 30");
        p3.write();
        INFO("wait for input...");
        p2.read(true);
        INFO("... got it");

        INFO("fast loop on temporary port");
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
            NetworkBase::connect(p.getName(), pa.getName());
            Bottle b("10 20 30");
            p.write(b);
            pa.read(b);
            p.close();
        }
    }

#if defined(ENABLE_BROKEN_TESTS)
    SECTION("check that input/output counts are accurate...")
    {
        int top = 3;
        Port p[3];
        p[0].open("/a");
        p[1].open("/b");
        p[2].open("/c");
        for (int i=0; i<top; i++) {
            CHECK(p[i].getInputCount() == 0); // no input connections
            CHECK(p[i].getOutputCount() == 0); // no output connections
        }
        Network::connect("/a", "/b");
        Network::connect("/a", "/c");

        Network::sync("/a");
        Network::sync("/b");
        Network::sync("/c");

        CHECK(p[0].getInputCount() == 0); // input connections
        CHECK(p[0].getOutputCount() == 2); // output connections
        CHECK(p[1].getInputCount() == 1); // input connections
        CHECK(p[1].getOutputCount() == 0); // output connections
        CHECK(p[2].getInputCount() == 1); // input connections
        CHECK(p[2].getOutputCount() == 0); // output connections

        Network::disconnect("/a", "/c");

        Network::sync("/a");
        Network::sync("/b");
        Network::sync("/c");

        CHECK(p[0].getInputCount() == 0); // input connections
        CHECK(p[0].getOutputCount() == 1); // output connections
        CHECK(p[1].getInputCount() == 1); // input connections
        CHECK(p[1].getOutputCount() == 0); // output connections
        CHECK(p[2].getInputCount() == 0); // input connections
        CHECK(p[2].getOutputCount() == 0); // output connections

    }
#endif // ENABLE_BROKEN_TESTS

    SECTION("check that we survive if no reply() made when promised")
    {
        Port p1;
        DelegatedReader reader(false);
        reader.start();
        p1.open("/writer");
        Network::connect("/writer", "/reader");
        Network::sync("/writer");
        Network::sync("/reader");
        Bottle bsend;
        Bottle breply;
        bsend.addInt32(10);
        p1.write(bsend, breply);
        p1.write(bsend, breply);
        p1.write(bsend, breply);
        reader.stop();
    }

    SECTION("check port status report (test 1)")
    {
        Port p1;
        Port p2;
        p1.open("/foo");
        p2.open("/bar");
        Network::connect("/foo", "/bar");
        Network::sync("/foo");
        Network::sync("/bar");
        MyReport report;
        p1.getReport(report);
        CHECK(report.ct>0); // got some report
        CHECK(report.oct == 1); // exactly one output
        p1.close();
        p2.close();
    }

    SECTION("check port status report (test 2)")
    {
        Port p1;
        Port p2;
        MyReport report1;
        MyReport report2;
        p1.setReporter(report1);
        p2.setReporter(report2);
        p1.open("/foo");
        p2.open("/bar");
        Network::connect("/foo", "/bar");
        Network::sync("/foo");
        Network::sync("/bar");
        CHECK(report1.ct>0); // sender got report callback
        CHECK(report1.oct == 1); // exactly one output callback
        CHECK(report1.ict == 0); // exactly zero input callbacks
        CHECK(report2.ct>0); // receiver got report callback
        CHECK(report2.oct == 0); // exactly zero output callbacks
        CHECK(report2.ict == 1); // exactly one input callback
        p1.close();
        p2.close();
    }

    SECTION("check port status report with rpc client (test 1)")
    {
        RpcClient p1;
        RpcServer p2;
        p1.open("/foo");
        p2.open("/bar");
        Network::connect("/foo", "/bar");
        Network::sync("/foo");
        Network::sync("/bar");
        MyReport report;
        p1.getReport(report);
        CHECK(report.ct>0); // got some report
        CHECK(report.oct == 1); // exactly one output
        p1.close();
        p2.close();
    }

    SECTION("check port status report with rpc client (test 2)")
    {
        RpcClient p1;
        RpcServer p2;
        MyReport report1;
        MyReport report2;
        p1.setReporter(report1);
        p2.setReporter(report2);
        p1.open("/foo");
        p2.open("/bar");
        Network::connect("/foo", "/bar");
        Network::sync("/foo");
        Network::sync("/bar");
        CHECK(report1.ct>0); // sender got report callback
        CHECK(report1.oct == 1); // exactly one output callback
        CHECK(report1.ict == 0); // exactly zero input callbacks
        CHECK(report2.ct>0); // receiver got report callback
        CHECK(report2.oct == 0); // exactly zero output callbacks
        CHECK(report2.ict == 1); // exactly one input callback
        p1.close();
        p2.close();
    }

    SECTION("check port admin interface")
    {
        BufferedPort<Bottle> p1;
        Port p2;
        p1.open("/p1");
        p2.open("/p2");
        Network::connect("/p2", "/p1");
        Network::sync("/p1");
        Network::sync("/p2");

        Bottle cmd("[help]");
        Bottle reply;
        p2.setAdminMode();
        p2.write(cmd, reply);

        CHECK(reply.size()>=1); // got a reply

        p1.close();
        p2.close();
    }

    SECTION("checking acquire/release")
    {
        BufferedPort<Bottle> in;
        BufferedPort<Bottle> out;
        in.setStrict();
        out.setStrict();
        in.open("/in");
        out.open("/out");
        Network::connect("/out", "/in");

        out.prepare().fromString("1");
        out.write(true);

        Bottle *bot = in.read();
        CHECK(bot!=nullptr); // Inserted message received
        if (bot!=nullptr) {
            CHECK(bot->size() == 1); // right length
        }

        out.prepare().fromString("1 2");
        out.write(true);

        void *key = in.acquire();
        Bottle *bot2 = in.read();
        CHECK(bot2!=nullptr); // Inserted message received
        if (bot2!=nullptr) {
            CHECK(bot2->size() == 2); // right length
        }

        out.prepare().fromString("1 2 3");
        out.write(true);

        void *key2 = in.acquire();
        Bottle *bot3 = in.read();
        CHECK(bot3!=nullptr); // Inserted message received
        if (bot3!=nullptr) {
            CHECK(bot3->size() == 3); // right length
        }
        if (bot2!=nullptr) {
            CHECK(bot2->size() == 2); // original (2) still ok
        }
        if (bot!=nullptr) {
            CHECK(bot->size() == 1); // original (1) still ok
        }

        in.release(key);
        in.release(key2);
    }

    SECTION("check N second timeout")
    {
        Port a;
        Port b;
        bool ok = a.setTimeout(duration_500ms);
        CHECK(a.setTimeout(duration_500ms)); // set timeout
        if (!ok) {
            return;
        }
        a.open("/a");
        b.open("/b");
        NetworkBase::connect("/a", "/b");
        Bottle msg("hello");
        Bottle reply;
        ok = a.write(msg, reply);
        CHECK_FALSE(ok); // send failed correctly
    }

    SECTION("check yarp ... /write works")
    {
        WriteReader writer;
        writer.start();
        int argc = 2;
        const char *argv[] = {"...", "/write"};
        yarp::companion::impl::Companion::getInstance().cmdRead(argc, const_cast<char**>(argv));
        writer.finish();
    }

    SECTION("check behavior on missing slash")
    {
        Port p;
        const char *name = "something/without/slash";
        bool opened = p.open(name);
        CHECK_FALSE(opened); // correctly rejected port
    }

    SECTION("checking interrupt")
    {
        PortReaderBuffer<PortablePair<Bottle, Bottle> > buf;

        Port input;
        Port output;
        input.open("/in");
        output.open("/out");

        buf.setStrict();
        buf.attach(input);

        output.addOutput(Contact("/in", "tcp"));

        PortablePair<Bottle, Bottle> bot1;
        bot1.head.fromString("1 2 3");
        bot1.body.fromString("4 5 6 7");

        INFO("interrupting...");
        output.interrupt();
        INFO("writing...");
        bool ok = output.write(bot1);
        CHECK_FALSE(ok); // output rejected correctly
        output.resume();
        ok = output.write(bot1);
        CHECK(ok); // output goes through after resume

        output.close();
        input.close();
    }

    SECTION("checking interrupt for BufferedPort")
    {
        BufferedPort<Bottle> input;
        BufferedPort<Bottle> output;
        input.open("/in");
        output.open("/out");
        CHECK(yarp::os::Network::connect("/out", "/in")); // checking connection

        Bottle& botOut1 = output.prepare();
        botOut1.clear();
        botOut1.addInt32(0);
        output.writeStrict();

        yarp::os::Time::delay(duration_100ms);

        Bottle *botIn1 = input.read();
        CHECK(botIn1!=nullptr); // Inserted message received
        if (botIn1)
        {
            CHECK(botIn1->get(0).asInt32() == 0); // Checking data validity
        }

        INFO("interrupting...");
        output.interrupt();
        INFO("resuming...");
        output.resume();

        Bottle& botOut2 = output.prepare();
        botOut2.clear();
        botOut2.addInt32(1);
        output.writeStrict();

        yarp::os::Time::delay(duration_100ms);

        Bottle *botIn2 = input.read();
        CHECK(botIn2!=nullptr); // Inserted message received
        CHECK(botIn2->get(0).asInt32() == 1); // Checking data validity

        output.close();
        input.close();
    }

    SECTION("checking interrupt on input side")
    {
        PortReaderBuffer<Bottle> buf;
        buf.setStrict(true);

        Port input;
        Port output;
        input.open("/in");
        output.open("/out");

        buf.setStrict();
        buf.attach(input);

        output.addOutput(Contact("/in", "tcp"));

        Bottle bot1;
        bot1.fromString("1 2 3");

        output.write(bot1);
        for (int i=0; i<20 && buf.getPendingReads()<1; i++) {
            Time::delay(duration_100ms);
        }
        CHECK(buf.getPendingReads() == 1); // first msg came through
        Bottle *bot2 = buf.read();
        yAssert(bot2);
        CHECK(bot2->size() == 3); // data looks ok

        bot1.addInt32(4);

        INFO("interrupting...");
        input.interrupt();

        output.write(bot1);
        for (int i=0; i<10 && buf.getPendingReads()<1; i++) {
            Time::delay(duration_100ms);
        }
        CHECK(buf.getPendingReads() == 0); // msg after interrupt ignored

        bot1.addInt32(5);

        input.resume();
        output.write(bot1);
        for (int i=0; i<20 && buf.getPendingReads()<1; i++) {
            Time::delay(duration_100ms);
        }
        CHECK(buf.getPendingReads() == 1); // next msg came through
        bot2 = buf.read();
        yAssert(bot2);
        CHECK(bot2->size() == 5); // data looks ok

        output.close();
        input.close();
    }

    SECTION("checking interrupt on input side without buffering")
    {
        Port input;
        Port output;
        input.open("/in");
        output.enableBackgroundWrite(true);
        output.open("/out");

        output.addOutput(Contact("/in", "tcp"));

        Bottle bot1;
        Bottle bot2;
        bot1.fromString("1 2 3");

        output.write(bot1);
        bool ok = input.read(bot2);
        CHECK(ok); // first msg came through
        CHECK(bot2.size() == 3); // data looks ok

        bot1.addInt32(4);

        INFO("interrupting...");
        input.interrupt();

        output.write(bot1);
        CHECK_FALSE(input.read(bot2)); // msg after interrupt ignored

        Time::delay(1);

        bot1.addInt32(5);

        INFO("resuming");
        input.resume();
        output.write(bot1);
        ok = input.read(bot2);
        CHECK(ok); // next msg came through
        CHECK(bot2.size() == 5); // data looks ok

        output.close();
        input.close();
    }

    SECTION("checking interrupt for a port with pending reply")
    {
        PortReaderBuffer<PortablePair<Bottle, Bottle> > buf;

        ServiceUser output("/out");
        Port input;
        input.open("/in");
        Network::connect(output.p.getName(), input.getName());
        output.start();
        Bottle cmd;
        Bottle reply;
        input.read(cmd, true);
        reply.addInt32(cmd.get(1).asInt32()+cmd.get(2).asInt32());
        CHECK(cmd.toString() == "[add] 1 2"); // cmd received ok
        input.interrupt();
        input.reply(reply);
        input.close();
        output.stop();
        output.p.close();
        INFO("successfully closed");
    }

    SECTION("checking interrupt with bad reader")
    {
        StreamUser output("/out");
        Port input;
        input.open("/in");

        Network::connect("/out", "/in");

        output.start();
        Time::delay(2);

        INFO("interrupting output...");
        output.p.interrupt();
        output.stop();
        INFO("made it through");

        input.close();
    }

    SECTION("checking opening/closing/reopening ports")
    {
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
            Time::delay(duration_100ms);
        }

        CHECK_FALSE(port.isClosed()); // port tagged as open
        port.close();
        CHECK(port.isClosed()); // port tagged as closed
        port.open("/test");
        CHECK_FALSE(port.isClosed()); // port tagged as open

        Bottle *bot = port.read(false);
        CHECK(bot==nullptr); // reader correctly reset

        Network::connect("/test2", "/test");
        Network::sync("/test");
        Network::sync("/test2");
        port2.prepare().fromString("2 msg");
        port2.write();

        bot = port.read();
        REQUIRE(bot != nullptr); // reader working
        CHECK(bot->get(0).asInt32() == 2); // reader read correct message
    }

    SECTION("checking BufferedPort callback")
    {
        DataPort pin;
        pin.useCallback();
        BufferedPort<Bottle> pout;
        pout.open("/out");
        pin.open("/in");
        Network::connect("/out", "/in");
        Network::sync("/out");
        Network::sync("/in");
        Bottle& msg = pout.prepare();
        msg.clear();
        msg.addInt32(42);
        pout.write();
        pout.waitForWrite();
        pout.close();
        pin.close();
        CHECK(pin.ct == 1); // callback happened
    }

    SECTION("checking BufferedPort callback without open (test 1)")
    {
        INFO( "");
        DataPort pin;
        pin.useCallback();
        pin.disableCallback();
    }

    SECTION("checking BufferedPort callback without open (test 2)")
    {
        DataPort pin;
        pin.useCallback();
        pin.disableCallback();
        pin.close();
    }

    SECTION("checking BufferedPort callback without open (test 3)")
    {
        DataPort pin;
        pin.useCallback();
    }

    SECTION("checking BufferedPort callback without open (test 4)")
    {
        DataPort pin;
        pin.useCallback();
        pin.close();
    }

    SECTION("checking user-level admin message reads")
    {
        Port pin;
        ServiceProvider admin_reader;
        pin.setAdminReader(admin_reader);
        pin.open("/in");
        Port pout;
        pout.setAdminMode();
        pout.open("/out");
        Network::connect("/out", "/in");
        Bottle cmd("hello");
        Bottle reply;
        pout.write(cmd, reply);
        CHECK(reply.get(1).asInt32() == 5); // admin_reader was called
        cmd.fromString("[ver]");
        pout.write(cmd, reply);
        CHECK(reply.size()>=4); // yarp commands still work
    }

    SECTION("checking callback locking")
    {
        Port pin;
        Port pout;
        Bottle data;
        pin.setCallbackLock();
        pin.setReader(data);
        pout.enableBackgroundWrite(true);
        pin.open("/in");
        pout.open("/out");
        Network::connect("/out", "/in");
        Bottle cmd("hello");
        pin.lockCallback();
        pout.write(cmd);
        Time::delay(duration_250ms);
        CHECK(data.size() == 0); // data does not arrive too early
        pin.unlockCallback();
        while (pout.isWriting()) {
            INFO("waiting for port to stabilize");
            Time::delay(duration_200ms);
        }
        CHECK(data.size() == 1); // data does eventually arrive
        pin.close();
        pout.close();
    }

#if defined(ENABLE_BROKEN_TESTS)
    SECTION("checking tcp")
    {
        for(int i = 0; i < 50; i++)
        {
            TcpTestServer server;
            server.start();

            TcpTestClient* client = new TcpTestClient();

            delete client;

            server.stop();
        }
    }
#endif // ENABLE_BROKEN_TESTS

    SECTION("testing the deadlock when you close a device(PeriodicThread) after the prepare of a closed port")
    {
        yarp::dev::PolyDriver p;
        Property prop;
        prop.put("device", "brokenDevice");
        CHECK(p.open(prop)); // Opening the broken_device
        CHECK(p.close()); // Closing the broken_device
    }

    SECTION("testing lockup if resume is called when not interrupted")
    {
        Port p;
        CHECK(p.open("/test")); // Checking the open of the port
        p.interrupt();
        p.resume();
        p.resume(); //should not lockup here
        p.close();
    }

    SECTION("Check ModifyingCarrier on output (w/o reply)")
    {
        Port out;
        Port in;
        CHECK(out.open("/out"));
        CHECK(in.open("/in"));

        Network::sync("/out");
        Network::sync("/in");

        TestModifyingCarrier::reset();
        REQUIRE(Network::connect(out.getName(), in.getName(), "tcp+send.test_mod"));

        Bottle cmd_out("hello");
        Bottle cmd_in;

        out.enableBackgroundWrite(true);
        cvNotifier nf;

        out.write(cmd_out, &nf);
        in.read(cmd_in);

        nf.wait();

        CHECK(TestModifyingCarrier::cnt_accept_in == 0);
        CHECK(TestModifyingCarrier::cnt_modify_in == 0);
        CHECK(TestModifyingCarrier::cnt_accept_out == 1);
        CHECK(TestModifyingCarrier::cnt_modify_out == 1);
        CHECK(TestModifyingCarrier::cnt_modify_reply == 0);
    }

    SECTION("Check ModifyingCarrier on output (w/ reply)")
    {
        Port out;
        Port in;
        CHECK(out.open("/out"));
        CHECK(in.open("/in"));

        Network::sync("/out");
        Network::sync("/in");

        TestModifyingCarrier::reset();
        REQUIRE(Network::connect(out.getName(), in.getName(), "tcp+send.test_mod"));

        Bottle cmd_out("hello");
        Bottle cmd_in;
        Bottle reply_in;
        Bottle reply_out("hey");

        out.enableBackgroundWrite(true);
        cvNotifier nf;

        out.write(cmd_out, reply_in, &nf);
        in.read(cmd_in, true);
        in.reply(reply_out);

        nf.wait();

        CHECK(reply_in.toString() == "hey");


        CHECK(TestModifyingCarrier::cnt_accept_in == 0);
        CHECK(TestModifyingCarrier::cnt_modify_in == 0);
        CHECK(TestModifyingCarrier::cnt_accept_out == 1);
        CHECK(TestModifyingCarrier::cnt_modify_out == 1);
        CHECK(TestModifyingCarrier::cnt_modify_reply == 1);
    }

    SECTION("Check ModifyingCarrier on output (w/ reply in portable)")
    {
        Port out;

        BottleWithAck bwa;
        Port in;
        in.setReader(bwa);

        CHECK(out.open("/out"));
        CHECK(in.open("/in"));

        Network::sync("/out");
        Network::sync("/in");

        TestModifyingCarrier::reset();
        REQUIRE(Network::connect(out.getName(), in.getName(), "tcp+send.test_mod"));

        Bottle cmd_out("hello");
        Bottle cmd_in;
        Bottle reply_in;
        out.write(cmd_out, reply_in);

        CHECK(reply_in.toString() == "hey");

        CHECK(TestModifyingCarrier::cnt_accept_in == 0);
        CHECK(TestModifyingCarrier::cnt_modify_in == 0);
        CHECK(TestModifyingCarrier::cnt_accept_out == 1);
        CHECK(TestModifyingCarrier::cnt_modify_out == 1);
        CHECK(TestModifyingCarrier::cnt_modify_reply == 1);
    }

    SECTION("Check ModifyingCarrier on input (w/o reply)")
    {
        Port out;
        Port in;
        CHECK(out.open("/out"));
        CHECK(in.open("/in"));

        Network::sync("/out");
        Network::sync("/in");

        TestModifyingCarrier::reset();
        REQUIRE(Network::connect(out.getName(), in.getName(), "tcp+recv.test_mod"));

        Bottle cmd_out("hello");
        Bottle cmd_in;

        out.enableBackgroundWrite(true);
        cvNotifier nf;

        out.write(cmd_out, &nf);
        in.read(cmd_in);

        nf.wait();

        CHECK(TestModifyingCarrier::cnt_accept_in == 1);
        CHECK(TestModifyingCarrier::cnt_modify_in == 1);
        CHECK(TestModifyingCarrier::cnt_accept_out == 0);
        CHECK(TestModifyingCarrier::cnt_modify_out == 0);
        CHECK(TestModifyingCarrier::cnt_modify_reply == 0);
    }

    SECTION("Check ModifyingCarrier on input (w/ reply)")
    {
        Port out;
        Port in;
        CHECK(out.open("/out"));
        CHECK(in.open("/in"));

        Network::sync("/out");
        Network::sync("/in");

        TestModifyingCarrier::reset();
        REQUIRE(Network::connect(out.getName(), in.getName(), "tcp+recv.test_mod"));

        Bottle cmd_out("hello");
        Bottle cmd_in;
        Bottle reply_in;
        Bottle reply_out("hey");

        out.enableBackgroundWrite(true);
        cvNotifier nf;

        out.write(cmd_out, reply_in, &nf);
        in.read(cmd_in, true);
        in.reply(reply_out);

        nf.wait();

        CHECK(reply_in.toString() == "hey");

        CHECK(TestModifyingCarrier::cnt_accept_in == 1);
        CHECK(TestModifyingCarrier::cnt_modify_in == 1);
        CHECK(TestModifyingCarrier::cnt_accept_out == 0);
        CHECK(TestModifyingCarrier::cnt_modify_out == 0);
        CHECK(TestModifyingCarrier::cnt_modify_reply == 0);
    }

    SECTION("Check ModifyingCarrier on output (w/ reply in portable)")
    {
        Port out;

        BottleWithAck bwa;
        Port in;
        in.setReader(bwa);

        CHECK(out.open("/out"));
        CHECK(in.open("/in"));

        Network::sync("/out");
        Network::sync("/in");

        TestModifyingCarrier::reset();
        REQUIRE(Network::connect(out.getName(), in.getName(), "tcp+recv.test_mod"));

        Bottle cmd_out("hello");
        Bottle cmd_in;
        Bottle reply_in;

        out.write(cmd_out, reply_in);

        CHECK(reply_in.toString() == "hey");

        CHECK(TestModifyingCarrier::cnt_accept_in == 1);
        CHECK(TestModifyingCarrier::cnt_modify_in == 1);
        CHECK(TestModifyingCarrier::cnt_accept_out == 0);
        CHECK(TestModifyingCarrier::cnt_modify_out == 0);
        CHECK(TestModifyingCarrier::cnt_modify_reply == 0);
    }

    SECTION("Check ModifyingCarrier on both input and output (w/o reply)")
    {
        Port out;
        Port in;
        CHECK(out.open("/out"));
        CHECK(in.open("/in"));

        Network::sync("/out");
        Network::sync("/in");

        TestModifyingCarrier::reset();
        REQUIRE(Network::connect(out.getName(), in.getName(), "tcp+send.test_mod+recv.test_mod"));

        Bottle cmd_out("hello");
        Bottle cmd_in;
        out.enableBackgroundWrite(true);
        cvNotifier nf;

        out.write(cmd_out, &nf);
        in.read(cmd_in);

        nf.wait();

        CHECK(TestModifyingCarrier::cnt_accept_in == 1);
        CHECK(TestModifyingCarrier::cnt_modify_in == 1);
        CHECK(TestModifyingCarrier::cnt_accept_out == 1);
        CHECK(TestModifyingCarrier::cnt_modify_out == 1);
        CHECK(TestModifyingCarrier::cnt_modify_reply == 0);
    }

    SECTION("Check ModifyingCarrier on both input and output (w/ reply)")
    {
        Port out;
        Port in;
        CHECK(out.open("/out"));
        CHECK(in.open("/in"));

        Network::sync("/out");
        Network::sync("/in");

        TestModifyingCarrier::reset();
        REQUIRE(Network::connect(out.getName(), in.getName(), "tcp+send.test_mod+recv.test_mod"));

        Bottle cmd_out("hello");
        Bottle cmd_in;
        Bottle reply_in;
        Bottle reply_out("hey");

        out.enableBackgroundWrite(true);
        cvNotifier nf;

        out.write(cmd_out, reply_in, &nf);
        in.read(cmd_in, true);
        in.reply(reply_out);

        nf.wait();

        CHECK(reply_in.toString() == "hey");

        CHECK(TestModifyingCarrier::cnt_accept_in == 1);
        CHECK(TestModifyingCarrier::cnt_modify_in == 1);
        CHECK(TestModifyingCarrier::cnt_accept_out == 1);
        CHECK(TestModifyingCarrier::cnt_modify_out == 1);
        CHECK(TestModifyingCarrier::cnt_modify_reply == 1);
    }

    SECTION("Check ModifyingCarrier on output (w/ reply in portable)")
    {
        Port out;

        BottleWithAck bwa;
        Port in;
        in.setReader(bwa);

        CHECK(out.open("/out"));
        CHECK(in.open("/in"));

        Network::sync("/out");
        Network::sync("/in");

        TestModifyingCarrier::reset();
        REQUIRE(Network::connect(out.getName(), in.getName(), "tcp+send.test_mod+recv.test_mod"));

        Bottle cmd_out("hello");
        Bottle cmd_in;
        Bottle reply_in;
        out.write(cmd_out, reply_in);

        CHECK(reply_in.toString() == "hey");

        CHECK(TestModifyingCarrier::cnt_accept_in == 1);
        CHECK(TestModifyingCarrier::cnt_modify_in == 1);
        CHECK(TestModifyingCarrier::cnt_accept_out == 1);
        CHECK(TestModifyingCarrier::cnt_modify_out == 1);
        CHECK(TestModifyingCarrier::cnt_modify_reply == 1);
    }

    NetworkBase::setLocalMode(false);
}

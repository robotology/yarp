/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/all.h>
#include <Demo.h>
#include <DemoStructList.h>
#include <DemoStructMap.h>
#include <DemoStructExt.h>
#include <SurfaceMeshWithBoundingBox.h>
#include <Wrapping.h>
#include <TestAnnotatedTypes.h>
#include <TestSomeMoreTypes.h>
#if defined(THRIFT_INCLUDE_PREFIX) && defined(THRIFT_NO_NAMESPACE_PREFIX)
# include <sub/directory/ClockServer.h>
#elif defined(THRIFT_INCLUDE_PREFIX)
# include <sub/directory/testing/ClockServer.h>
#elif defined(THRIFT_NO_NAMESPACE_PREFIX)
# include <ClockServer.h>
#else
# include <testing/ClockServer.h>
#endif
#include <Settings.h>

#define CATCH_CONFIG_MAIN
#if defined(USE_SYSTEM_CATCH)
#include <catch.hpp>
#else
#include "catch.hpp"
#endif


using namespace yarp::os;
using namespace yarp::os::impl;


class Server : public Demo
{
private:
    bool running;
    bool closing;
public:
    Server() {
        running = false;
        closing = false;
    }

    int32_t get_answer() override {
        return 42;
    }

    int32_t add_one(const int32_t x) override {
        INFO("adding 1 to " << x);
        return x+1;
    }

    int32_t add_pair(const int32_t x, const int32_t y) override {
        INFO("adding " << x << " and " << y);
        return x+y;
    }

    void test_void(const int32_t x) override {
        INFO("test void with " << x);
    }

    void test_1way(const int32_t x) override {
        INFO("test oneway with " << x);
    }

    bool test_defaults(const int32_t x) override {
        INFO("test defaults with " << x);
        return (x==42);
    }

    std::vector<DemoEnum> test_enum_vector(const std::vector<DemoEnum> & x) override {
        INFO("test_enum_vector");
        std::vector<DemoEnum> result = x;
        result.push_back(ENUM1);
        return result;
    }

    int32_t test_partial(const int32_t x,
                                 const std::vector<int32_t> & lst,
                                 const int32_t y) override {
        INFO("test_partial with " << x << " and " << y);
        YARP_UNUSED(lst);
        return x+y;
    }

    int32_t test_tail_defaults(const DemoEnum x) override {
        if (x==ENUM1) {
            return 42;
        }
        return 999;
    }

    int32_t test_longer_tail_defaults(const int32_t ignore, const DemoEnum _enum, const int32_t _int, const std::string& _string) override {
        YARP_UNUSED(ignore);
        if (_enum==ENUM2 && _int==42 && _string=="Space Monkey from the Planet: Space") {
            return 999;
        }
        return _int;
    }



    void do_start_a_service() override {
        running = true;
        while (!closing) {
            INFO("Operating...");
            Time::delay(0.1);
        }
        running = false;
    }

    bool do_check_for_service() override {
        return running;
    }

    void do_stop_a_service() override {
        closing = true;
        while (running) {
            Time::delay(0.1);
        }
        closing = false;
    }

    DemoStructMap get_demo_map_struct() override {
        DemoStructMap demoStructMap;
        demoStructMap.mapValues["map_0"] = {0, 10};
        demoStructMap.mapValues["map_1"] = {1, 11};
        demoStructMap.mapValues["map_2"] = {2, 12};
        demoStructMap.mapValues["map_3"] = {3, 13};
        return demoStructMap;
    }

    bool this_is_a_const_method() const {
        return true;
    }
};


class BrokenServer : public Demo
{
public:
    int32_t get_answer() override {
        return 42;
    }
};


class WrappingServer : public Wrapping
{
public:
    int32_t check(const yarp::os::Value& param) override {
        if (param.isInt32()) return param.asInt32()+1;
        if (param.asString()=="6*7") return 42;
        return 9;
    }

    Bottle getBottle() override {
        Bottle b("this is a test (bottle)");
        return b;
    }
};

class DemoStructCallbacks : public DemoStruct::Editor
{
public:
    int wsx, dsx, wsy, dsy;

    DemoStructCallbacks() {
        wsx = dsx = wsy = dsy = 0;
    }

    bool will_set_x() override {
        INFO("will_set_x called, x is " << get_x());
        wsx = get_x();
        return true;
    }
    bool will_set_y() override {
        INFO("will_set_y called, y is " << get_y());
        wsy = get_y();
        return true;
    }
    bool did_set_x() override {
        INFO("did_set_x called, x is " << get_x());
        dsx = get_x();
        return true;
    }
    bool did_set_y() override {
        INFO("did_set_y called, y is " << get_y());
        dsy = get_y();
        return true;
    }
};

class ClientPeek : public PortReader
{
public:
    bool read(ConnectionReader& con) override {
        Bottle bot;
        bot.read(con);
        INFO("Got " << bot.toString());
        return true;
    }
};

class SettingsReceiver : public Settings::Editor
{
public:
    bool called_will_set_id;
    bool called_did_set_id;

    SettingsReceiver() {
        reset();
    }

    void reset() {
        called_will_set_id = false;
        called_did_set_id = false;
    }

    bool will_set_id() override {
        called_will_set_id = true;
        return false;
    }

    bool did_set_id() override {
        called_did_set_id = true;
        return false;
    }
};



TEST_CASE("IdlThriftTest", "[yarp::idl::thrift]")
{
    Network yarp;
    yarp.setLocalMode(true);

    SECTION("add one")
    {
        ClientPeek client_peek;
        Demo client;
        client.yarp().attachAsClient(client_peek);
        client.add_one(14);

        Server server;
        Bottle bot("[add] [one] 14");
        DummyConnector con;
        bot.write(con.getWriter());
        server.read(con.getReader());
        bot.read(con.getReader());
        INFO("Result is " << bot.toString());
        CHECK(bot.get(0).asInt32() == 15);

        bot.fromString("[add] [one] 15");
        DummyConnector con2;
        bot.write(con2.getWriter());
        server.read(con2.getReader());
        bot.read(con2.getReader());
        INFO("Result is " << bot.toString());

        CHECK(bot.get(0).asInt32() == 16);
    }

    SECTION("test void")
    {
        ClientPeek client_peek;
        Demo client;
        client.yarp().attachAsClient(client_peek);
        client.test_void(14);
        client.test_1way(14);

        Server server;
        Bottle bot("[test] [void] 14");
        DummyConnector con;
        bot.write(con.getWriter());
        server.read(con.getReader());
        bot.read(con.getReader());
        INFO("Result is " << bot.toString() << " (should be blank)");

        CHECK(bot.size() == 0);
        CHECK(!bot.isNull());
    }

    SECTION("test live")
    {
        Demo client;
        Server server;

        RpcClient client_port;
        RpcServer server_port;
        REQUIRE(client_port.open("/client"));
        REQUIRE(server_port.open("/server"));
        REQUIRE(yarp.connect(client_port.getName(), server_port.getName()));

        int x = 0;
        client.yarp().attachAsClient(client_port);
        server.yarp().attachAsServer(server_port);
        x = client.add_one(99);
        INFO("Result " << x);
        client.test_void(200);
        client.test_void(201);
        x = client.add_one(100);
        INFO("Result " << x);
        client.test_1way(200);
        client.test_1way(201);
        x = client.add_one(101);
        INFO("Result " << x);

        CHECK(x == 102);
    }

    SECTION("test live rpc")
    {
        Demo client;
        Server server;

        RpcClient client_port;
        RpcServer server_port;
        REQUIRE(client_port.open("/client"));
        REQUIRE(server_port.open("/server"));
        REQUIRE(yarp.connect(client_port.getName(), server_port.getName()));

        client.yarp().attachAsClient(client_port);
        server.yarp().attachAsServer(server_port);

        // Check regular RPC works, even with oneways

        Bottle cmd, reply;

        cmd.fromString("[add] [one] 5");
        client_port.write(cmd,reply);
        INFO("cmd " << cmd.toString() << " reply " << reply.toString());

        CHECK(reply.get(0).asInt32() == 6);

        cmd.fromString("[test] [void] 5");
        client_port.write(cmd,reply);
        INFO("cmd " << cmd.toString() << " reply " << reply.toString());

        cmd.fromString("[add] [one] 6");
        client_port.write(cmd,reply);
        INFO("cmd " << cmd.toString() << " reply " << reply.toString());

        CHECK(reply.get(0).asInt32() == 7);

        cmd.fromString("[test] [1way] 5");
        client_port.write(cmd,reply);
        INFO("cmd " << cmd.toString() << " reply " << reply.toString());

        cmd.fromString("[add] [one] 7");
        client_port.write(cmd,reply);
        INFO("cmd " << cmd.toString() << " reply " << reply.toString());

        CHECK(reply.get(0).asInt32() == 8);
    }

    SECTION("test enums")
    {
        Demo client;
        Server server;

        RpcClient client_port;
        RpcServer server_port;
        REQUIRE(client_port.open("/client"));
        REQUIRE(server_port.open("/server"));
        REQUIRE(yarp.connect(client_port.getName(), server_port.getName()));
        client.yarp().attachAsClient(client_port);
        server.yarp().attachAsServer(server_port);

        std::vector<DemoEnum> lst1;
        lst1.push_back(ENUM1);
        lst1.push_back(ENUM2);
        std::vector<DemoEnum> lst2 = client.test_enum_vector(lst1);
        INFO("lst1 " << lst1.size() << " lst2 " << lst2.size());

        CHECK(lst2.size() == 3);
        CHECK(lst1[0] == lst2[0]);
        CHECK(lst1[1] == lst2[1]);
    }

    SECTION("test defaults")
    {
        Demo client;
        Server server;

        RpcClient client_port;
        RpcServer server_port;
        client_port.open("/client");
        REQUIRE(server_port.open("/server"));
        REQUIRE(yarp.connect(client_port.getName(), server_port.getName()));
        client.yarp().attachAsClient(client_port);
        server.yarp().attachAsServer(server_port);

        CHECK(client.test_defaults());
        CHECK_FALSE(client.test_defaults(14));
    }

    SECTION("test partial")
    {
        Server server;

        RpcClient client_port;
        RpcServer server_port;
        CHECK(client_port.open("/client"));
        REQUIRE(server_port.open("/server"));
        REQUIRE(yarp.connect(client_port.getName(), server_port.getName()));
        server.yarp().attachAsServer(server_port);

        Bottle msg, reply;
        msg.fromString("add pair 4 3");
        client_port.write(msg,reply);
        INFO(msg.toString() << " -> " << reply.toString());

        CHECK(reply.get(0).asInt32() == 7);

        msg.fromString("add pair 4");
        client_port.write(msg,reply);
        INFO("(incomplete) " << msg.toString() << " -> " << reply.toString());
        CHECK(reply.get(0).asVocab32() == yarp::os::createVocab32('f','a','i','l'));

        msg.fromString("add pair");
        reply.fromString("0");
        client_port.write(msg,reply);
        INFO("(incomplete) " << msg.toString() << " -> " << reply.toString());
        CHECK(reply.get(0).asVocab32() == yarp::os::createVocab32('f','a','i','l'));

        msg.fromString("add");
        reply.fromString("0");
        client_port.write(msg,reply);
        INFO("(incomplete) " << msg.toString() << " -> " << reply.toString());
        CHECK(reply.get(0).asVocab32() == yarp::os::createVocab32('f','a','i','l'));

        msg.fromString("");
        reply.fromString("0");
        client_port.write(msg,reply);
        INFO("(incomplete) " << msg.toString() << " -> " << reply.toString());
        CHECK(reply.get(0).asVocab32() == yarp::os::createVocab32('f','a','i','l'));

        msg.fromString("add pair 10 20");
        reply.fromString("0");
        client_port.write(msg,reply);
        INFO(msg.toString() << " -> " << reply.toString());
        CHECK(reply.get(0).asInt32() == 30);

        msg.fromString("test partial 10 (40 50 60) 5");
        reply.fromString("0");
        client_port.write(msg,reply);
        INFO(msg.toString() << " -> " << reply.toString());
        CHECK(reply.get(0).asInt32() == 15);

        msg.fromString("test partial 10 (40 50)");
        reply.fromString("0");
        client_port.write(msg,reply);
        INFO(msg.toString() << " -> " << reply.toString());
        CHECK(reply.get(0).asVocab32() == yarp::os::createVocab32('f','a','i','l'));
    }

    SECTION("test defaults with rpc")
    {
        Server server;

        RpcClient client_port;
        RpcServer server_port;
        REQUIRE(client_port.open("/client"));
        REQUIRE(server_port.open("/server"));
        REQUIRE(yarp.connect(client_port.getName(), server_port.getName()));
        server.yarp().attachAsServer(server_port);

        Bottle msg, reply;
        msg.fromString("test_tail_defaults");
        client_port.write(msg,reply);
        INFO(msg.toString() << " -> " << reply.toString());
        CHECK(reply.get(0).asInt32() == 42);

        msg.fromString("test_tail_defaults 55");
        client_port.write(msg,reply);
        INFO(msg.toString() << " -> " << reply.toString());
        CHECK(reply.get(0).asInt32() == 999);

        msg.fromString("test longer tail defaults");
        client_port.write(msg,reply);
        INFO(msg.toString() << " -> " << reply.toString());
        CHECK(reply.get(0).asVocab32() == yarp::os::createVocab32('f','a','i','l'));

        msg.fromString("test longer tail defaults 888");
        client_port.write(msg,reply);
        INFO(msg.toString() << " -> " << reply.toString());
        CHECK(reply.get(0).asInt32() == 999);;

        msg.fromString("test longer tail defaults 888 ENUM2 47");
        client_port.write(msg,reply);
        INFO(msg.toString() << " -> " << reply.toString());
        CHECK(reply.get(0).asInt32() == 47);
    }

    SECTION("test names with spaces")
    {
        Server server;

        RpcClient client_port;
        RpcServer server_port;
        REQUIRE(client_port.open("/client"));
        REQUIRE(server_port.open("/server"));
        REQUIRE(yarp.connect(client_port.getName(), server_port.getName()));
        server.yarp().attachAsServer(server_port);

        Bottle msg, reply;
        msg.fromString("add_one 42");
        client_port.write(msg,reply);
        INFO(msg.toString() << " -> " << reply.toString());
        CHECK(reply.get(0).asInt32() == 43);

        msg.fromString("add one 52");
        client_port.write(msg,reply);
        INFO(msg.toString() << " -> " << reply.toString());
        CHECK(reply.get(0).asInt32() == 53);

        msg.fromString("get_answer");
        client_port.write(msg,reply);
        INFO(msg.toString() << " -> " << reply.toString());
        CHECK(reply.get(0).asInt32() == 42);

        msg.fromString("get answer");
        client_port.write(msg,reply);
        INFO(msg.toString() << " -> " << reply.toString());
        CHECK(reply.get(0).asInt32() == 42);
    }

    SECTION("test surface mesh")
    {
        SurfaceMesh mesh;
        Box3D bb;
        mesh.meshName = "testing";
        bb.corners.push_back(PointXYZ(1,2,3));
        SurfaceMeshWithBoundingBox obj(mesh,bb);

        Bottle bot;
        bot.read(obj);

        SurfaceMeshWithBoundingBox obj2;
        bot.write(obj2);
        Bottle bot2;
        bot2.read(obj2);

        INFO("mesh copy: " << bot.toString() << " -> " << bot2.toString());

        CHECK(obj2.mesh.meshName == obj.mesh.meshName);
        CHECK(obj2.boundingBox.corners.size() == obj.boundingBox.corners.size());
        CHECK(bot == bot2);
    }

    SECTION("test wrapping")
    {
        Wrapping client;
        WrappingServer server;

        RpcClient client_port;
        RpcServer server_port;
        REQUIRE(client_port.open("/client"));
        REQUIRE(server_port.open("/server"));
        REQUIRE(yarp.connect(client_port.getName(), server_port.getName()));

        int x = 0;
        client.yarp().attachAsClient(client_port);
        server.yarp().attachAsServer(server_port);
        x = client.check(Value(99));
        INFO("Result " << x);
        CHECK(x == 100);
        x = client.check(Value("6*7"));
        INFO("Result " << x);
        CHECK(x == 42);
        x = client.check(Value("test"));
        INFO("Result " << x);
        CHECK(x == 9);

        Bottle b = client.getBottle();
        INFO("Bottle is " << b.toString());
        CHECK(b.size() == 5);

        Bottle cmd, reply;
        cmd.fromString("getBottle");
        client_port.write(cmd,reply);
        INFO("Bottle is " << reply.get(0).toString());
        CHECK(reply.size() == 5);
    }

    SECTION("test_missing_method")
    {
        Demo client;
        BrokenServer server;

        RpcClient client_port;
        RpcServer server_port;
        REQUIRE(client_port.open("/client"));
        REQUIRE(server_port.open("/server"));
        REQUIRE(yarp.connect(client_port.getName(), server_port.getName()));

        int x = 0;
        client.yarp().attachAsClient(client_port);
        server.yarp().attachAsServer(server_port);
        x = client.get_answer();
        INFO("Answer " << x);
        CHECK(x == 42);

        x = client.add_one(88);
        INFO("AddOne gives " << x);
        CHECK(x != 89);
    }

    SECTION("test unwrap")
    {
        DemoStructList s;
        s.lst.push_back(DemoStruct(5,10));
        s.lst.push_back(DemoStruct(9,900));
        Bottle b;
        b.read(s);
        INFO("Wrapped: " << b.toString());
        CHECK(b.size() == 1);

        DemoStructList::unwrapped s2;
        s2.content.lst = s.lst;
        b.read(s2);
        INFO("Wrapped: " << b.toString());
        CHECK(b.size() == 2);
    }

    SECTION("test tostring")
    {
        DemoStructList s;
        s.lst.push_back(DemoStruct(5,10));
        s.lst.push_back(DemoStruct(9,900));
        INFO("String: " << s.toString());
        CHECK(s.toString() == "((5 10) (9 900))");

        DemoStruct d;
        d.x = 10;
        d.y = 20;
        INFO("String: " << d.toString());
        CHECK(d.toString() == "10 20");
    }

    SECTION("test editor")
    {
        DemoStruct d;
        d.x = 0;
        d.y = 0;
        DemoStruct::Editor e;
        e.edit(d,false);
        e.set_x(15);
        Bottle b;
        b.read(e);
        INFO(">>> set_x -> " <<  b.toString());
        CHECK(b.size() == 2);
        REQUIRE(b.get(1).asList() != nullptr);
        CHECK(b.get(1).asList()->get(0).asString() == "set");
        CHECK(b.get(1).asList()->get(1).asString() == "x");
        CHECK(b.get(1).asList()->get(2).asInt32() == 15);

        e.clean();
        e.set_y(30);
        b.read(e);
        INFO(">>> set_y -> " << b.toString());
        CHECK(b.size() == 2);
        REQUIRE(b.get(1).asList() != nullptr);
        CHECK(b.get(1).asList()->get(1).asString() == "y");
        CHECK(b.get(1).asList()->get(2).asInt32() ==30);

        e.clean();
        e.set_x(1);
        e.set_y(2);
        b.read(e);
        INFO(">>> set_x set_y -> " << b.toString());
        CHECK(b.size() == 3);
        REQUIRE(b.get(1).asList() != nullptr);
        REQUIRE(b.get(2).asList() != nullptr);
        CHECK(b.get(1).asList()->get(1).asString() == "x");
        CHECK(b.get(1).asList()->get(2).asInt32() == 1);
        CHECK(b.get(2).asList()->get(1).asString() == "y");
        CHECK(b.get(2).asList()->get(2).asInt32() == 2);

        DemoStruct d2;
        DemoStruct::Editor e2;
        e2.edit(d2,false);
        d2.x = 99;
        d2.y = 99;
        e.clean();
        e.set_y(30);
        Portable::copyPortable(e,e2);
        CHECK(d2.x == 99);
        CHECK(d2.y == 30);

        DemoStructCallbacks c;
        c.edit(d2,false);
        d2.x = 99;
        d2.y = 99;
        c.clean();
        c.set_y(30);
        CHECK(c.wsy == 99);
        CHECK(c.dsy == 30);

        DemoStruct d3;
        d3.x = 0;
        d3.y = 0;
        DemoStructCallbacks c2;
        c2.edit(d3,false);
        Portable::copyPortable(c,c2);
        CHECK(c2.wsy == 0);
        CHECK(c2.dsy == 30);
    }


    SECTION("test list editor")
    {
        DemoStructExt d;
        d.int_list.resize(5);
        DemoStructExt::Editor e;
        e.edit(d,false);
        e.set_int_list(4,15);
        Bottle b;
        b.read(e);
        INFO(">>> set_int_list -> " << b.toString());
        CHECK(b.size() == 2);
        REQUIRE(b.get(1).asList() != nullptr);
        CHECK(b.get(1).asList()->get(1).asString() == "int_list");
        REQUIRE(b.get(1).asList()->get(2).asList() != nullptr);
        CHECK(b.get(1).asList()->get(2).asList()->get(4).asInt32() == 15);
    }

    SECTION("test general help")
    {

        Server server;
        Bottle bot("[help]");
        DummyConnector con;
        bot.write(con.getWriter());
        server.read(con.getReader());
        bot.read(con.getReader());
        INFO("Service general help is " <<  bot.toString());
        std::string help = bot.toString();
        CHECK(help.find("get_answer") != std::string::npos);
    }

    SECTION("test specific help")
    {
        Server server;
        Bottle bot("[help] get_answer");
        DummyConnector con;
        bot.write(con.getWriter());
        server.read(con.getReader());
        bot.read(con.getReader());
        INFO("Service specific help is " << bot.toString());
        std::string help = bot.toString();
        CHECK(help.find("gets the answer") != std::string::npos);
    }

    SECTION("test structure general help")
    {
        DemoStruct d;
        DemoStruct::Editor e;
        e.edit(d,false);
        Bottle bot("help");
        DummyConnector con;

        bot.write(con.getWriter());
        e.read(con.getReader());
        bot.read(con.getReader());
        INFO("Structure general help is " << bot.toString());
        std::string help = bot.toString();
        CHECK(help.find("x") != std::string::npos);
    }

    SECTION("test structure specific help")
    {
        DemoStruct d;
        DemoStruct::Editor e;
        e.edit(d,false);
        Bottle bot("help x");
        DummyConnector con;

        bot.write(con.getWriter());
        e.read(con.getReader());
        bot.read(con.getReader());
        INFO("Structure specific help is " << bot.toString());
        std::string help = bot.toString();
        CHECK(help.find("this is the x part") != std::string::npos);
    }

    SECTION("test primitives")
    {
        TestSomeMoreTypes a, b;
        Bottle tmp;
        a.a_bool = true;
        a.a_i8 = 8;
        a.a_i16 = 16;
        a.a_i32 = 32;
        a.a_i64 = 64;
        tmp.read(a);
        tmp.write(b);
        CHECK(a.a_bool == b.a_bool);
        CHECK(a.a_i16 == b.a_i16);
        CHECK(a.a_i32 == b.a_i32);
        CHECK(a.a_i64 == b.a_i64);
    }

    SECTION("test annotated types")
    {
        TestAnnotatedTypes a, b;
        Bottle tmp;
        a.a_vocab = yarp::os::createVocab32('d', 'e', 'm', 'o');
        a.a_ui8 = 0xff;
        a.a_ui16 = 0xffff;
        a.a_ui32 = 0xffffffff;
        a.a_ui64 = 0xffffffffffffffff;
        a.a_float32 = 0.32;
        a.a_float64 = 0.64;
        a.a_size = sizeof(TestAnnotatedTypes);
        tmp.read(a);
        tmp.write(b);
        CHECK(a.a_vocab == b.a_vocab);
        CHECK(a.a_ui8 == b.a_ui8);
        CHECK(a.a_ui16 == b.a_ui16);
        CHECK(a.a_ui32 == b.a_ui32);
        CHECK(a.a_ui64 == b.a_ui64);
        CHECK(a.a_float32 == b.a_float32);
        CHECK(a.a_float64 == b.a_float64);
    }

    SECTION("test settings")
    {
        Settings::Editor settings;
        SettingsReceiver receiver;

        yarp::os::RpcClient sender_port;
        yarp::os::RpcServer receiver_port;

        settings.yarp().attachAsClient(sender_port);
        receiver.yarp().attachAsServer(receiver_port);

        REQUIRE(sender_port.open("/sender"));
        REQUIRE(receiver_port.open("/receiver"));
        REQUIRE(yarp.connect(sender_port.getName(), receiver_port.getName()));

        settings.set_id(5);
        CHECK(receiver.state().id == 5); // int assignment

        settings.set_name("hello");
        CHECK(receiver.state().name == "hello"); // string assignment

        settings.start_editing();
        settings.set_id(6);
        CHECK(receiver.state().id == 5); // not too early

        settings.set_name("world");
        CHECK(receiver.state().name == "hello"); // string not too early
        settings.stop_editing();
        CHECK(receiver.state().id == 6); // int group
        CHECK(receiver.state().name == "world"); // string group

        yarp::os::Bottle cmd, reply;
        cmd.fromString("patch (set id 3) (set name frog)");
        sender_port.write(cmd,reply);
        CHECK(reply.toString() == "[ok]"); // return on success
        CHECK(receiver.state().id == 3); // id ok
        CHECK(receiver.state().name == "frog"); // name ok

        cmd.fromString("set id 9");
        reply.clear();
        sender_port.write(cmd,reply);
        CHECK(receiver.state().id == 9); //  set id ok

        cmd.fromString("set id 99 name \"Space Monkey\"");
        reply.clear();
        sender_port.write(cmd,reply);
        CHECK(receiver.state().id == 99); // multi set ok
        CHECK(receiver.state().name == "Space Monkey"); // multi set ok

        cmd.fromString("set id 99 name \"Space Monkey\" id 101");
        reply.clear();
        sender_port.write(cmd,reply);
        CHECK(receiver.state().id == 101);  // triple set ok
    }

    SECTION("test start/stop")
    {
        Demo client;
        Server server;
        RpcClient client_port;
        RpcServer server_port;
        REQUIRE(client_port.open("/client"));
        REQUIRE(server_port.open("/server"));
        REQUIRE(yarp.connect(client_port.getName(), server_port.getName()));
        client.yarp().attachAsClient(client_port);
        server.yarp().attachAsServer(server_port);

        INFO("Starting a long operation");
        client.do_start_a_service();
        Time::delay(1);
        INFO("Stopping a long operation");
        client.do_stop_a_service();
    }

    SECTION("test map types")
    {
        Demo client;
        Server server;
        RpcClient client_port;
        RpcServer server_port;
        REQUIRE(client_port.open("/client"));
        REQUIRE(server_port.open("/server"));
        REQUIRE(yarp.connect(client_port.getName(), server_port.getName()));
        client.yarp().attachAsClient(client_port);
        server.yarp().attachAsServer(server_port);

        INFO("Requesting a map type");
        DemoStructMap demoStructMap = client.get_demo_map_struct();
        CHECK(demoStructMap.mapValues.size() == 4);
        CHECK(demoStructMap.mapValues["map_0"].x == 0);
        CHECK(demoStructMap.mapValues["map_0"].y == 10);
        CHECK(demoStructMap.mapValues["map_1"].x == 1);
        CHECK(demoStructMap.mapValues["map_1"].y == 11);
        CHECK(demoStructMap.mapValues["map_2"].x == 2);
        CHECK(demoStructMap.mapValues["map_2"].y == 12);
        CHECK(demoStructMap.mapValues["map_3"].x == 3);
        CHECK(demoStructMap.mapValues["map_3"].y == 13);
    }

    SECTION("test const")
    {
        Demo client;
        Server server;
        RpcClient client_port;
        RpcServer server_port;
        REQUIRE(client_port.open("/client"));
        REQUIRE(server_port.open("/server"));
        REQUIRE(yarp.connect(client_port.getName(), server_port.getName()));
        client.yarp().attachAsClient(client_port);
        server.yarp().attachAsServer(server_port);

        INFO("calling a const method");
        CHECK(const_cast<const Demo&>(client).this_is_a_const_method());

    }
}

/*
 * Copyright: (C) 2011 IITRBCS
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <stdio.h>

#include <yarp/os/all.h>
#include <yarp/os/impl/UnitTest.h>
#include <Demo.h>
#include <DemoStructList.h>
#include <DemoStructExt.h>
#include <SurfaceMeshWithBoundingBox.h>
#include <Wrapping.h>
#include <TestSomeMoreTypes.h>
#include <sub/directory/ClockServer.h>
#include <Settings.h>

using namespace yarp::os;
using namespace yarp::os::impl;

class ThriftTest : public UnitTest {
public:
    virtual ConstString getName() {
        return "ThriftTest";
    }
};

class Server : public Demo {
private:
    bool running;
    bool closing;
public:
    Server() {
        running = false;
        closing = false;
    }

    virtual int32_t get_answer() {
        return 42;
    }

    virtual int32_t add_one(const int32_t x) {
        printf("adding 1 to %d\n", x);
        return x+1;
    }

    virtual int32_t add_pair(const int32_t x, const int32_t y) {
        printf("adding %d and %d\n", x, y);
        return x+y;
    }

    virtual void test_void(const int32_t x) {
        printf("test void with %d\n", x);
    }

    virtual void test_1way(const int32_t x) {
        printf("test oneway with %d\n", x);
    }

    virtual bool test_defaults(const int32_t x) {
        printf("test defaults with %d\n", x);
        return (x==42);
    }

    virtual std::vector<DemoEnum> test_enum_vector(const std::vector<DemoEnum> & x) {
        printf("test_enum_vector\n");
        std::vector<DemoEnum> result = x;
        result.push_back(ENUM1);
        return result;
    }

    virtual int32_t test_partial(const int32_t x, 
                                 const std::vector<int32_t> & lst,
                                 const int32_t y) {
        printf("test_partial with %d and %d\n", x, y);
        YARP_UNUSED(lst);
        return x+y;
    }

    virtual int32_t test_tail_defaults(const DemoEnum x) {
        if (x==ENUM1) {
            return 42;
        }
        return 999;
    }

    virtual int32_t test_longer_tail_defaults(const int32_t ignore, const DemoEnum _enum, const int32_t _int, const std::string& _string) {
        YARP_UNUSED(ignore);
        if (_enum==ENUM2 && _int==42 && _string=="Space Monkey from the Planet: Space") {
            return 999;
        }
        return _int;
    }



    virtual void do_start_a_service() {
        running = true;
        while (!closing) {
            printf("Operating...\n");
            Time::delay(0.1);
        }
        running = false;
    }

    virtual bool do_check_for_service() {
        return running;
    }

    virtual void do_stop_a_service() {
        closing = true;
        while (running) {
            Time::delay(0.1);
        }
        closing = false;
    }
};


class BrokenServer : public Demo {
public:
    virtual int32_t get_answer() {
        return 42;
    }
};


class WrappingServer : public Wrapping {
public:
    virtual int32_t check(const yarp::os::Value& param) {
        if (param.isInt()) return param.asInt()+1;
        if (param.asString()=="6*7") return 42;
        return 9;
    }

    virtual Bottle getBottle() {
        Bottle b("this is a test (bottle)");
        return b;
    }
};

class DemoStructCallbacks : public DemoStruct::Editor {
public:
    int wsx, dsx, wsy, dsy;

    DemoStructCallbacks() {
        wsx = dsx = wsy = dsy = 0;
    }

    virtual bool will_set_x() {
        printf("will_set_x called, x is %d\n", get_x());
        wsx = get_x();
        return true;
    }
    virtual bool will_set_y() {
        printf("will_set_y called, y is %d\n", get_y());
        wsy = get_y();
        return true;
    }
    virtual bool did_set_x() {
        printf("did_set_x called, x is %d\n", get_x());
        dsx = get_x();
        return true;
    }
    virtual bool did_set_y() {
        printf("did_set_y called, y is %d\n", get_y());
        dsy = get_y();
        return true;
    }
};

class ClientPeek : public PortReader {
public:
    virtual bool read(ConnectionReader& con) {
        Bottle bot;
        bot.read(con);
        printf("Got %s\n", bot.toString().c_str());
        return true;
    }
};

class SettingsReceiver : public Settings::Editor {
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

    virtual bool will_set_id() { 
        called_will_set_id = true;
        return false;
    }

    virtual bool did_set_id() { 
        called_did_set_id = true;
        return false;
    }
};

bool add_one() {
    printf("\n*** add_one()\n");
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
    printf("Result is %s\n", bot.toString().c_str());

    if (bot.get(0).asInt() != 15) return false;

    bot.fromString("[add] [one] 15");
    DummyConnector con2;
    bot.write(con2.getWriter());
    server.read(con2.getReader());
    bot.read(con2.getReader());
    printf("Result is %s\n", bot.toString().c_str());

    if (bot.get(0).asInt() != 16) return false;

    return true;
}

bool test_void() {
    printf("\n*** test_void()\n");

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
    printf("Result is %s (should be blank)\n", bot.toString().c_str());

    return bot.size()==0 && !bot.isNull();
}

bool test_live() {
    printf("\n*** test_live()\n");

    Network yarp;

    Demo client;
    Server server;

    RpcClient client_port;
    RpcServer server_port;
    client_port.open("/client");
    server_port.open("/server");
    yarp.connect(client_port.getName(),server_port.getName());

    int x = 0;
    client.yarp().attachAsClient(client_port);
    server.yarp().attachAsServer(server_port);
    x = client.add_one(99);
    printf("Result %d\n", x);
    client.test_void(200);
    client.test_void(201);
    x = client.add_one(100);
    printf("Result %d\n", x);
    client.test_1way(200);
    client.test_1way(201);
    x = client.add_one(101);
    printf("Result %d\n", x);

    return (x==102);
}

bool test_live_rpc() {
    printf("\n*** test_live_rpc()\n");

    Network yarp;

    Demo client;
    Server server;

    RpcClient client_port;
    RpcServer server_port;
    client_port.open("/client");
    server_port.open("/server");
    yarp.connect(client_port.getName(),server_port.getName());

    client.yarp().attachAsClient(client_port);
    server.yarp().attachAsServer(server_port);

    // Check regular RPC works, even with oneways

    Bottle cmd, reply;
    cmd.fromString("[add] [one] 5");
    client_port.write(cmd,reply);
    printf("Cmd %s reply %s\n", cmd.toString().c_str(), reply.toString().c_str());
    if (reply.get(0).asInt()!=6) return false;

    cmd.fromString("[test] [void] 5");
    client_port.write(cmd,reply);
    printf("Cmd %s reply %s\n", cmd.toString().c_str(), reply.toString().c_str());

    cmd.fromString("[add] [one] 6");
    client_port.write(cmd,reply);
    printf("Cmd %s reply %s\n", cmd.toString().c_str(), reply.toString().c_str());
    if (reply.get(0).asInt()!=7) return false;

    cmd.fromString("[test] [1way] 5");
    client_port.write(cmd,reply);
    printf("Cmd %s reply %s\n", cmd.toString().c_str(), reply.toString().c_str());

    cmd.fromString("[add] [one] 7");
    client_port.write(cmd,reply);
    printf("Cmd %s reply %s\n", cmd.toString().c_str(), reply.toString().c_str());
    if (reply.get(0).asInt()!=8) return false;

    return true;
}

bool test_enums() {
    printf("\n*** test_enums()\n");

    Network yarp;

    Demo client;
    Server server;

    RpcClient client_port;
    RpcServer server_port;
    client_port.open("/client");
    server_port.open("/server");
    yarp.connect(client_port.getName(),server_port.getName());
    client.yarp().attachAsClient(client_port);
    server.yarp().attachAsServer(server_port);

    std::vector<DemoEnum> lst1;
    lst1.push_back(ENUM1);
    lst1.push_back(ENUM2);
    std::vector<DemoEnum> lst2 = client.test_enum_vector(lst1);
    printf("lst1 %d lst2 %d\n", (int)lst1.size(), (int)lst2.size());

    return (lst2.size()==3 && lst1[0]==lst2[0] && lst1[1]==lst2[1]);
}

bool test_defaults() {
    printf("\n*** test_defaults()\n");


    Network yarp;

    Demo client;
    Server server;

    RpcClient client_port;
    RpcServer server_port;
    client_port.open("/client");
    server_port.open("/server");
    yarp.connect(client_port.getName(),server_port.getName());
    client.yarp().attachAsClient(client_port);
    server.yarp().attachAsServer(server_port);

    bool ok1 = client.test_defaults();
    bool ok2 = client.test_defaults(14);
    printf("42 %d 14 %d\n", ok1?1:0, ok2?1:0);

    return ok1 && !ok2;
}

bool test_partial() {
    printf("\n*** test_partial()\n");


    Network yarp;

    Server server;

    RpcClient client_port;
    RpcServer server_port;
    client_port.open("/client");
    server_port.open("/server");
    yarp.connect(client_port.getName(),server_port.getName());
    server.yarp().attachAsServer(server_port);

    Bottle msg, reply;
    msg.fromString("add pair 4 3");
    client_port.write(msg,reply);
    printf("%s -> %s\n", msg.toString().c_str(), reply.toString().c_str());
    if (reply.get(0).asInt() != 7) return false;

    msg.fromString("add pair 4");
    client_port.write(msg,reply);
    printf("(incomplete) %s -> %s\n", msg.toString().c_str(), reply.toString().c_str());
    if (reply.get(0).asVocab() != VOCAB4('f','a','i','l')) return false;

    msg.fromString("add pair");
    reply.fromString("0");
    client_port.write(msg,reply);
    printf("(incomplete) %s -> %s\n", msg.toString().c_str(), reply.toString().c_str());
    if (reply.get(0).asVocab() != VOCAB4('f','a','i','l')) return false;

    msg.fromString("add");
    reply.fromString("0");
    client_port.write(msg,reply);
    printf("(incomplete) %s -> %s\n", msg.toString().c_str(), reply.toString().c_str());
    if (reply.get(0).asVocab() != VOCAB4('f','a','i','l')) return false;

    msg.fromString("");
    reply.fromString("0");
    client_port.write(msg,reply);
    printf("(incomplete) %s -> %s\n", msg.toString().c_str(), reply.toString().c_str());
    if (reply.get(0).asVocab() != VOCAB4('f','a','i','l')) return false;

    msg.fromString("add pair 10 20");
    reply.fromString("0");
    client_port.write(msg,reply);
    printf("%s -> %s\n", msg.toString().c_str(), reply.toString().c_str());
    if (reply.get(0).asInt() != 30) return false;

    msg.fromString("test partial 10 (40 50 60) 5");
    reply.fromString("0");
    client_port.write(msg,reply);
    printf("%s -> %s\n", msg.toString().c_str(), reply.toString().c_str());
    if (reply.get(0).asInt() != 15) return false;

    msg.fromString("test partial 10 (40 50)");
    reply.fromString("0");
    client_port.write(msg,reply);
    printf("%s -> %s\n", msg.toString().c_str(), reply.toString().c_str());
    if (reply.get(0).asVocab() != VOCAB4('f','a','i','l')) return false;

    return true;
}

bool test_defaults_with_rpc() {
    printf("\n*** test_defaults_with_rpc()\n");


    Network yarp;

    Server server;

    RpcClient client_port;
    RpcServer server_port;
    client_port.open("/client");
    server_port.open("/server");
    yarp.connect(client_port.getName(),server_port.getName());
    server.yarp().attachAsServer(server_port);

    Bottle msg, reply;
    msg.fromString("test_tail_defaults");
    client_port.write(msg,reply);
    printf("%s -> %s\n", msg.toString().c_str(), reply.toString().c_str());
    if (reply.get(0).asInt() != 42) return false;

    msg.fromString("test_tail_defaults 55");
    client_port.write(msg,reply);
    printf("%s -> %s\n", msg.toString().c_str(), reply.toString().c_str());
    if (reply.get(0).asInt() != 999) return false;

    msg.fromString("test longer tail defaults");
    client_port.write(msg,reply);
    printf("%s -> %s\n", msg.toString().c_str(), reply.toString().c_str());
    if (reply.get(0).asVocab() != VOCAB4('f','a','i','l')) return false;

    msg.fromString("test longer tail defaults 888");
    client_port.write(msg,reply);
    printf("%s -> %s\n", msg.toString().c_str(), reply.toString().c_str());
    if (reply.get(0).asInt() != 999) return false;

    msg.fromString("test longer tail defaults 888 ENUM2 47");
    client_port.write(msg,reply);
    printf("%s -> %s\n", msg.toString().c_str(), reply.toString().c_str());
    if (reply.get(0).asInt() != 47) return false;

    return true;
}


bool test_names_with_spaces() {
    printf("\n*** test_names_with_spaces()\n");

    Network yarp;

    Server server;

    RpcClient client_port;
    RpcServer server_port;
    client_port.open("/client");
    server_port.open("/server");
    yarp.connect(client_port.getName(),server_port.getName());
    server.yarp().attachAsServer(server_port);

    Bottle msg, reply;
    msg.fromString("add_one 42");
    client_port.write(msg,reply);
    printf("%s -> %s\n", msg.toString().c_str(), reply.toString().c_str());
    if (reply.get(0).asInt() != 43) return false;

    msg.fromString("add one 52");
    client_port.write(msg,reply);
    printf("%s -> %s\n", msg.toString().c_str(), reply.toString().c_str());
    if (reply.get(0).asInt() != 53) return false;

    msg.fromString("get_answer");
    client_port.write(msg,reply);
    printf("%s -> %s\n", msg.toString().c_str(), reply.toString().c_str());
    if (reply.get(0).asInt() != 42) return false;

    msg.fromString("get answer");
    client_port.write(msg,reply);
    printf("%s -> %s\n", msg.toString().c_str(), reply.toString().c_str());
    if (reply.get(0).asInt() != 42) return false;

    return true;
}

bool test_surface_mesh() {
    printf("\n*** test_surface_mesh()\n");
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

    printf("mesh copy: %s -> %s\n", bot.toString().c_str(),
           bot2.toString().c_str());

    if (obj2.mesh.meshName!=obj.mesh.meshName) {
        printf("mesh name not copied correctly\n");
        return false;
    }
    if (obj2.boundingBox.corners.size()!=obj.boundingBox.corners.size()) {
        printf("corners not copied correctly\n");
        return false;
    }
    if (bot!=bot2) {
        printf("not copied correctly\n");
        return false;
    }

    return true;
}

bool test_wrapping() {
    printf("\n*** test_wrapping()\n");

    Network yarp;

    Wrapping client;
    WrappingServer server;

    RpcClient client_port;
    RpcServer server_port;
    client_port.open("/client");
    server_port.open("/server");
    yarp.connect(client_port.getName(),server_port.getName());

    int x = 0;
    client.yarp().attachAsClient(client_port);
    server.yarp().attachAsServer(server_port);
    x = client.check(99);
    printf("Result %d\n", x);
    if (x!=100) return false;
    x = client.check(Value("6*7"));
    printf("Result %d\n", x);
    if (x!=42) return false;
    x = client.check(Value("test"));
    printf("Result %d\n", x);
    if (x!=9) return false;

    Bottle b = client.getBottle();
    printf("Bottle is %s\n", b.toString().c_str());
    if (b.size()!=5) return false;

    Bottle cmd, reply;
    cmd.fromString("getBottle");
    client_port.write(cmd,reply);
    printf("Bottle is %s\n", reply.get(0).toString().c_str());
    if (!reply.get(0).isList()) return false;
    if (reply.get(0).asList()->size()!=5) return false;

    return true;
}

bool test_missing_method() {
    printf("\n*** test_missing_method()\n");

    Network yarp;

    Demo client;
    BrokenServer server;

    RpcClient client_port;
    RpcServer server_port;
    client_port.open("/client");
    server_port.open("/server");
    yarp.connect(client_port.getName(),server_port.getName());

    int x = 0;
    client.yarp().attachAsClient(client_port);
    server.yarp().attachAsServer(server_port);
    x = client.get_answer();
    printf("Answer %d\n", x);
    if (x!=42) return false;

    x = client.add_one(88);
    printf("AddOne gives %d\n", x);
    if (x==89) return false;

    return true;
}

bool test_unwrap() {
    printf("\n*** test_unwrap()\n");
    
    DemoStructList s;
    s.lst.push_back(DemoStruct(5,10));
    s.lst.push_back(DemoStruct(9,900));
    Bottle b;
    b.read(s);
    printf("Wrapped: %s\n", b.toString().c_str());
    if (b.size()!=1) return false;

    DemoStructList::unwrapped s2;
    s2.content.lst = s.lst;
    b.read(s2);
    printf("Wrapped: %s\n", b.toString().c_str());
    if (b.size()!=2) return false;

    return true;
}

bool test_tostring() {
    printf("\n*** test_tostring()\n");
    
    DemoStructList s;
    s.lst.push_back(DemoStruct(5,10));
    s.lst.push_back(DemoStruct(9,900));
    printf("String: %s\n", s.toString().c_str());
    if (s.toString() != "((5 10) (9 900))") {
        fprintf(stderr, "string mismatch\n");
        return false;
    }

    DemoStruct d;
    d.x = 10;
    d.y = 20;
    printf("String: %s\n", d.toString().c_str());
    if (d.toString() != "10 20") {
        fprintf(stderr, "string mismatch\n");
        return false;
    }

    return true;
}

bool test_editor() {
    printf("\n*** test_editor()\n");
    DemoStruct d;
    d.x = 0;
    d.y = 0;
    DemoStruct::Editor e;
    e.edit(d,false);
    e.set_x(15);
    Bottle b;
    b.read(e);
    printf(">>> set_x -> %s\n", b.toString().c_str());
    if (b.size()!=2) {
        fprintf(stderr, "wrong length after set_x\n");
        return false;
    }
    if (b.get(1).asList()==NULL) {
        fprintf(stderr, "wrong type after set_x\n");
        return false;
    }
    if (b.get(1).asList()->get(0).asString()!="set") {
        fprintf(stderr, "wrong act after set_x\n");
        return false;
    }
    if (b.get(1).asList()->get(1).asString()!="x") {
        fprintf(stderr, "wrong tag after set_x\n");
        return false;
    }
    if (b.get(1).asList()->get(2).asInt()!=15) {
        fprintf(stderr, "wrong value after set_x\n");
        return false;
    }

    e.clean();
    e.set_y(30);
    b.read(e);
    printf(">>> set_y -> %s\n", b.toString().c_str());
    if (b.size()!=2) {
        fprintf(stderr, "wrong length after set_y\n");
        return false;
    }
    if (b.get(1).asList()==NULL) {
        fprintf(stderr, "wrong type after set_y\n");
        return false;
    }
    if (b.get(1).asList()->get(1).asString()!="y") {
        fprintf(stderr, "wrong tag after set_y\n");
        return false;
    }
    if (b.get(1).asList()->get(2).asInt()!=30) {
        fprintf(stderr, "wrong value after set_y\n");
        return false;
    }

    e.clean();
    e.set_x(1);
    e.set_y(2);
    b.read(e);
    printf(">>> set_x set_y -> %s\n", b.toString().c_str());
    if (b.size()!=3) {
        fprintf(stderr, "wrong length after set_x set_y\n");
        return false;
    }
    if (b.get(1).asList()==NULL) {
        fprintf(stderr, "wrong type 0 after set_x set_y\n");
        return false;
    }
    if (b.get(2).asList()==NULL) {
        fprintf(stderr, "wrong type 1 after set_x set_y\n");
        return false;
    }
    if (b.get(1).asList()->get(1).asString()!="x") {
        fprintf(stderr, "wrong x tag after set_x set_y\n");
        return false;
    }
    if (b.get(1).asList()->get(2).asInt()!=1) {
        fprintf(stderr, "wrong x value after set_x set_y\n");
        return false;
    }
    if (b.get(2).asList()->get(1).asString()!="y") {
        fprintf(stderr, "wrong y tag after set_x set_y\n");
        return false;
    }
    if (b.get(2).asList()->get(2).asInt()!=2) {
        fprintf(stderr, "wrong y value after set_x set_y\n");
        return false;
    }

    DemoStruct d2;
    DemoStruct::Editor e2;
    e2.edit(d2,false);
    d2.x = 99;
    d2.y = 99;
    e.clean();
    e.set_y(30);
    Portable::copyPortable(e,e2);
    if (d2.x!=99) {
        fprintf(stderr, "wrong x value after patch\n");
        return false;
    }
    if (d2.y!=30) {
        fprintf(stderr, "wrong y value after patch\n");
        return false;
    }

    DemoStructCallbacks c;
    c.edit(d2,false);
    d2.x = 99;
    d2.y = 99;
    c.clean();
    c.set_y(30);
    if (c.wsy!=99||c.dsy!=30) {
        fprintf(stderr, "callback muddle\n");
        return false;
    }

    DemoStruct d3;
    d3.x = 0;
    d3.y = 0;
    DemoStructCallbacks c2;
    c2.edit(d3,false);
    Portable::copyPortable(c,c2);
    if (c2.wsy!=0||c2.dsy!=30) {
        fprintf(stderr, "callback muddle\n");
        return false;
    }

    return true;
}


bool test_list_editor() {
    printf("\n*** test_list_editor()\n");
    DemoStructExt d;
    d.int_list.resize(5);
    DemoStructExt::Editor e;
    e.edit(d,false);
    e.set_int_list(4,15);
    Bottle b;
    b.read(e);
    printf(">>> set_int_list -> %s\n", b.toString().c_str());
    if (b.size()!=2) {
        fprintf(stderr, "wrong length after set_int_list\n");
        return false;
    }
    if (b.get(1).asList()==NULL) {
        fprintf(stderr, "no patch after set_int_list\n");
        return false;
    }
    if (b.get(1).asList()->get(1).asString()!="int_list") {
        fprintf(stderr, "wrong tag after set_int_list\n");
        return false;
    }
    if (b.get(1).asList()->get(2).asList()==NULL) {
        fprintf(stderr, "no list after set_int_list\n");
        return false;
    }
    if (b.get(1).asList()->get(2).asList()->get(4).asInt()!=15) {
        fprintf(stderr, "wrong value after set_int_list\n");
        return false;
    }
    return true;
}

bool test_help() {
    printf("\n*** test_help()\n");

    {
        Server server;
        Bottle bot("[help]");
        DummyConnector con;
        bot.write(con.getWriter());
        server.read(con.getReader());
        bot.read(con.getReader());
        printf("Service general help is %s\n", bot.toString().c_str());
        ConstString help = bot.toString();
        if (help.find("get_answer")==ConstString::npos) {
            fprintf(stderr,"no list given\n");
            return false;
        }
    }

    {
        Server server;
        Bottle bot("[help] get_answer");
        DummyConnector con;
        bot.write(con.getWriter());
        server.read(con.getReader());
        bot.read(con.getReader());
        printf("Service specific help is %s\n", bot.toString().c_str());
        ConstString help = bot.toString();
        if (help.find("gets the answer")==ConstString::npos) {
            fprintf(stderr,"no help given\n");
            return false;
        }
    }

    {
        DemoStruct d;
        DemoStruct::Editor e;
        e.edit(d,false);
        Bottle bot("help");
        DummyConnector con;
        
        bot.write(con.getWriter());
        e.read(con.getReader());
        bot.read(con.getReader());
        printf("Structure general help is %s\n", bot.toString().c_str());
        ConstString help = bot.toString();
        if (help.find("x")==ConstString::npos) {
            fprintf(stderr,"no field list\n");
            return false;
        }
    }

    {
        DemoStruct d;
        DemoStruct::Editor e;
        e.edit(d,false);
        Bottle bot("help x");
        DummyConnector con;
        
        bot.write(con.getWriter());
        e.read(con.getReader());
        bot.read(con.getReader());
        printf("Structure specific help is %s\n", bot.toString().c_str());
        ConstString help = bot.toString();
        if (help.find("this is the x part")==ConstString::npos) {
            fprintf(stderr,"no help given\n");
            return false;
        }
    }

    return true;
}

bool test_primitives() {
    printf("\n*** test_primitives()\n");
    TestSomeMoreTypes a, b;
    Bottle tmp;
    a.a_bool = true;
    a.a_byte = 8;
    a.a_i16 = 16;
    a.a_i32 = 32;
    a.a_i64 = 64;
    tmp.read(a);
    tmp.write(b);
    if (a.a_bool!=b.a_bool) {
        fprintf(stderr,"copy bool failed\n");
        return false;
    }
    if (a.a_i16!=b.a_i16) {
        fprintf(stderr,"copy i16 failed\n");
        return false;
    }
    if (a.a_i32!=b.a_i32) {
        fprintf(stderr,"copy i32 failed\n");
        return false;
    }
    if (a.a_i64!=b.a_i64) {
        fprintf(stderr,"copy i64 failed\n");
        return false;
    }
    printf("Copies all done\n");
    return true;
}

bool test_settings(UnitTest& test) {
    test.report(0,"test settings");

    Settings::Editor settings;
    SettingsReceiver receiver;

    Network yarp;
    yarp::os::RpcClient sender_port;
    yarp::os::RpcServer receiver_port;

    settings.yarp().attachAsClient(sender_port);
    receiver.yarp().attachAsServer(receiver_port);

    if (!sender_port.open("/sender")) return 1;
    if (!receiver_port.open("/receiver")) return 1;
    yarp.connect("/sender","/receiver");    

    settings.set_id(5);
    test.checkEqual(receiver.state().id,5,"int assignment");

    settings.set_name("hello");
    test.checkEqual(receiver.state().name,"hello","string assignment");

    settings.begin();
    settings.set_id(6);
    test.checkEqual(receiver.state().id,5,"not too early");
    settings.set_name("world");
    test.checkEqual(receiver.state().name,"hello","string not too early");
    settings.end();
    test.checkEqual(receiver.state().id,6,"int group");
    test.checkEqual(receiver.state().name,"world","string group");

    yarp::os::Bottle cmd, reply;
    cmd.fromString("patch (set id 3) (set name frog)");
    sender_port.write(cmd,reply);
    test.checkEqual(reply.toString(),"[ok]","return on success");
    test.checkEqual(receiver.state().id,3,"id ok");
    test.checkEqual(receiver.state().name,"frog","name ok");

    cmd.fromString("set id 9");
    reply.clear();
    sender_port.write(cmd,reply);
    test.checkEqual(receiver.state().id,9,"set id ok");

    cmd.fromString("set id 99 name \"Space Monkey\"");
    reply.clear();
    sender_port.write(cmd,reply);
    test.checkEqual(receiver.state().id,99,"multi set ok");
    test.checkEqual(receiver.state().name,"Space Monkey","multi set ok");

    cmd.fromString("set id 99 name \"Space Monkey\" id 101");
    reply.clear();
    sender_port.write(cmd,reply);
    test.checkEqual(receiver.state().id,101,"triple set ok");

    return test.isOk();
}

bool test_start_stop(UnitTest& test) {
    test.report(0,"test start/stop");

    Network yarp;
    Demo client;
    Server server;
    RpcClient client_port;
    RpcServer server_port;
    client_port.open("/client");
    server_port.open("/server");
    yarp.connect(client_port.getName(),server_port.getName());
    client.yarp().attachAsClient(client_port);
    server.yarp().attachAsServer(server_port);

    printf("Starting a long operation\n");
    client.do_start_a_service();
    Time::delay(1);
    printf("Stopping a long operation\n");
    client.do_stop_a_service();
    return true;
}

int main(int argc, char *argv[]) {
    if (argc>1) {
        Network yarp;
        Server server;
        RpcServer server_port;
        server.yarp().attachAsServer(server_port);
        server_port.open(argv[1]);
        while (true) {
            Time::delay(60);
        }
        return 0;
    }

    Network yarp;
    yarp.setLocalMode(true);

    if (!add_one()) return 1;
    if (!test_void()) return 1;
    if (!test_live()) return 1;
    if (!test_live_rpc()) return 1;
    if (!test_enums()) return 1;
    if (!test_defaults()) return 1;
    if (!test_partial()) return 1;
    if (!test_defaults_with_rpc()) return 1;
    if (!test_names_with_spaces()) return 1;
    if (!test_surface_mesh()) return 1;
    if (!test_wrapping()) return 1;
    if (!test_missing_method()) return 1;
    if (!test_unwrap()) return 1;
    if (!test_tostring()) return 1;
    if (!test_editor()) return 1;
    if (!test_list_editor()) return 1;
    if (!test_help()) return 1;
    if (!test_primitives()) return 1;
    UnitTest::startTestSystem();
    ThriftTest test;
    if (!test_settings(test)) return 1;
    if (!test_start_stop(test)) return 1;
    UnitTest::stopTestSystem();
    return 0;
}

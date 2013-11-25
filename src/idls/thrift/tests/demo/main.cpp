// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright: (C) 2011 IITRBCS
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <stdio.h>

#include <yarp/os/all.h>
#include <Demo.h>
#include <SurfaceMeshWithBoundingBox.h>
#include <Wrapping.h>

using namespace yarp::os;

class Server : public Demo {
public:
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
        return x+y;
    }

    virtual int32_t test_tail_defaults(const DemoEnum x) {
        if (x==ENUM1) {
            return 42;
        }
        return 999;
    }

    virtual int32_t test_longer_tail_defaults(const int32_t ignore, const DemoEnum _enum, const int32_t _int, const std::string& _string) {
        if (_enum==ENUM2 && _int==42 && _string=="Space Monkey from the Planet: Space") {
            return 999;
        }
        return _int;
    }
};

class WrappingServer : public Wrapping {
public:
    virtual int32_t check(const yarp::os::Value& param) {
        if (param.isInt()) return param.asInt()+1;
        if (param.asString()=="6*7") return 42;
        return 9;
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
    yarp.setLocalMode(true);

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
    yarp.setLocalMode(true);

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
    yarp.setLocalMode(true);

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
    printf("lst1 %d lst2 %d\n", lst1.size(), lst2.size());

    return (lst2.size()==3 && lst1[0]==lst2[0] && lst1[1]==lst2[1]);
}

bool test_defaults() {
    printf("\n*** test_defaults()\n");


    Network yarp;
    yarp.setLocalMode(true);

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
    yarp.setLocalMode(true);

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
    yarp.setLocalMode(true);

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
    yarp.setLocalMode(true);

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
    yarp.setLocalMode(true);

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

    return true;
}

int main(int argc, char *argv[]) {
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
    return 0;
}

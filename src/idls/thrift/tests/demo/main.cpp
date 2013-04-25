// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright: (C) 2011 IITRBCS
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <stdio.h>

#include <yarp/os/all.h>
#include <Demo.h>

using namespace yarp::os;

class Server : public Demo {
public:
    virtual int32_t add_one(const int32_t x) {
        printf("adding 1 to %d\n", x);
        return x+1;
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

    return bot.get(0).asInt() == 15;
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
    printf("Result is %s\n", bot.toString().c_str());

    return bot.size()==0 && !bot.isNull();
}

bool test_live() {
    printf("\n*** test_live()\n");

    Network yarp;
    yarp.setLocalMode(true);

    Demo client;
    Server server;

    Port client_port,server_port;
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

    Port client_port,server_port;
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

    Port client_port,server_port;
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

    Port client_port,server_port;
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

int main(int argc, char *argv[]) {
    if (!add_one()) return 1;
    if (!test_void()) return 1;
    if (!test_live()) return 1;
    if (!test_live_rpc()) return 1;
    if (!test_enums()) return 1;
    if (!test_defaults()) return 1;
    return 0;
}

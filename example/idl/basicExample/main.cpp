// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright: (C) 2012 IITRBCS
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <stdio.h>

#include <yarp/os/all.h>

#include "Example.h"

using namespace yarp::os;

class NewStyleServer : public Example {
public:
    virtual int32_t add(const int32_t x, const int32_t y) {
        printf("adding integers %d and %d found in function arguments\n", x, y);
        return x+y;
    }
};

class OldStyleServer : public yarp::os::PortReader {
public:
    virtual bool read(ConnectionReader& reader) {
        Bottle cmd, resp;
        cmd.read(reader);
        printf("Command is: %s\n", cmd.toString().c_str());
        ConnectionWriter *writer = reader.getWriter();
        if (!writer) return true;
        int tag = cmd.get(0).asVocab();
        if (tag==VOCAB3('a','d','d')) {
            int x = cmd.get(1).asInt();
            int y = cmd.get(2).asInt();
            printf("adding integers %d and %d found in message\n", x, y);
            resp.addInt(x+y);
            return resp.write(*writer);
        }
        return true;
    }
};

int main(int argc, char *argv[]) {
    Network yarp;
    int r;

    Port server;
    Port client;
    if (!server.open("/example/server")) { return 1; }
    if (!client.open("/example/client")) { return 1; }
    yarp.connect(client.getName(),server.getName());
    yarp.sync(client.getName());
    yarp.sync(server.getName());
    printf("\n");
    printf("RPC between an IDL-based client and server\n");
    NewStyleServer example_server;
    example_server.yarp().attachAsServer(server);
    Example e;
    e.yarp().attachAsClient(client);
    r = e.add(3,7);
    printf("Result of function call: %d\n", r);

    printf("\n");
    printf("RPC between a regular client and IDL-based server\n");
    Bottle cmd("[add] 3 7"), reply;
    client.write(cmd,reply);
    printf("Result on wire: %s\n", reply.toString().c_str());

    printf("\n");
    printf("RPC between an IDL-based client and a regular server\n");

    OldStyleServer s;
    e.yarp().attachAsClient(s);
    r = e.add(3,7);
    printf("Result of function call: %d\n", r);

    printf("\n");
    printf("RPC between a regular client and a regular server\n");

    e.yarp().write(cmd,reply);
    printf("Result on wire: %s\n", reply.toString().c_str());

    printf("\n");

    client.close();
    server.close();
  
    return 0;
}

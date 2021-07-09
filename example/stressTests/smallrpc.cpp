/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Basic rpc tests, without use of controlboard stuff

#include <yarp/os/all.h>
using namespace yarp::os;


class RpcService : public PortReader {
public:
    virtual bool read(ConnectionReader& con) {
        Bottle cmd;
        cmd.read(con);
        ConnectionWriter *writer = con.getWriter();
        if (writer!=NULL) {
            Time::delay(0.08);
            cmd.write(*writer);
        }
        return true;
    }
};

int runServer(Searchable& config) {
    RpcService service;
    Port p;
    p.setReader(service);
    p.open(config.check("name",Value("/rpc/server")).asString().c_str());
    while(true) {
        printf("Server running.\n");
        Time::delay(5);
    }
    return 0;
}

int runClient(Searchable& config) {
    Port p;
    p.open(config.check("name",Value("/rpc/client")).asString().c_str());
    std::string sname =
        config.check("remote",Value("/rpc/server")).asString().c_str();
    Network::connect(p.getName().c_str(),sname);
    Network::sync(sname);
    for (int i=0; i<10000; i++) {
        Bottle cmd, reply;
        cmd.addString(p.getName().c_str());
        cmd.addInt32(i);
        p.write(cmd,reply);
        printf("[%s] [%s]\n", cmd.toString().c_str(),
               reply.toString().c_str());
        Time::delay(0.1);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    Property config;
    config.fromCommand(argc,argv);
    if (config.check("server")) {
        return runServer(config);
    }
    if (config.check("client")) {
        return runClient(config);
    }
    printf("Run as:\n");
    printf("  smallrpc --server\n");
    printf("  smallrpc --client --name /rpc/clientN\n");
    return 1;
}

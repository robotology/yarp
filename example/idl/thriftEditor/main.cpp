/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <stdio.h>
#include <yarp/os/all.h>
#include "Settings.h"

// class to test callbacks when Settings are changed.
class Receiver : public Settings::Editor {
public:
    virtual bool will_set_id() { 
        printf("[id] is about to be set, it is currently %d\n",
               state().id);
    }

    virtual bool did_set_id() { 
        printf("[id] was set, to %d\n", state().id);
    }

    virtual bool will_set_name() { 
        printf("[name] is about to be set, it is currently '%s'\n",
               state().name.c_str());
    }

    virtual bool did_set_name() { 
        printf("[name] was set, to '%s'\n", state().name.c_str());
    }
};


int main() {
    yarp::os::Network yarp;
  
    Settings::Editor settings; // original copy of Settings
    Receiver receiver;         // a copy of Settings updated over network

    yarp::os::RpcClient sender_port;    // a port for sending out Settings
    yarp::os::RpcServer receiver_port;  // a port for receiving Settings

    // link copies of Settings to the ports that transmit/recieve them
    settings.yarp().attachAsClient(sender_port);
    receiver.yarp().attachAsServer(receiver_port);

    // open ports and connect them
    if (!sender_port.open("/sender")) return 1;
    if (!receiver_port.open("/receiver")) return 1;
    yarp.connect("/sender","/receiver");

    printf("####################################################\n");
    printf("Setting [id]\n");

    // change one field in Settings (should see receiver callbacks trigger)
    settings.set_id(5);

    printf("####################################################\n");
    printf("Setting [name]\n");

    // change one field in Settings (should see receiver callbacks trigger)
    settings.set_name("hello");

    printf("####################################################\n");
    printf("Setting [id] and [name]\n");

    // change two fields at a time, in case we want to be efficient of
    // network use
    settings.begin();
    settings.set_id(6);
    if (receiver.state().id==6) {
        printf("Oops, id was set too early!\n");
        return 1;
    } else {
        printf("(set_id has been called, but has not happened yet, good.\n");
    }
    settings.set_name("world");
    settings.end();
    if (receiver.state().id!=6) {
        printf("Oops, id was never set!\n");
        return 1;
    }

    printf("####################################################\n");
    printf("Pretend to use server from RPC: patch command\n");

    yarp::os::Bottle cmd, reply;
    cmd.fromString("patch (set id 3) (set name frog)");
    sender_port.write(cmd,reply);
    printf("answer: %s\n", reply.toString().c_str());

    printf("####################################################\n");
    printf("Pretend to use server from RPC: set command\n");

    cmd.fromString("set id 9");
    reply.clear();
    sender_port.write(cmd,reply);
    printf("answer: %s\n", reply.toString().c_str());

    cmd.fromString("set id 99 name \"Space Monkey\"");
    reply.clear();
    sender_port.write(cmd,reply);
    printf("answer: %s\n", reply.toString().c_str());

    printf("####################################################\n");

    return 0;
}

// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */



#include <yarp/os/impl/FallbackNameServer.h>
#include <yarp/os/impl/DgramTwoWayStream.h>
#include <yarp/os/impl/NetType.h>
#include <yarp/os/impl/NameServer.h>
#include <yarp/Bytes.h>

using namespace yarp::os::impl;

const Address FallbackNameServer::mcastLastResort("224.2.1.1",10001,
                                                  "mcast","fallback");


void FallbackNameServer::run() {
    DgramTwoWayStream send;
    send.join(getAddress(),true);
    listen.join(getAddress(),false);
    
    YARP_DEBUG(Logger::get(),"Fallback server running");
    while (listen.isOk()&&send.isOk()&&!closed) {
        YARP_DEBUG(Logger::get(),"Fallback server waiting");
        String msg;
        listen.beginPacket();
        msg = NetType::readLine(listen);
        listen.endPacket();
        YARP_DEBUG(Logger::get(),"Fallback server got something");
        if (listen.isOk()&&!closed) {
            YARP_DEBUG(Logger::get(),String("Fallback server got ") + msg);
            if (YARP_STRSTR(msg,"NAME_SERVER ") == 0) {
                Address addr;
                String result = owner.apply(msg,addr);
                //Bytes b((char*)(result.c_str()),result.length());
                send.beginPacket();
                send.writeLine(result);
                send.flush();
                send.endPacket();
            }
        }
    }
}


void FallbackNameServer::close() {
    closed = true;
    listen.interrupt();
}







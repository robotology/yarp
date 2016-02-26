/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/conf/system.h>
#ifdef YARP_HAS_ACE

#include <yarp/os/impl/FallbackNameClient.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/NetType.h>
#include <yarp/os/impl/NameClient.h>
#include <yarp/os/impl/NameConfig.h>
#include <yarp/os/Time.h>
#include <yarp/os/impl/FallbackNameServer.h>

using namespace yarp::os::impl;
using namespace yarp::os;

void FallbackNameClient::run() {
    NameConfig nc;
    Contact call = FallbackNameServer::getAddress();
    DgramTwoWayStream send;
    send.join(call,true);
    listen.join(call,false);
    if (!listen.isOk()) {
        YARP_ERROR(Logger::get(),String("Multicast not available"));
        return;
    }
    String msg = String("NAME_SERVER query ") + nc.getNamespace();
    send.beginPacket();
    send.writeLine(msg.c_str(),(int)msg.length());
    send.flush();
    send.endPacket();
    for (int i=0; i<5; i++) {
        listen.beginPacket();
        String txt = listen.readLine();
        listen.endPacket();
        if (closed) return;
        YARP_DEBUG(Logger::get(),String("Fallback name client got ") + txt);
        if (txt.find("registration ")==0) {
            address = NameClient::extractAddress(txt);
            YARP_INFO(Logger::get(),String("Received address ") +
                      address.toURI());
            return;
        }
    }
}


void FallbackNameClient::close() {
    if (!closed) {
        closed = true;
        listen.interrupt();
        listen.close();
        join();
    }
}


Contact FallbackNameClient::getAddress() {
    return address;
}


Contact FallbackNameClient::seek() {
    int tries = 3;
    for (int k=0; k<tries; k++) {

        FallbackNameClient seeker;

        YARP_INFO(Logger::get(),
                  String("Polling for name server (using multicast), try ") +
                  NetType::toString(k+1) +
                  String(" of max ") +
                  NetType::toString(tries));

        seeker.start();
        Time::delay(0.25);
        if (seeker.getAddress().isValid()) {
            return seeker.getAddress();
        }
        int len = 20;
        for (int i0=0; i0<len; i0++) {
            ACE_OS::fprintf(stderr,"++");
        }
        ACE_OS::fprintf(stderr,"\n");

        for (int i=0; i<len; i++) {
            Time::delay(0.025);
            ACE_OS::fprintf(stderr,"++");
            if (seeker.getAddress().isValid()) {
                ACE_OS::fprintf(stderr,"\n");
                return seeker.getAddress();
            }
        }
        ACE_OS::fprintf(stderr,"\n");
        YARP_INFO(Logger::get(),"No response to search for server");
        seeker.close();
        seeker.join();
    }
    return Contact();
}

#else

int FallbackNameClientDummySymbol = 42;

#endif // YARP_HAS_ACE


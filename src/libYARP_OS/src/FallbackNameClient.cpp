// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/FallbackNameClient.h>
#include <yarp/Logger.h>
#include <yarp/NetType.h>
#include <yarp/NameClient.h>
#include <yarp/NameConfig.h>
#include <yarp/os/Time.h>
#include <yarp/FallbackNameServer.h>

using namespace yarp;
using namespace yarp::os;

void FallbackNameClient::run() {
    try {
        NameConfig nc;
        Address call = FallbackNameServer::getAddress();
        DgramTwoWayStream send;
        send.join(call,true);
        listen.join(call,false);
        String msg = String("NAME_SERVER query ") + nc.getNamespace();
        send.beginPacket();
        send.writeLine(msg);
        send.flush();
        send.endPacket();
        for (int i=0; i<5; i++) {
            listen.beginPacket();
            String txt = NetType::readLine(listen);
            listen.endPacket();
            if (closed) return;
            YARP_DEBUG(Logger::get(),String("Fallback name client got ") + txt);
            if (txt.strstr("registration ")==0) {
                address = NameClient::extractAddress(txt);
                YARP_INFO(Logger::get(),String("Received address ") + 
                          address.toString());
                return;
            }
        }
    } catch (IOException e) {
        YARP_DEBUG(Logger::get(),e.toString() + "<<< Fallback client shutting down with exception");
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


Address FallbackNameClient::getAddress() {
    return address;
}


Address FallbackNameClient::seek() {
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
        try {
            seeker.close();
            seeker.join();
        } catch (IOException e) {
            YARP_DEBUG(Logger::get(), e.toString() + " <<< exception while closing seeker");
        }
    }
    return Address();
}



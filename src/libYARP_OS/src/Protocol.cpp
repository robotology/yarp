// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/impl/Protocol.h>
#include <yarp/os/Bottle.h>

using namespace yarp::os::impl;
using namespace yarp::os;


bool Protocol::open(const Route& route) {
    setRoute(route);
    setCarrier(route.getCarrierName());
    if (delegate==NULL) {
        return false;
    }
    bool ok = sendHeader();
    if (!ok) {
        return false;
    }
    return expectReplyToHeader();
}


bool Protocol::defaultSendIndex() {
    YARP_ASSERT(writer!=NULL);
    writeYarpInt(10);
    int len = (int)writer->length();
    char lens[] = { (char)len, 1,
                    -1, -1, -1, -1,
                    -1, -1, -1, -1 };
    Bytes b(lens,10);
    os().write(b);
    for (int i=0; i<len; i++) {
        NetType::netInt((int)writer->length(i),number.bytes());
        os().write(number.bytes());
    }
    NetType::netInt(0,number.bytes());
    os().write(number.bytes());
    return os().isOk();
}


bool Protocol::defaultExpectIndex() {
    YARP_DEBUG(Logger::get(),"expecting an index");
    YARP_SPRINTF1(Logger::get(),
                  debug,
                  "Protocol::expectIndex for %s", 
                  getRoute().toString().c_str());
    // expect index header
    ssize_t r = NetType::readFull(is(),header.bytes());
    if ((size_t)r!=header.length()) {
        YARP_DEBUG(log,"broken index");
        return false;
    }
    int len = interpretYarpNumber(header.bytes());
    if (len<0) {
        YARP_DEBUG(log,"broken index - header is not a number");
        return false;
    }
    if (len!=10) {
        YARP_DEBUG(log,"broken index - header is wrong length");
        return false;
    }
    YARP_DEBUG(Logger::get(),"index coming in happily...");
    r = NetType::readFull(is(),indexHeader.bytes());
    if ((size_t)r!=indexHeader.length()) {
        YARP_DEBUG(log,"broken index, secondary header");
        return false;
    }
    YARP_DEBUG(Logger::get(),"secondary header came in happily...");
    int inLen = (unsigned char)(indexHeader.get()[0]);
    int outLen = (unsigned char)(indexHeader.get()[1]);
    // Big limit on number of blocks here!  Inherited from QNX.
    // should make it go away if it hurts someone.

    int total = 0;
    for (int i=0; i<inLen; i++) {
        ssize_t l = NetType::readFull(is(),number.bytes());
        if ((size_t)l!=number.length()) {
            YARP_DEBUG(log,"bad input block length");
            return false;
        }
        int x = NetType::netInt(number.bytes());
        total += x;
    }
    for (int i2=0; i2<outLen; i2++) {
        ssize_t l = NetType::readFull(is(),number.bytes());
        if ((size_t)l!=number.length()) {
            YARP_DEBUG(log,"bad output block length");
            return false;
        }
        int x = NetType::netInt(number.bytes());
        total += x;
    }
    messageLen = total;
    YARP_SPRINTF1(Logger::get(),
                  debug,
                  "Total message length: %d",
                  messageLen);
    //YARP_DEBUG(Logger::get(),"got an index");
    return true;
}

bool Protocol::defaultSendAck() {
    YARP_DEBUG(Logger::get(),"sending an acknowledgment");
    if (delegate->requireAck()) {
        writeYarpInt(0);
    }
    return true;
}

void Protocol::setRoute(const Route& route) {
    Route r = route;

    // First, make sure route is canonicalized.
    // If there are qualifiers in the source name, propagate them
    // to the carrier.
    String from = r.getFromName();
    String carrier = r.getCarrierName();
    if (YARP_STRSTR(from," ")!=String::npos) {
        Bottle b(from.c_str());
        if (b.size()>1) {
            r = r.addFromName(b.get(0).toString().c_str());
            for (int i=1; i<b.size(); i++) {
                Value& v = b.get(i);
                Bottle *lst = v.asList();
                if (lst!=NULL) {
                    carrier = carrier + "+" + lst->get(0).toString().c_str() +
                        "." + lst->get(1).toString().c_str();
                } else {
                    carrier = carrier + "+" + v.toString().c_str();
                }
            }
            r = r.addCarrierName(carrier);
        }
    }

    this->route = r;

    if (!recv_delegate) {
        Bottle b(getSenderSpecifier().c_str());
        if (b.check("recv")) {
            need_recv_delegate = true;
        }
    }
}


String Protocol::getSenderSpecifier() {
    Route r = getRoute();
    String from = r.getFromName();
    String carrier = r.getCarrierName();
    YARP_STRING_INDEX start = YARP_STRSTR(carrier,"+");
    if (start!=String::npos) {
        from += " (";
        for (YARP_STRING_INDEX i=start+1; 
             i<(YARP_STRING_INDEX)carrier.length(); 
             i++) {
            char ch = carrier[i];
            if (ch=='+') {
                from += ") (";
            } else if (ch=='.') {
                from += " ";
            } else {
                from += ch;
            }
        }
        from += ")";
    }
    return from;
}


bool Protocol::getRecvDelegate() {
    if (recv_delegate) return true;
    if (!need_recv_delegate) return true;
    Bottle b(getSenderSpecifier().c_str());
    ConstString tag = b.find("recv").asString();
    recv_delegate = Carriers::chooseCarrier(String(tag.c_str()));
    if (!recv_delegate) {
        fprintf(stderr,"Need carrier \"%s\", but cannot find it.\n",
                tag.c_str());
        close();
        return false;
    }
    if (!recv_delegate->modifiesIncomingData()) {
        fprintf(stderr,"Carrier \"%s\" does not modify incoming data as expected.\n",
                tag.c_str());
        close();
        return false;
    }
    return recv_delegate->configure(*this);
}

bool Protocol::skipIncomingData(yarp::os::ConnectionReader& reader) {
    size_t pending = reader.getSize();
    if (pending>0) {
        while (pending>0) {
            char buf[10000];
            size_t next = (pending<sizeof(buf))?pending:sizeof(buf);
            reader.expectBlock(&buf[0],next);
            pending -= next;
        }
        return true;
    }
    return false;
}

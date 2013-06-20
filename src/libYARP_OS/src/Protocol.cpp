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

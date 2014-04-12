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
    if (from.find(" ")!=String::npos) {
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

    if (!send_delegate) {
        Bottle b(getSenderSpecifier().c_str());
        if (b.check("send")) {
            need_send_delegate = true;
        }
    }
}


ConstString Protocol::getSenderSpecifier() {
    Route r = getRoute();
    ConstString from = r.getFromName();
    ConstString carrier = r.getCarrierName();
    size_t start = carrier.find("+");
    if (start!=String::npos) {
        from += " (";
        for (size_t i=start+1; i<(size_t)carrier.length(); i++) {
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

bool Protocol::getSendDelegate() {
    if (send_delegate) return true;
    if (!need_send_delegate) return true;
    Bottle b(getSenderSpecifier().c_str());
    ConstString tag = b.find("send").asString();
    send_delegate = Carriers::chooseCarrier(String(tag.c_str()));
    if (!send_delegate) {
        fprintf(stderr,"Need carrier \"%s\", but cannot find it.\n",
                tag.c_str());
        close();
        return false;
    }
    if (!send_delegate->modifiesOutgoingData()) {
        fprintf(stderr,"Carrier \"%s\" does not modify outgoing data as expected.\n",
                tag.c_str());
        close();
        return false;
    }
    
    return send_delegate->configure(*this);
}


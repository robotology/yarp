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


Protocol::Protocol(TwoWayStream* stream) :
        messageLen(0),
        pendingAck(false),
        log(Logger::get()),
        active(true),
        delegate(NULL),
        recv_delegate(NULL),
        send_delegate(NULL),
        need_recv_delegate(false),
        need_send_delegate(false),
        recv_delegate_fail(false),
        send_delegate_fail(false),
        route("null","null","tcp"),
        writer(NULL),
        ref(NULL),
        envelope(""),
        port(NULL),
        pendingReply(false)
{
    // We start off with the streams used to contact the port that
    // owns this connection.
    shift.takeStream(stream);

    reader.setProtocol(this);
}

bool Protocol::open(const ConstString& name) {
    if (name=="") return false;
    setRoute(getRoute().addToName(name));
    // We are not the initiator of the connection, so we
    // expect to receive a header (carrier-dependent).
    bool ok = expectHeader();
    if (!ok) return false;
    // Respond to header (carrier-dependent).
    return respondToHeader();
}

bool Protocol::open(const Route& route) {
    setRoute(route);
    setCarrier(route.getCarrierName());
    if (delegate==NULL) return false;
    // We are the initiator of the connection, so we
    // send a header (carrier-dependent).
    bool ok = sendHeader();
    if (!ok) return false;
    // Expect a resonse to the header (carrier-dependent).
    return expectReplyToHeader();
}

void Protocol::setRoute(const Route& route) {
    Route r = route;

    // We reorganize the route to reduce variation in naming.
    // If there are qualifiers in the source port name, propagate
    // those qualifiers to the carrier.
    ConstString from = r.getFromName();
    ConstString carrier = r.getCarrierName();
    if (from.find(" ")!=ConstString::npos) {
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

    // Record canonicalized route.
    this->route = r;

    // Check if we have a receiver modifier.
    if (!recv_delegate) {
        Bottle b(getSenderSpecifier().c_str());
        if (b.check("recv")) {
            need_recv_delegate = true;
        }
    }

    // Check if we have a sender modifier.
    if (!send_delegate) {
        Bottle b(getSenderSpecifier().c_str());
        if (b.check("send")) {
            need_send_delegate = true;
        }
    }
}


ConstString Protocol::getSenderSpecifier() {
    Route r = getRoute();
    // We pull the sender name from the route.
    ConstString from = r.getFromName();
    // But we need to add any qualifiers looking in the carrier
    // name.  Ideally, we wouldn't need to bundle that in with
    // the sender name, but we do it for now in the name of
    // backwards compatibility.
    ConstString carrier = r.getCarrierName();
    size_t start = carrier.find("+");
    if (start!=ConstString::npos) {
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
    // If we've already checked for a receiver modifier, return.
    if (recv_delegate) return true;
    if (!need_recv_delegate) return true;
    if (recv_delegate_fail) return false;
    Bottle b(getSenderSpecifier().c_str());
    // Check for a "recv" qualifier.
    ConstString tag = b.find("recv").asString();
    recv_delegate = Carriers::chooseCarrier(ConstString(tag.c_str()));
    if (!recv_delegate) {
        fprintf(stderr,"Need carrier \"%s\", but cannot find it.\n",
                tag.c_str());
        recv_delegate_fail = true;
        close();
        return false;
    }
    if (!recv_delegate->modifiesIncomingData()) {
        fprintf(stderr,"Carrier \"%s\" does not modify incoming data as expected.\n",
                tag.c_str());
        recv_delegate_fail = true;
        close();
        return false;
    }
    // Configure the receiver modifier.
    if (!recv_delegate->configure(*this)) {
        fprintf(stderr,"Carrier \"%s\" could not configure the send delegate.\n",
                tag.c_str());
        recv_delegate_fail = true;
        close();
        return false;
    }
    return true;
}

bool Protocol::getSendDelegate() {
    // If we've already checked for a sender modifier, return.
    if (send_delegate) return true;
    if (!need_send_delegate) return true;
    if (send_delegate_fail) return false;
    Bottle b(getSenderSpecifier().c_str());
    // Check for a "send" qualifier.
    ConstString tag = b.find("send").asString();
    send_delegate = Carriers::chooseCarrier(ConstString(tag.c_str()));
    if (!send_delegate) {
        fprintf(stderr,"Need carrier \"%s\", but cannot find it.\n",
                tag.c_str());
        send_delegate_fail = true;
        close();
        return false;
    }
    if (!send_delegate->modifiesOutgoingData()) {
        fprintf(stderr,"Carrier \"%s\" does not modify outgoing data as expected.\n",
                tag.c_str());
        send_delegate_fail = true;
        close();
        return false;
    }
    // Configure the sender modifier.
    if (!send_delegate->configure(*this)) {
        fprintf(stderr,"Carrier \"%s\" could not configure the send delegate.\n",
                tag.c_str());
        send_delegate_fail = true;
        close();
        return false;
    }
    return true;
}

void Protocol::interrupt() {
    if (!active) return;
    if (pendingAck) {
        // Don't neglect to send one last acknowledgement if needed.
        sendAck();
    }
    // Break the input stream.
    shift.getInputStream().interrupt();
    active = false;
}

bool Protocol::respondToHeader() {
    yAssert(delegate!=NULL);
    bool ok = delegate->respondToHeader(*this);
    if (!ok) return false;
    os().flush();
    return os().isOk();
}

bool Protocol::expectAck() {
    yAssert(delegate!=NULL);
    if (delegate->requireAck()) {
        return delegate->expectAck(*this);
    }
    return true;
}

void Protocol::closeHelper() {
    active = false;
    if (pendingAck) {
        sendAck();
    }
    shift.close();
    if (delegate!=NULL) {
        delegate->close();
        delete delegate;
        delegate = NULL;
    }
    if (recv_delegate!=NULL) {
        recv_delegate->close();
        delete recv_delegate;
        recv_delegate = NULL;
    }
    if (send_delegate!=NULL) {
        send_delegate->close();
        delete send_delegate;
        send_delegate = NULL;
    }
}

bool Protocol::sendAck() {
    bool ok = true;
    pendingAck = false;
    if (delegate==NULL) return false;
    if (delegate->requireAck()) {
        ok = delegate->sendAck(*this);
    }
    getStreams().endPacket();
    return ok;
}

bool Protocol::expectIndex() {
    // We'll eventually need to send an acknowledgement
    // (if the carrier in use requires that).
    pendingAck = true;
    messageLen = 0;
    // This is where a message can be considered to begin.
    // If things go wrong on an unreliable carrier (e.g. on
    // udp), we should skip to the beginning of the next
    // message, as marked by this call.
    getStreams().beginPacket();
    ref = NULL;
    bool ok = false;
    if (delegate!=NULL) {
        // What we actually do here is carrier-specific.
        // Perhaps we do nothing at all.
        ok = delegate->expectIndex(*this);
    }
    if (ok) {
        // Set up a reader for the user payload.
        reader.reset(is(),&getStreams(),getRoute(),
                     messageLen,delegate->isTextMode(),
                     delegate->isBareMode());
        // Pass on a reference to the object being
        // send, if we know it, for local connections.
        if (ref!=NULL) {
            reader.setReference(ref);
        }
    } else {
        reader.reset(is(),&getStreams(),getRoute(),0,false);
    }
    return ok;
}

void Protocol::setCarrier(const ConstString& carrierNameBase) {
    // Set up the carrier for this connection.  The carrier
    // has all the protocol-specific behavior.
    ConstString carrierName = carrierNameBase;
    if (carrierNameBase=="") carrierName = "tcp";
    setRoute(getRoute().addCarrierName(carrierName));
    if (delegate==NULL) {
        delegate = Carriers::chooseCarrier(carrierName);
        if (delegate!=NULL) {
            if (delegate->modifiesIncomingData()) {
                if (active) {
                    fprintf(stderr,"Carrier \"%s\" cannot be used this way, try \"tcp+recv.%s\" instead.\n",carrierName.c_str(),carrierName.c_str());
                }
                close();
                return;
            }
            delegate->prepareSend(*this);
        }
    }
}

bool Protocol::expectHeader() {
    // A header, for historic reasons, is seen as
    // a protocol fingerprint (at least 8 bytes)
    // and the name of the sender.  In practice,
    // these callbacks have been stretched to the
    // point where their names have little bearing
    // on what the get used for.
    messageLen = 0;
    bool ok = expectProtocolSpecifier();
    if (!ok) return false;
    ok = expectSenderSpecifier();
    if (!ok) return false;
    yAssert(delegate!=NULL);
    ok = delegate->expectExtraHeader(*this);
    return ok;
}

bool Protocol::expectProtocolSpecifier() {
    // Historically YARP has used the first 8 bytes of
    // every connection as a way to identify it.  This
    // assumption is showing its age, and should really
    // be generalized.
    char buf[8];
    yarp::os::Bytes header((char*)&buf[0],sizeof(buf));
    YARP_SSIZE_T len = is().readFull(header);
    if (len==-1) {
        YARP_DEBUG(log,"no connection");
        return false;
    }
    if((size_t)len!=header.length()) {
        YARP_DEBUG(log,"data stream died");
        return false;
    }
    bool already = false;
    if (delegate!=NULL) {
        if (delegate->checkHeader(header)) {
            already = true;
        }
    }
    if (!already) {
        delegate = Carriers::chooseCarrier(header);
        if (delegate==NULL) {
            // Carrier not found; send a human-readable message.
            ConstString msg = "* Error. Protocol not found.\r\n* Hello. You appear to be trying to communicate with a YARP Port.\r\n* The first 8 bytes sent to a YARP Port are critical for identifying the\r\n* protocol you wish to speak.\r\n* The first 8 bytes you sent were not associated with any particular protocol.\r\n* If you are a human, try typing \"CONNECT foo\" followed by a <RETURN>.\r\n* The 8 bytes \"CONNECT \" correspond to a simple text-mode protocol.\r\n* Goodbye.\r\n";
            yarp::os::Bytes b((char*)msg.c_str(),msg.length());
            os().write(b);
            os().flush();
        }
    }
    if (delegate==NULL) {
        YARP_DEBUG(log,"unrecognized protocol");
        return false;
    }
    setRoute(getRoute().addCarrierName(delegate->getName()));
    delegate->setParameters(header);
    return true;
}

ConnectionReader& Protocol::beginRead() {
    // We take care of reading the message index
    // (carrier-specific preamble), then leave it
    // up to caller to read the actual message payload.
    getRecvDelegate();
    if (delegate!=NULL) {
        bool ok = false;
        while (!ok) {
            ok = expectIndex();
            if (!ok) {
                if (!is().isOk()) {
                    // Go ahead, we'll be shutting down, it'll
                    // work out.
                    ok = true;
                }
            }
        }
        respondToIndex();
    }
    return reader;
}

bool Protocol::write(SizedWriter& writer) {
    // End any current write.
    writer.stopWrite();
    // Skip if this connection is not active (e.g. when there are several
    // logical mcast connections but only one write is actually needed).
    if (!getConnection().isActive()) return false;
    this->writer = &writer;
    bool replied = false;
    yAssert(delegate!=NULL);
    getStreams().beginPacket(); // Message begins.
    bool ok = delegate->write(*this,writer);
    getStreams().endPacket();   // Message ends.
    PortReader *reply = writer.getReplyHandler();
    if (reply!=NULL) {
        if (!delegate->supportReply()) {
            // We are expected to get a reply, but cannot.
            YARP_INFO(log,ConstString("connection ") + getRoute().toString() + " does not support replies (try \"tcp\" or \"text_ack\")");
        }
        if (ok) {
            // Read reply.
            reader.reset(is(),&getStreams(), getRoute(),
                         messageLen,delegate->isTextMode(),
                         delegate->isBareMode());
            replied = reply->read(reader);
        }
    }
    expectAck(); // Expect acknowledgement (carrier-specific).
    this->writer = NULL;
    return replied;
}


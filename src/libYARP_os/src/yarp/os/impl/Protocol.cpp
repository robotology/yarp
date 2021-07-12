/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/Protocol.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Carrier.h>
#include <yarp/os/Carriers.h>
#include <yarp/os/NetType.h>
#include <yarp/os/Portable.h>
#include <yarp/os/ShiftStream.h>
#include <yarp/os/TwoWayStream.h>
#include <yarp/os/impl/LogComponent.h>
#include <yarp/os/impl/StreamConnectionReader.h>

#include <cstdio>
#include <cstdlib>
#include <string>

using namespace yarp::os::impl;
using namespace yarp::os;

namespace {
YARP_OS_LOG_COMPONENT(PROTOCOL, "yarp.os.impl.Protocol")
} // namespace

Protocol::Protocol(TwoWayStream* stream) :
        messageLen(0),
        pendingAck(false),
        active(true),
        delegate(nullptr),
        recv_delegate(nullptr),
        send_delegate(nullptr),
        need_recv_delegate(false),
        need_send_delegate(false),
        recv_delegate_fail(false),
        send_delegate_fail(false),
        route("null", "null", "tcp"),
        writer(nullptr),
        ref(nullptr),
        envelope(""),
        port(nullptr),
        pendingReply(false)
{
    // We start off with the streams used to contact the port that
    // owns this connection.
    shift.takeStream(stream);

    reader.setProtocol(this);
    yCDebug(PROTOCOL, "Remote contact = %s", reader.getRemoteContact().toURI().c_str());
}


Protocol::~Protocol()
{
    closeHelper();
}


void Protocol::setRoute(const Route& route)
{
    Route r = route;

    // We reorganize the route to reduce variation in naming.
    // If there are qualifiers in the source port name, propagate
    // those qualifiers to the carrier.
    std::string from = r.getFromName();
    std::string carrier = r.getCarrierName();
    if (from.find(' ') != std::string::npos) {
        Bottle b(from);
        if (b.size() > 1) {
            r.setFromName(b.get(0).toString());
            for (size_t i = 1; i < b.size(); i++) {
                Value& v = b.get(i);
                Bottle* lst = v.asList();
                if (lst != nullptr) {
                    carrier.append("+").append(lst->get(0).toString()).append(".").append(lst->get(1).toString());
                } else {
                    carrier.append("+").append(v.toString());
                }
            }
            r.setCarrierName(carrier);
        }
    }

    // Record canonicalized route.
    this->route = r;

    // Check if we have a receiver modifier.
    if (recv_delegate == nullptr) {
        Bottle b(getSenderSpecifier());
        if (b.check("recv")) {
            need_recv_delegate = true;
        }
    }

    // Check if we have a sender modifier.
    if (send_delegate == nullptr) {
        Bottle b(getSenderSpecifier());
        if (b.check("send")) {
            need_send_delegate = true;
        }
    }
}


const Route& Protocol::getRoute() const
{
    return route;
}


TwoWayStream& Protocol::getStreams()
{
    return shift;
}


void Protocol::takeStreams(TwoWayStream* streams)
{
    shift.takeStream(streams);
    if (streams != nullptr) {
        active = true;
    }
}


TwoWayStream* Protocol::giveStreams()
{
    return shift.giveStream();
}


bool Protocol::checkStreams() const
{
    return shift.isOk();
}


void Protocol::setReference(yarp::os::Portable* ref)
{
    this->ref = ref;
}


std::string Protocol::getSenderSpecifier() const
{
    Route r = getRoute();
    // We pull the sender name from the route.
    std::string from = r.getFromName();
    // But we need to add any qualifiers looking in the carrier
    // name.  Ideally, we wouldn't need to bundle that in with
    // the sender name, but we do it for now in the name of
    // backwards compatibility.
    std::string carrier = r.getCarrierName();
    size_t start = carrier.find('+');
    if (start != std::string::npos) {
        from += " (";
        for (size_t i = start + 1; i < (size_t)carrier.length(); i++) {
            char ch = carrier[i];
            if (ch == '+') {
                from += ") (";
            } else if (ch == '.') {
                from += " ";
            } else {
                from += ch;
            }
        }
        from += ")";
    }
    return from;
}


const std::string& Protocol::getEnvelope() const
{
    return envelope;
}


void Protocol::setRemainingLength(int len)
{
    messageLen = len;
}


Connection& Protocol::getConnection()
{
    if (delegate == nullptr) {
        return nullConnection;
    }
    return *delegate;
}


Contactable* Protocol::getContactable() const
{
    return port;
}


bool Protocol::open(const std::string& name)
{
    if (name.empty()) {
        return false;
    }
    Route r = getRoute();
    r.setToName(name);
    setRoute(r);
    // We are not the initiator of the connection, so we
    // expect to receive a header (carrier-dependent).
    bool ok = expectHeader();
    if (!ok) {
        return false;
    }
    // Respond to header (carrier-dependent).
    return respondToHeader();
}


bool Protocol::open(const Route& route)
{
    setRoute(route);
    setCarrier(route.getCarrierName());
    if (delegate == nullptr) {
        return false;
    }
    // We are the initiator of the connection, so we
    // send a header (carrier-dependent).
    bool ok = sendHeader();
    if (!ok) {
        return false;
    }
    // Expect a resonse to the header (carrier-dependent).
    return expectReplyToHeader();
}


void Protocol::close()
{
    closeHelper();
}


void Protocol::interrupt()
{
    if (!active) {
        return;
    }
    if (pendingAck) {
        // Don't neglect to send one last acknowledgement if needed.
        sendAck();
    }
    // Break the input stream.
    shift.getInputStream().interrupt();
    active = false;
}


OutputStream& Protocol::getOutputStream()
{
    return shift.getOutputStream();
}


InputStream& Protocol::getInputStream()
{
    return shift.getInputStream();
}


void Protocol::rename(const Route& route)
{
    setRoute(route);
}


bool Protocol::isOk() const
{
    return !(!checkStreams() || recv_delegate_fail || send_delegate_fail);
}


bool Protocol::write(SizedWriter& writer)
{
    // End any current write.
    writer.stopWrite();
    // Skip if this connection is not active (e.g. when there are several
    // logical mcast connections but only one write is actually needed).
    if (!getConnection().isActive()) {
        return false;
    }
    this->writer = &writer;
    bool replied = false;
    yCAssert(PROTOCOL, delegate != nullptr);
    getStreams().beginPacket(); // Message begins.
    bool ok = delegate->write(*this, writer);
    getStreams().endPacket(); // Message ends.
    PortReader* reply = writer.getReplyHandler();
    if (reply != nullptr) {
        if (!delegate->supportReply()) {
            // We are expected to get a reply, but cannot.
            yCInfo(PROTOCOL, "connection %s does not support replies (try \"tcp\" or \"text_ack\")", getRoute().toString().c_str());
        }
        if (ok) {
            // Read reply.
            reader.reset(is(), &getStreams(), getRoute(), messageLen, delegate->isTextMode(), delegate->isBareMode());
            replied = reply->read(reader);
        }
    }
    expectAck(); // Expect acknowledgement (carrier-specific).
    this->writer = nullptr;
    return replied;
}


void Protocol::reply(SizedWriter& writer)
{
    writer.stopWrite();
    delegate->reply(*this, writer);
    pendingReply = false;
}


OutputProtocol& Protocol::getOutput()
{
    return *this;
}


InputProtocol& Protocol::getInput()
{
    return *this;
}


ConnectionReader& Protocol::beginRead()
{
    // We take care of reading the message index
    // (carrier-specific preamble), then leave it
    // up to caller to read the actual message payload.
    getRecvDelegate();
    if (delegate != nullptr) {
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


void Protocol::endRead()
{
    reader.flushWriter();
    sendAck(); // acknowledge after reply (if there is one)
}


void Protocol::suppressReply()
{
    reader.suppressReply();
}


bool Protocol::setTimeout(double timeout)
{
    bool ok = os().setWriteTimeout(timeout);
    if (!ok) {
        return false;
    }
    return is().setReadTimeout(timeout);
}


void Protocol::setEnvelope(const std::string& str)
{
    envelope = str;
}


Connection& Protocol::getReceiver()
{
    if (recv_delegate == nullptr) {
        return nullConnection;
    }
    return *recv_delegate;
}


void Protocol::attachPort(yarp::os::Contactable* port)
{
    this->port = port;
}


bool Protocol::isReplying() const
{
    return pendingReply;
}


void Protocol::beginWrite()
{
    getSendDelegate();
}


Connection& Protocol::getSender()
{
    if (send_delegate == nullptr) {
        return nullConnection;
    }
    return *send_delegate;
}


bool Protocol::getRecvDelegate()
{
    // If we've already checked for a receiver modifier, return.
    if (recv_delegate != nullptr) {
        return true;
    }
    if (!need_recv_delegate) {
        return true;
    }
    if (recv_delegate_fail) {
        return false;
    }
    Bottle b(getSenderSpecifier());
    // Check for a "recv" qualifier.
    std::string tag = b.find("recv").asString();
    recv_delegate = Carriers::chooseCarrier(tag);
    if (recv_delegate == nullptr) {
        fprintf(stderr, "Need carrier \"%s\", but cannot find it.\n", tag.c_str());
        recv_delegate_fail = true;
        close();
        return false;
    }
    if (!recv_delegate->modifiesIncomingData()) {
        fprintf(stderr, "Carrier \"%s\" does not modify incoming data as expected.\n", tag.c_str());
        recv_delegate_fail = true;
        close();
        return false;
    }
    // Configure the receiver modifier.
    if (!recv_delegate->configure(*this)) {
        fprintf(stderr, "Carrier \"%s\" could not configure the send delegate.\n", tag.c_str());
        recv_delegate_fail = true;
        close();
        return false;
    }
    return true;
}


bool Protocol::getSendDelegate()
{
    // If we've already checked for a sender modifier, return.
    if (send_delegate != nullptr) {
        return true;
    }
    if (!need_send_delegate) {
        return true;
    }
    if (send_delegate_fail) {
        return false;
    }
    Bottle b(getSenderSpecifier());
    // Check for a "send" qualifier.
    std::string tag = b.find("send").asString();
    send_delegate = Carriers::chooseCarrier(tag);
    if (send_delegate == nullptr) {
        fprintf(stderr, "Need carrier \"%s\", but cannot find it.\n", tag.c_str());
        send_delegate_fail = true;
        close();
        return false;
    }
    if (!send_delegate->modifiesOutgoingData()) {
        fprintf(stderr, "Carrier \"%s\" does not modify outgoing data as expected.\n", tag.c_str());
        send_delegate_fail = true;
        close();
        return false;
    }
    // Configure the sender modifier.
    if (!send_delegate->configure(*this)) {
        fprintf(stderr, "Carrier \"%s\" could not configure the send delegate.\n", tag.c_str());
        send_delegate_fail = true;
        close();
        return false;
    }
    return true;
}


bool Protocol::respondToHeader()
{
    yCAssert(PROTOCOL, delegate != nullptr);
    bool ok = delegate->respondToHeader(*this);
    if (!ok) {
        return false;
    }
    os().flush();
    return os().isOk();
}


bool Protocol::expectAck()
{
    yCAssert(PROTOCOL, delegate != nullptr);
    if (delegate->requireAck()) {
        return delegate->expectAck(*this);
    }
    return true;
}


void Protocol::closeHelper()
{
    active = false;
    if (pendingAck) {
        sendAck();
    }
    shift.close();
    if (delegate != nullptr) {
        delegate->close();
        delete delegate;
        delegate = nullptr;
    }
    if (recv_delegate != nullptr) {
        recv_delegate->close();
        delete recv_delegate;
        recv_delegate = nullptr;
    }
    if (send_delegate != nullptr) {
        send_delegate->close();
        delete send_delegate;
        send_delegate = nullptr;
    }
}


bool Protocol::sendAck()
{
    bool ok = true;
    pendingAck = false;
    if (delegate == nullptr) {
        return false;
    }
    if (delegate->requireAck()) {
        ok = delegate->sendAck(*this);
        os().flush();
    }
    getStreams().endPacket();
    return ok;
}


bool Protocol::expectIndex()
{
    // We'll eventually need to send an acknowledgement
    // (if the carrier in use requires that).
    pendingAck = true;
    messageLen = 0;
    // This is where a message can be considered to begin.
    // If things go wrong on an unreliable carrier (e.g. on
    // udp), we should skip to the beginning of the next
    // message, as marked by this call.
    getStreams().beginPacket();
    ref = nullptr;
    bool ok = false;
    if (delegate != nullptr) {
        // What we actually do here is carrier-specific.
        // Perhaps we do nothing at all.
        ok = delegate->expectIndex(*this);
    }
    if (ok) {
        // Set up a reader for the user payload.
        reader.reset(is(), &getStreams(), getRoute(), messageLen, delegate->isTextMode(), delegate->isBareMode());
        // Pass on a reference to the object being
        // send, if we know it, for local connections.
        if (ref != nullptr) {
            reader.setReference(ref);
        }
    } else {
        reader.reset(is(), &getStreams(), getRoute(), 0, false);
    }
    return ok;
}


void Protocol::setCarrier(const std::string& carrierNameBase)
{
    // Set up the carrier for this connection.  The carrier
    // has all the protocol-specific behavior.
    std::string carrierName = carrierNameBase;
    if (carrierNameBase.empty()) {
        carrierName = "tcp";
    }
    Route route = getRoute();
    route.setCarrierName(carrierName);
    setRoute(route);
    if (delegate == nullptr) {
        delegate = Carriers::chooseCarrier(carrierName);
        if (delegate != nullptr) {
            if (delegate->modifiesIncomingData()) {
                if (active) {
                    fprintf(stderr, "Carrier \"%s\" cannot be used this way, try \"tcp+recv.%s\" instead.\n", carrierName.c_str(), carrierName.c_str());
                }
                close();
                return;
            }
            // Configure the carrier.
            if (!delegate->configure(*this)) {
                fprintf(stderr, "Carrier \"%s\" could not be configured.\n", carrierName.c_str());
                close();
                return;
            }
            delegate->prepareSend(*this);
        }
    }
}


bool Protocol::expectHeader()
{
    // A header, for historic reasons, is seen as
    // a protocol fingerprint (at least 8 bytes)
    // and the name of the sender.  In practice,
    // these callbacks have been stretched to the
    // point where their names have little bearing
    // on what the get used for.
    messageLen = 0;
    bool ok = expectProtocolSpecifier();
    if (!ok) {
        return false;
    }
    ok = expectSenderSpecifier();
    if (!ok) {
        return false;
    }
    yCAssert(PROTOCOL, delegate != nullptr);
    ok = delegate->expectExtraHeader(*this);
    return ok;
}


bool Protocol::expectProtocolSpecifier()
{
    // Historically YARP has used the first 8 bytes of
    // every connection as a way to identify it.  This
    // assumption is showing its age, and should really
    // be generalized.
    char buf[8];
    yarp::os::Bytes header((char*)&buf[0], sizeof(buf));
    yarp::conf::ssize_t len = is().readFull(header);
    if (len == -1) {
        yCDebug(PROTOCOL, "no connection");
        return false;
    }
    if ((size_t)len != header.length()) {
        yCDebug(PROTOCOL, "data stream died");
        return false;
    }
    bool already = false;
    if (delegate != nullptr) {
        if (delegate->checkHeader(header)) {
            already = true;
        }
    }
    if (!already) {
        delegate = Carriers::chooseCarrier(header);
        if (delegate == nullptr) {
            // Carrier not found; send a human-readable message.
            std::string msg = "* Error. Protocol not found.\r\n* Hello. You appear to be trying to communicate with a YARP Port.\r\n* The first 8 bytes sent to a YARP Port are critical for identifying the\r\n* protocol you wish to speak.\r\n* The first 8 bytes you sent were not associated with any particular protocol.\r\n* If you are a human, try typing \"CONNECT foo\" followed by a <RETURN>.\r\n* The 8 bytes \"CONNECT \" correspond to a simple text-mode protocol.\r\n* Goodbye.\r\n";
            yarp::os::Bytes b((char*)msg.c_str(), msg.length());
            os().write(b);
            os().flush();
        }
    }
    if (delegate == nullptr) {
        yCDebug(PROTOCOL, "unrecognized protocol");
        return false;
    }
    Route r = getRoute();
    r.setCarrierName(delegate->getName());
    setRoute(r);
    delegate->setParameters(header);
    return true;
}


bool Protocol::expectSenderSpecifier()
{
    yCAssert(PROTOCOL, delegate != nullptr);
    return delegate->expectSenderSpecifier(*this);
}


bool Protocol::sendHeader()
{
    yCAssert(PROTOCOL, delegate != nullptr);
    return delegate->sendHeader(*this);
}


bool Protocol::expectReplyToHeader()
{
    yCAssert(PROTOCOL, delegate != nullptr);
    return delegate->expectReplyToHeader(*this);
}


bool Protocol::respondToIndex()
{
    return true;
}

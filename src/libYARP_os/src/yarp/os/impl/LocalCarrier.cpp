/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/LocalCarrier.h>

#include <yarp/os/ConnectionState.h>
#include <yarp/os/Portable.h>
#include <yarp/os/Route.h>
#include <yarp/os/SizedWriter.h>

#include <yarp/os/impl/LogComponent.h>

using namespace yarp::os;

namespace {
YARP_OS_LOG_COMPONENT(LOCALCARRIER, "yarp.os.impl.LocalCarrier")
} // namespace

yarp::os::impl::LocalCarrierManager yarp::os::impl::LocalCarrier::manager;

yarp::os::impl::LocalCarrierManager::LocalCarrierManager() :
        senderMutex(),
        receiverMutex(),
        received(0),
        sender(nullptr),
        receiver(nullptr)
{
}

void yarp::os::impl::LocalCarrierManager::setSender(LocalCarrier* sender)
{
    senderMutex.lock();
    this->sender = sender;
}

yarp::os::impl::LocalCarrier* yarp::os::impl::LocalCarrierManager::getReceiver()
{
    received.wait();
    LocalCarrier* result = receiver;
    sender = nullptr;
    senderMutex.unlock();
    return result;
}

yarp::os::impl::LocalCarrier* yarp::os::impl::LocalCarrierManager::getSender(LocalCarrier* receiver)
{
    receiverMutex.lock();
    this->receiver = receiver;
    LocalCarrier* result = sender;
    received.post();
    receiverMutex.unlock();
    return result;
}

void yarp::os::impl::LocalCarrierManager::revoke(LocalCarrier* carrier)
{
    if (sender == carrier) {
        senderMutex.unlock();
    }
}


void yarp::os::impl::LocalCarrierStream::attach(LocalCarrier* owner, bool sender)
{
    this->owner = owner;
    this->sender = sender;
    done = false;
}

InputStream& yarp::os::impl::LocalCarrierStream::getInputStream()
{
    return *this;
}

OutputStream& yarp::os::impl::LocalCarrierStream::getOutputStream()
{
    return *this;
}

const Contact& yarp::os::impl::LocalCarrierStream::getLocalAddress() const
{
    return localAddress;
}

const Contact& yarp::os::impl::LocalCarrierStream::getRemoteAddress() const
{
    return remoteAddress;
}

bool yarp::os::impl::LocalCarrierStream::setTypeOfService(int tos)
{
    YARP_UNUSED(tos);
    return true;
}

yarp::conf::ssize_t yarp::os::impl::LocalCarrierStream::read(yarp::os::Bytes& b)
{
    yCAssert(LOCALCARRIER, false);
    return b.length();
}

void yarp::os::impl::LocalCarrierStream::write(const yarp::os::Bytes& b)
{
    YARP_UNUSED(b);
    yCAssert(LOCALCARRIER, false);
}

void yarp::os::impl::LocalCarrierStream::reset()
{
}

void yarp::os::impl::LocalCarrierStream::beginPacket()
{
}

void yarp::os::impl::LocalCarrierStream::endPacket()
{
}

void yarp::os::impl::LocalCarrierStream::interrupt()
{
    done = true;
}

void yarp::os::impl::LocalCarrierStream::close()
{
    if (owner != nullptr) {
        LocalCarrier* owned = owner;
        owner = nullptr;
        owned->shutdown();
    }
    done = true;
}

bool yarp::os::impl::LocalCarrierStream::isOk() const
{
    return !done;
}


yarp::os::impl::LocalCarrier::LocalCarrier() :
        peerMutex(), sent(0), received(0)
{
    ref = nullptr;
    peer = nullptr;
    doomed = false;
}

yarp::os::impl::LocalCarrier::~LocalCarrier()
{
    shutdown();
}

yarp::os::Carrier* yarp::os::impl::LocalCarrier::create() const
{
    return new LocalCarrier();
}

void yarp::os::impl::LocalCarrier::removePeer()
{
    if (!doomed) {
        peerMutex.lock();
        peer = nullptr;
        peerMutex.unlock();
    }
}

void yarp::os::impl::LocalCarrier::shutdown()
{
    if (!doomed) {
        doomed = true;
        peerMutex.lock();
        if (peer != nullptr) {
            peer->accept(nullptr);
            LocalCarrier* wasPeer = peer;
            peer = nullptr;
            wasPeer->removePeer();
        }
        peerMutex.unlock();
    }
}

std::string yarp::os::impl::LocalCarrier::getName() const
{
    return "local";
}

bool yarp::os::impl::LocalCarrier::requireAck() const
{
    return false;
}

bool yarp::os::impl::LocalCarrier::isConnectionless() const
{
    return false;
}

bool yarp::os::impl::LocalCarrier::canEscape() const
{
    return false;
}

bool yarp::os::impl::LocalCarrier::isLocal() const
{
    return true;
}

std::string yarp::os::impl::LocalCarrier::getSpecifierName() const
{
    return "LOCALITY";
}

bool yarp::os::impl::LocalCarrier::checkHeader(const Bytes& header)
{
    if (header.length() == 8) {
        std::string target = getSpecifierName();
        for (int i = 0; i < 8; i++) {
            if (!(target[i] == header.get()[i])) {
                return false;
            }
        }
        return true;
    }
    return false;
}

void yarp::os::impl::LocalCarrier::getHeader(Bytes& header) const
{
    if (header.length() == 8) {
        std::string target = getSpecifierName();
        for (int i = 0; i < 8; i++) {
            header.get()[i] = target[i];
        }
    }
}

void yarp::os::impl::LocalCarrier::setParameters(const Bytes& header)
{
    YARP_UNUSED(header);
}

bool yarp::os::impl::LocalCarrier::sendHeader(ConnectionState& proto)
{
    portName = proto.getRoute().getFromName();

    manager.setSender(this);

    defaultSendHeader(proto);
    // now switch over to some local structure to communicate
    peerMutex.lock();
    peer = manager.getReceiver();
    yCDebug(LOCALCARRIER,
            "sender %p sees receiver %p",
            this,
            peer);
    peerMutex.unlock();

    return true;
}

bool yarp::os::impl::LocalCarrier::expectExtraHeader(ConnectionState& proto)
{
    portName = proto.getRoute().getToName();
    // switch over to some local structure to communicate
    peerMutex.lock();
    peer = manager.getSender(this);
    yCDebug(LOCALCARRIER,
            "receiver %p (%s) sees sender %p (%s)",
            this,
            portName.c_str(),
            peer,
            peer->portName.c_str());
    Route route = proto.getRoute();
    route.setFromName(peer->portName);
    proto.setRoute(route);
    peerMutex.unlock();

    return true;
}

bool yarp::os::impl::LocalCarrier::becomeLocal(ConnectionState& proto, bool sender)
{
    auto* stream = new LocalCarrierStream();
    if (stream != nullptr) {
        stream->attach(this, sender);
    }
    proto.takeStreams(stream);
    return true;
}

bool yarp::os::impl::LocalCarrier::write(ConnectionState& proto, SizedWriter& writer)
{
    YARP_UNUSED(proto);
    yarp::os::Portable* ref = writer.getReference();
    if (ref != nullptr) {
        peerMutex.lock();
        if (peer != nullptr) {
            peer->accept(ref);
        } else {
            yCError(LOCALCARRIER, "local send failed - write without peer");
        }
        peerMutex.unlock();
    } else {
        yCError(LOCALCARRIER, "local send failed - no object");
    }

    return true;
}

bool yarp::os::impl::LocalCarrier::respondToHeader(ConnectionState& proto)
{
    // I am the receiver
    return becomeLocal(proto, false);
}


bool yarp::os::impl::LocalCarrier::expectReplyToHeader(ConnectionState& proto)
{
    // I am the sender
    return becomeLocal(proto, true);
}

bool yarp::os::impl::LocalCarrier::expectIndex(ConnectionState& proto)
{

    yCDebug(LOCALCARRIER, "local recv: wait send");
    sent.wait();
    yCDebug(LOCALCARRIER, "local recv: got send");
    proto.setReference(ref);
    received.post();
    if (ref != nullptr) {
        yCDebug(LOCALCARRIER, "local recv: received");
    } else {
        yCDebug(LOCALCARRIER, "local recv: shutdown");
        proto.is().interrupt();
        return false;
    }

    return true;
}

void yarp::os::impl::LocalCarrier::accept(yarp::os::Portable* ref)
{
    this->ref = ref;
    yCDebug(LOCALCARRIER, "local send: send ref");
    sent.post();
    if (ref != nullptr && !doomed) {
        yCDebug(LOCALCARRIER, "local send: wait receipt");
        received.wait();
        yCDebug(LOCALCARRIER, "local send: received");
    }
}

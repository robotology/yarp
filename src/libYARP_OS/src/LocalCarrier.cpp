// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2007 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <yarp/os/impl/LocalCarrier.h>
#include <yarp/os/impl/Protocol.h>
#include <yarp/os/Portable.h>


yarp::os::impl::LocalCarrierManager yarp::os::impl::LocalCarrier::manager;



yarp::os::impl::LocalCarrierManager::LocalCarrierManager() :
        senderMutex(1),
        receiverMutex(1),
        received(0) {
    sender = receiver = NULL;
}

void yarp::os::impl::LocalCarrierManager::setSender(LocalCarrier *sender) {
    senderMutex.wait();
    this->sender = sender;
}

yarp::os::impl::LocalCarrier *yarp::os::impl::LocalCarrierManager::getReceiver() {
    received.wait();
    LocalCarrier *result = receiver;
    sender = NULL;
    senderMutex.post();
    return result;
}

yarp::os::impl::LocalCarrier *yarp::os::impl::LocalCarrierManager::getSender(LocalCarrier *receiver) {
    receiverMutex.wait();
    this->receiver = receiver;
    LocalCarrier *result = sender;
    received.post();
    receiverMutex.post();
    return result;
}

void yarp::os::impl::LocalCarrierManager::revoke(LocalCarrier *carrier) {
    if (sender == carrier) {
        senderMutex.post();
    }
}


void yarp::os::impl::LocalCarrierStream::attach(LocalCarrier *owner, bool sender) {
    this->owner = owner;
    this->sender = sender;
    done = false;
}

void yarp::os::impl::LocalCarrierStream::interrupt() {
    done = true;
}

void yarp::os::impl::LocalCarrierStream::close() {
    SocketTwoWayStream::close();
    if (owner!=NULL) {
        LocalCarrier *owned = owner;
        owner = NULL;
        owned->shutdown();
    }
    done = true;
}

bool yarp::os::impl::LocalCarrierStream::isOk() {
    return !done;
}


yarp::os::impl::LocalCarrier::LocalCarrier() : peerMutex(1), sent(0), received(0) {
    ref = NULL;
    peer = NULL;
    doomed = false;
}

yarp::os::impl::LocalCarrier::~LocalCarrier() {
    shutdown();
}

yarp::os::impl::Carrier *yarp::os::impl::LocalCarrier::create() {
    return new LocalCarrier();
}

void yarp::os::impl::LocalCarrier::removePeer() {
    if (!doomed) {
        peerMutex.wait();
        peer = NULL;
        peerMutex.post();
    }
}

void yarp::os::impl::LocalCarrier::shutdown() {
    if (!doomed) {
        doomed = true;
        peerMutex.wait();
        if (peer!=NULL) {
            peer->accept(NULL);
            LocalCarrier *wasPeer = peer;
            peer = NULL;
            wasPeer->removePeer();
        }
        peerMutex.post();
    }
}

yarp::os::impl::String yarp::os::impl::LocalCarrier::getName() {
    return "local";
}

bool yarp::os::impl::LocalCarrier::requireAck() {
    return false;
}

bool yarp::os::impl::LocalCarrier::isConnectionless() {
    return false;
}

bool yarp::os::impl::LocalCarrier::canEscape() {
    return false;
}

bool yarp::os::impl::LocalCarrier::isLocal() {
    return true;
}

yarp::os::impl::String yarp::os::impl::LocalCarrier::getSpecifierName() {
    return "LOCALITY";
}

bool yarp::os::impl::LocalCarrier::checkHeader(const Bytes& header) {
    if (header.length()==8) {
        String target = getSpecifierName();
        for (int i=0; i<8; i++) {
            if (!(target[i]==header.get()[i])) {
                return false;
            }
        }
        return true;
    }
    return false;
}

void yarp::os::impl::LocalCarrier::getHeader(const Bytes& header) {
    if (header.length()==8) {
        String target = getSpecifierName();
        for (int i=0; i<8; i++) {
            header.get()[i] = target[i];
        }
    }
}

void yarp::os::impl::LocalCarrier::setParameters(const Bytes& header) {
}

bool yarp::os::impl::LocalCarrier::sendHeader(Protocol& proto) {
    portName = proto.getRoute().getFromName();

    manager.setSender(this);

    defaultSendHeader(proto);
    // now switch over to some local structure to communicate
    peerMutex.wait();
    peer = manager.getReceiver();
    //printf("sender %ld sees receiver %ld\n", (long int) this,
    //       (long int) peer);
    peerMutex.post();

    return true;
}

bool yarp::os::impl::LocalCarrier::expectExtraHeader(Protocol& proto) {
    portName = proto.getRoute().getToName();
    // switch over to some local structure to communicate
    peerMutex.wait();
    peer = manager.getSender(this);
    //printf("receiver %ld (%s) sees sender %ld (%s)\n",
    //       (long int) this, portName.c_str(),
    //       (long int) peer, peer->portName.c_str());
    proto.setRoute(proto.getRoute().addFromName(peer->portName));
    peerMutex.post();

    return true;
}

bool yarp::os::impl::LocalCarrier::becomeLocal(Protocol& proto, bool sender) {
    LocalCarrierStream *stream = new LocalCarrierStream();
    if (stream!=NULL) {
        stream->attach(this,sender);
    }
    proto.takeStreams(stream);
    //YARP_ERROR(Logger::get(),"*** don't trust local carrier yet ****");
    //ACE_OS::exit(1);
    return true;
}

bool yarp::os::impl::LocalCarrier::write(Protocol& proto, SizedWriter& writer) {

    yarp::os::Portable *ref = writer.getReference();
    if (ref!=NULL) {
        peerMutex.wait();
        if (peer!=NULL) {
            peer->accept(ref);
        } else {
            YARP_ERROR(Logger::get(),
                        "local send failed - write without peer");
        }
        peerMutex.post();
    } else {
        YARP_ERROR(Logger::get(),
                    "local send failed - no object");
    }

    return true;
}

bool yarp::os::impl::LocalCarrier::respondToHeader(Protocol& proto) {
    // i am the receiver

    return becomeLocal(proto,false);
}


bool yarp::os::impl::LocalCarrier::expectReplyToHeader(Protocol& proto) {
    // i am the sender

    return becomeLocal(proto,true);
}

bool yarp::os::impl::LocalCarrier::expectIndex(Protocol& proto) {

    YARP_DEBUG(Logger::get(),"local recv: wait send");
    sent.wait();
    YARP_DEBUG(Logger::get(),"local recv: got send");
    proto.setReference(ref);
    received.post();
    if (ref!=NULL) {
        YARP_DEBUG(Logger::get(),"local recv: received");
    } else {
        YARP_DEBUG(Logger::get(),"local recv: shutdown");
        proto.is().interrupt();
        return false;
    }

    return true;
}

void yarp::os::impl::LocalCarrier::accept(yarp::os::Portable *ref) {
    this->ref = ref;
    YARP_DEBUG(Logger::get(),"local send: send ref");
    sent.post();
    if (ref!=NULL&&!doomed) {
        YARP_DEBUG(Logger::get(),"local send: wait receipt");
        received.wait();
        YARP_DEBUG(Logger::get(),"local send: received");
    }
}

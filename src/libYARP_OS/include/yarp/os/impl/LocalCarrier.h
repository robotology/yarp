// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_LOCALCARRIER_
#define _YARP2_LOCALCARRIER_

#include <yarp/os/impl/AbstractCarrier.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Portable.h>
#include <yarp/os/impl/SocketTwoWayStream.h>

namespace yarp {
    namespace os {
        namespace impl {
            class LocalCarrier;
            class LocalCarrierManager;
            class LocalCarrierStream;
        }
    }
}

/**
 *
 * Coordinate ports communicating locally within a process.
 *
 */
class yarp::os::impl::LocalCarrierManager {
private:
    yarp::os::Semaphore senderMutex, receiverMutex, received;
    LocalCarrier *sender, *receiver;
public:
    LocalCarrierManager() : senderMutex(1), receiverMutex(1),
                            received(0) {
        sender = receiver = NULL;
    }

    void setSender(LocalCarrier *sender) {
        senderMutex.wait();
        this->sender = sender;
    }

    LocalCarrier *getReceiver() {
        received.wait();
        LocalCarrier *result = receiver;
        sender = NULL;
        senderMutex.post();
        return result;
    }

    LocalCarrier *getSender(LocalCarrier *receiver) {
        receiverMutex.wait();
        this->receiver = receiver;
        LocalCarrier *result = sender;
        received.post();
        receiverMutex.post();
        return result;
    }

    void revoke(LocalCarrier *carrier) {
        if (sender == carrier) {
            senderMutex.post();
        }
    }
};


/**
 *
 * A stream for communicating locally within a process.
 *
 */
class yarp::os::impl::LocalCarrierStream : public SocketTwoWayStream {
private:
    LocalCarrier *owner;
    bool sender;
    bool done;
public:
    void attach(LocalCarrier *owner, bool sender) {
        this->owner = owner;
        this->sender = sender;
        done = false;
    }

    virtual void interrupt() {
        done = true;
    }

    virtual void close();

    virtual bool isOk() {
        return !done;
    }

};

/**
 *
 * A carrier for communicating locally within a process.
 *
 */
class yarp::os::impl::LocalCarrier : public AbstractCarrier {
protected:
    bool doomed;
    yarp::os::Portable *ref;
    LocalCarrier *peer;
    yarp::os::Semaphore peerMutex;
    yarp::os::Semaphore sent, received;
    String portName;

    static LocalCarrierManager manager;

public:

    LocalCarrier() : peerMutex(1), sent(0), received(0) {
        ref = NULL;
        peer = NULL;
        doomed = false;
    }

    virtual ~LocalCarrier() {
        shutdown();
    }

    virtual Carrier *create() {
        return new LocalCarrier();
    }

    void removePeer() {
        if (!doomed) {
            peerMutex.wait();
            peer = NULL;
            peerMutex.post();
        }
    }

    void shutdown() {
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

    virtual String getName() {
        return "local";
    }

    virtual bool requireAck() {
        return false;
    }

    virtual bool isConnectionless() {
        return false;
    }

    virtual bool canEscape() {
        return false;
    }

    virtual bool isLocal() {
        return true;
    }

    virtual String getSpecifierName() {
        return "LOCALITY";
    }

    virtual bool checkHeader(const Bytes& header) {
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

    virtual void getHeader(const Bytes& header) {
        if (header.length()==8) {
            String target = getSpecifierName();
            for (int i=0; i<8; i++) {
                header.get()[i] = target[i];
            }
        }
    }

    virtual void setParameters(const Bytes& header) {
    }

    virtual bool sendHeader(Protocol& proto) {
        portName = proto.getRoute().getFromName();

        manager.setSender(this);

        proto.defaultSendHeader();
        // now switch over to some local structure to communicate
        peerMutex.wait();
        peer = manager.getReceiver();
        //printf("sender %ld sees receiver %ld\n", (long int) this,
        //       (long int) peer);
        peerMutex.post();

        return true;
    }

    virtual bool expectExtraHeader(Protocol& proto) {
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

    virtual bool becomeLocal(Protocol& proto, bool sender) {
        LocalCarrierStream *stream = new LocalCarrierStream();
        if (stream!=NULL) {
            stream->attach(this,sender);
        }
        proto.takeStreams(stream);
        //YARP_ERROR(Logger::get(),"*** don't trust local carrier yet ****");
        //ACE_OS::exit(1);
        return true;
    }

    virtual bool write(Protocol& proto, SizedWriter& writer) {

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

    virtual bool respondToHeader(Protocol& proto) {
        // i am the receiver

        return becomeLocal(proto,false);
    }


    virtual bool expectReplyToHeader(Protocol& proto) {
        // i am the sender

        return becomeLocal(proto,true);
    }

    virtual bool expectIndex(Protocol& proto) {

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

    void accept(yarp::os::Portable *ref) {
        this->ref = ref;
        YARP_DEBUG(Logger::get(),"local send: send ref");
        sent.post();
        if (ref!=NULL&&!doomed) {
            YARP_DEBUG(Logger::get(),"local send: wait receipt");
            received.wait();
            YARP_DEBUG(Logger::get(),"local send: received");
        }
    }

};

#endif

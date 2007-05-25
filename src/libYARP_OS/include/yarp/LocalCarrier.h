// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_LOCALCARRIER_
#define _YARP2_LOCALCARRIER_

#include <yarp/AbstractCarrier.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Portable.h>

namespace yarp {
    class LocalCarrier;
    class LocalCarrierManager;
}


class yarp::LocalCarrierManager {
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

class yarp::LocalCarrier : public AbstractCarrier {
protected:
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
    }

    virtual ~LocalCarrier() {
        peerMutex.wait();
        if (peer!=NULL) {
            peer->accept(NULL);
            LocalCarrier *wasPeer = peer;
            peer = NULL;
            wasPeer->removePeer();
        }
        peerMutex.post();
    }

    virtual Carrier *create() {
        return new LocalCarrier();
    }

    void removePeer() {
        peerMutex.wait();
        peer = NULL;
        peerMutex.post();        
    }

    virtual String getName() {
        return "local";
    }

    virtual bool requireAck() {
        return true;
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

    virtual void sendHeader(Protocol& proto) {
        portName = proto.getRoute().getFromName();

        manager.setSender(this);

        proto.defaultSendHeader();
        // now switch over to some local structure to communicate
        peerMutex.wait();
        peer = manager.getReceiver();
        //printf("sender %ld sees receiver %ld\n", (long int) this,
        //       (long int) peer);
        peerMutex.post();
    }

    virtual void expectExtraHeader(Protocol& proto) {
        portName = proto.getRoute().getToName();
        // switch over to some local structure to communicate
        peerMutex.wait();
        peer = manager.getSender(this);
        //printf("receiver %ld (%s) sees sender %ld (%s)\n", 
        //       (long int) this, portName.c_str(),
        //       (long int) peer, peer->portName.c_str());
        proto.setRoute(proto.getRoute().addFromName(peer->portName));
        peerMutex.post();
    }

    virtual void becomeLocal(Protocol& proto, bool sender) {
        //proto.takeStreams(NULL); // free up port from tcp
        YARP_DEBUG(Logger::get(),"*** local carrier is experimental ****");
        //ACE_OS::exit(1);
    }

    virtual void write(Protocol& proto, SizedWriter& writer) {
        // will need to modify that so that we can work
        // through a shared object reference instead (the target
        // is in the same process space as we are)

        // ideally, we'd just pass a reference to the writer object
        // and have it queried on the other side -- need to work a
        // bit on how SizedWriter operates so that this becomes efficiently
        // doable

        //proto.sendIndex();
        //proto.sendContent();
        //proto.expectAck();

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
    }

    virtual void respondToHeader(Protocol& proto) {
        // i am the receiver

        becomeLocal(proto,false);
    }


    virtual void expectReplyToHeader(Protocol& proto) {
        // i am the sender

        becomeLocal(proto,true);
    }

    virtual void expectIndex(Protocol& proto) {

        YARP_DEBUG(Logger::get(),"local recv: wait send");
        sent.wait();
        YARP_DEBUG(Logger::get(),"local recv: got send");
        proto.setReference(ref);
        received.post();
        if (ref!=NULL) {
            YARP_DEBUG(Logger::get(),"local recv: received");
        } else {
            YARP_DEBUG(Logger::get(),"local recv: shutdown");
        }
    }

    void accept(yarp::os::Portable *ref) {
        this->ref = ref;
        YARP_DEBUG(Logger::get(),"local send: send ref");
        sent.post();
        YARP_DEBUG(Logger::get(),"local send: wait receipt");
        received.wait();
        YARP_DEBUG(Logger::get(),"local send: received");
    }
};

#endif


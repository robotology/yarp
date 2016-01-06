// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_LOCALCARRIER
#define YARP2_LOCALCARRIER

#include <yarp/os/AbstractCarrier.h>
#include <yarp/os/impl/SocketTwoWayStream.h>
#include <yarp/os/Semaphore.h>


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
 * Coordinate ports communicating locally within a process.
 */
class yarp::os::impl::LocalCarrierManager {
public:
    LocalCarrierManager();

    void setSender(LocalCarrier *sender);
    LocalCarrier *getReceiver();
    LocalCarrier *getSender(LocalCarrier *receiver);
    void revoke(LocalCarrier *carrier);

private:
    yarp::os::Semaphore senderMutex, receiverMutex, received;
    LocalCarrier *sender, *receiver;
};


/**
 * A stream for communicating locally within a process.
 */
class yarp::os::impl::LocalCarrierStream : public SocketTwoWayStream {
public:
    void attach(LocalCarrier *owner, bool sender);

    virtual void interrupt();
    virtual void close();
    virtual bool isOk();

private:
    LocalCarrier *owner;
    bool sender;
    bool done;
};

/**
 * A carrier for communicating locally within a process.
 */
class yarp::os::impl::LocalCarrier : public AbstractCarrier {
public:
    LocalCarrier();

    virtual ~LocalCarrier();

    virtual Carrier *create();

    virtual String getName();

    virtual bool requireAck();
    virtual bool isConnectionless();
    virtual bool canEscape();
    virtual bool isLocal();
    virtual String getSpecifierName();
    virtual bool checkHeader(const Bytes& header);
    virtual void getHeader(const Bytes& header);
    virtual void setParameters(const Bytes& header);
    virtual bool sendHeader(ConnectionState& proto);
    virtual bool expectExtraHeader(ConnectionState& proto);
    virtual bool becomeLocal(ConnectionState& proto, bool sender);
    virtual bool write(ConnectionState& proto, SizedWriter& writer);
    virtual bool respondToHeader(ConnectionState& proto);
    virtual bool expectReplyToHeader(ConnectionState& proto);
    virtual bool expectIndex(ConnectionState& proto);

    void removePeer();
    void shutdown();
    void accept(yarp::os::Portable *ref);

protected:
    bool doomed;
    yarp::os::Portable *ref;
    LocalCarrier *peer;
    yarp::os::Semaphore peerMutex;
    yarp::os::Semaphore sent, received;
    String portName;

    static LocalCarrierManager manager;
};

#endif

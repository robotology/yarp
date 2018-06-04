/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_LOCALCARRIER_H
#define YARP_OS_IMPL_LOCALCARRIER_H

#include <yarp/os/AbstractCarrier.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Mutex.h>


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
class yarp::os::impl::LocalCarrierManager
{
public:
    LocalCarrierManager();

    void setSender(LocalCarrier *sender);
    LocalCarrier *getReceiver();
    LocalCarrier *getSender(LocalCarrier *receiver);
    void revoke(LocalCarrier *carrier);

private:
    yarp::os::Mutex senderMutex;
    yarp::os::Mutex receiverMutex;
    yarp::os::Semaphore received;
    LocalCarrier *sender, *receiver;
};


/**
 * A stream for communicating locally within a process.
 */
class yarp::os::impl::LocalCarrierStream : public TwoWayStream,
                                           public InputStream,
                                           public OutputStream
{
public:
    void attach(LocalCarrier *owner, bool sender);

    virtual InputStream& getInputStream() override;
    virtual OutputStream& getOutputStream() override;
    virtual const Contact& getLocalAddress() const override;
    virtual const Contact& getRemoteAddress() const override;
    virtual bool setTypeOfService(int tos) override;

    using yarp::os::InputStream::read;
    virtual yarp::conf::ssize_t read(const yarp::os::Bytes& b) override;

    using yarp::os::OutputStream::write;
    virtual void write(const yarp::os::Bytes& b) override;

    virtual void reset() override;
    virtual void beginPacket() override;
    virtual void endPacket() override;
    virtual void interrupt() override;
    virtual void close() override;
    virtual bool isOk() const override;

private:
    Contact localAddress, remoteAddress;
    LocalCarrier *owner;
    bool sender;
    bool done;
};

/**
 * A carrier for communicating locally within a process.
 */
class yarp::os::impl::LocalCarrier : public AbstractCarrier
{
public:
    LocalCarrier();

    virtual ~LocalCarrier();

    virtual Carrier *create() override;

    virtual std::string getName() const override;

    virtual bool requireAck() const override;
    virtual bool isConnectionless() const override;
    virtual bool canEscape() const override;
    virtual bool isLocal() const override;
    virtual std::string getSpecifierName() const;
    virtual bool checkHeader(const Bytes& header) override;
    virtual void getHeader(Bytes& header) const override;
    virtual void setParameters(const Bytes& header) override;
    virtual bool sendHeader(ConnectionState& proto) override;
    virtual bool expectExtraHeader(ConnectionState& proto) override;
    virtual bool becomeLocal(ConnectionState& proto, bool sender);
    virtual bool write(ConnectionState& proto, SizedWriter& writer) override;
    virtual bool respondToHeader(ConnectionState& proto) override;
    virtual bool expectReplyToHeader(ConnectionState& proto) override;
    virtual bool expectIndex(ConnectionState& proto) override;

    void removePeer();
    void shutdown();
    void accept(yarp::os::Portable *ref);

protected:
    bool doomed;
    yarp::os::Portable *ref;
    LocalCarrier *peer;
    yarp::os::Mutex peerMutex;
    yarp::os::Semaphore sent;
    yarp::os::Semaphore received;
    std::string portName;

    static LocalCarrierManager manager;
};

#endif // YARP_OS_IMPL_LOCALCARRIER_H

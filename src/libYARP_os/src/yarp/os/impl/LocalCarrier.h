/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_LOCALCARRIER_H
#define YARP_OS_IMPL_LOCALCARRIER_H

#include <yarp/os/AbstractCarrier.h>
#include <yarp/os/InputStream.h>
#include <yarp/os/OutputStream.h>
#include <yarp/os/Portable.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/TwoWayStream.h>

#include <mutex>

namespace yarp {
namespace os {
namespace impl {

class LocalCarrier;

/**
 * Coordinate ports communicating locally within a process.
 */
class LocalCarrierManager
{
public:
    LocalCarrierManager();

    void setSender(LocalCarrier* sender);
    LocalCarrier* getReceiver();
    LocalCarrier* getSender(LocalCarrier* receiver);
    void revoke(LocalCarrier* carrier);

private:
    std::mutex senderMutex;
    std::mutex receiverMutex;
    yarp::os::Semaphore received;
    LocalCarrier *sender, *receiver;
};


/**
 * A stream for communicating locally within a process.
 */
class LocalCarrierStream :
        public TwoWayStream,
        public InputStream,
        public OutputStream
{
public:
    void attach(LocalCarrier* owner, bool sender);

    InputStream& getInputStream() override;
    OutputStream& getOutputStream() override;
    const Contact& getLocalAddress() const override;
    const Contact& getRemoteAddress() const override;
    bool setTypeOfService(int tos) override;

    using yarp::os::InputStream::read;
    yarp::conf::ssize_t read(yarp::os::Bytes& b) override;

    using yarp::os::OutputStream::write;
    void write(const yarp::os::Bytes& b) override;

    void reset() override;
    void beginPacket() override;
    void endPacket() override;
    void interrupt() override;
    void close() override;
    bool isOk() const override;

private:
    Contact localAddress, remoteAddress;
    LocalCarrier* owner;
    bool sender;
    bool done;
};

/**
 * A carrier for communicating locally within a process.
 */
class LocalCarrier :
        public AbstractCarrier
{
public:
    LocalCarrier();

    virtual ~LocalCarrier();

    Carrier* create() const override;

    std::string getName() const override;

    bool requireAck() const override;
    bool isConnectionless() const override;
    bool canEscape() const override;
    bool isLocal() const override;
    virtual std::string getSpecifierName() const;
    bool checkHeader(const Bytes& header) override;
    void getHeader(Bytes& header) const override;
    void setParameters(const Bytes& header) override;
    bool sendHeader(ConnectionState& proto) override;
    bool expectExtraHeader(ConnectionState& proto) override;
    virtual bool becomeLocal(ConnectionState& proto, bool sender);
    bool write(ConnectionState& proto, SizedWriter& writer) override;
    bool respondToHeader(ConnectionState& proto) override;
    bool expectReplyToHeader(ConnectionState& proto) override;
    bool expectIndex(ConnectionState& proto) override;

    void removePeer();
    void shutdown();
    void accept(yarp::os::Portable* ref);

protected:
    bool doomed;
    yarp::os::Portable* ref;
    LocalCarrier* peer;
    std::mutex peerMutex;
    yarp::os::Semaphore sent;
    yarp::os::Semaphore received;
    std::string portName;

    static LocalCarrierManager manager;
};

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_LOCALCARRIER_H

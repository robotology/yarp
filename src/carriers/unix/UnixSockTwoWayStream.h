/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_UNIX_UNIXSOCKTWOWAYSTREAM_H
#define YARP_UNIX_UNIXSOCKTWOWAYSTREAM_H

#include <yarp/os/ManagedBytes.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/TwoWayStream.h>

#include <mutex>

/**
 * A stream abstraction for unix socket communication.
 */
class UnixSockTwoWayStream :
        public yarp::os::TwoWayStream,
        public yarp::os::InputStream,
        public yarp::os::OutputStream
{

public:
    UnixSockTwoWayStream(const std::string& _socketPath = "");

    ~UnixSockTwoWayStream() override;

    InputStream& getInputStream() override;

    OutputStream& getOutputStream() override;

    const yarp::os::Contact& getLocalAddress() const override;

    const yarp::os::Contact& getRemoteAddress() const override;

    void interrupt() override;

    void close() override;

    using yarp::os::InputStream::read;
    yarp::conf::ssize_t read(yarp::os::Bytes& b) override;

    using yarp::os::OutputStream::write;
    void write(const yarp::os::Bytes& b) override;

    bool isOk() const override;

    void reset() override;

    void beginPacket() override;
    void endPacket() override;

    bool open(bool sender = false);
    void setLocalAddress(yarp::os::Contact& _localAddress);
    void setRemoteAddress(yarp::os::Contact& _remoteAddress);

private:
    bool closed{false};
    bool openedAsReader{false};
    yarp::os::Contact localAddress;
    yarp::os::Contact remoteAddress;
    bool happy{true};

    std::string socketPath;
    int reader_fd{-1};
    int sender_fd{-1};

    static constexpr size_t maxAttempts = 5;
    static constexpr double delayBetweenAttempts = 0.1;
};

#endif // YARP_UNIX_UNIXSOCKTWOWAYSTREAM_H

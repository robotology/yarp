/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_UNIX_UNIXSOCKTWOWAYSTREAM_H
#define YARP_UNIX_UNIXSOCKTWOWAYSTREAM_H

#include <yarp/os/TwoWayStream.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/Semaphore.h>

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
    UnixSockTwoWayStream(const std::string& _socketPath="");

    ~UnixSockTwoWayStream() override;

    InputStream& getInputStream() override;

    OutputStream& getOutputStream() override;

    const yarp::os::Contact& getLocalAddress() const override;

    const yarp::os::Contact& getRemoteAddress() const override;

    void interrupt() override;

    void close() override;

    void closeMain();

    using yarp::os::InputStream::read;
    yarp::conf::ssize_t read(yarp::os::Bytes& b) override;

    using yarp::os::OutputStream::write;
    void write(const yarp::os::Bytes& b) override;

    void flush() override;

    bool isOk() const override;

    void reset() override;

    void beginPacket() override;
    void endPacket() override;

    yarp::os::Bytes getMonitor();

    void setMonitor(const yarp::os::Bytes& data);

    void removeMonitor();

    bool open(bool sender = false);
    void setLocalAddress(yarp::os::Contact& _localAddress);
    void setRemoteAddress(yarp::os::Contact& _remoteAddress);

private:
    yarp::os::ManagedBytes monitor;
    bool closed {false};
    bool interrupting {false};
    bool reader {false};
    yarp::os::Contact localAddress;
    yarp::os::Contact remoteAddress;
    std::mutex mutex;
    bool happy {true};

    std::string socketPath;
    int fd {-1};
    int cl {-1};
};

#endif // YARP_UNIX_UNIXSOCKTWOWAYSTREAM_H

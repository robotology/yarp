/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2010 Daniel Krieg <krieg@fias.uni-frankfurt.de>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_MPISTREAM_H
#define YARP_MPISTREAM_H

#include <yarp/os/TwoWayStream.h>
#include <string>
#include <yarp/os/Bytes.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/NetType.h>

#include "MpiComm.h"

#include <string>
#include <iostream>


/**
 * Abstract base class for port communication via MPI.
 */
class MpiStream :
        public yarp::os::TwoWayStream,
        public yarp::os::InputStream,
        public yarp::os::OutputStream
{
protected:
    int readAvail, readAt;
    char* readBuffer;
    bool terminate;
    std::string name;
    MpiComm* comm;

    yarp::os::Contact local;
    yarp::os::Contact remote;
public:
    MpiStream(std::string name, MpiComm* comm);
    virtual ~MpiStream();

    using yarp::os::OutputStream::write;
    using yarp::os::InputStream::read;

    void close() override = 0;
    bool isOk() const override;
    void interrupt() override;
    ssize_t read(yarp::os::Bytes& b) override = 0;
    void write(const yarp::os::Bytes& b) override = 0;
    InputStream& getInputStream() override;
    OutputStream& getOutputStream() override;
    const yarp::os::Contact& getLocalAddress() const override;
    const yarp::os::Contact& getRemoteAddress() const override;
    void resetBuffer();
    void reset() override { resetBuffer();}
    void beginPacket() override;
    void endPacket() override;
};


#endif // YARP_MPISTREAM_H

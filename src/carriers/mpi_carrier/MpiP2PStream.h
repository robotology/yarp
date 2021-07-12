/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2010 Daniel Krieg <krieg@fias.uni-frankfurt.de>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_MPIP2PSTREAM_H
#define YARP_MPIP2PSTREAM_H

#include "MpiStream.h"

/**
 * Implements communication via MPI point-to-point send/receive.
 */
class MpiP2PStream :
        public MpiStream
{
public:
    MpiP2PStream(std::string name, MpiComm* comm) : MpiStream(name,comm) {};

    using MpiStream::write;
    using MpiStream::read;

    ssize_t read(yarp::os::Bytes& b) override;
    void write(const yarp::os::Bytes& b) override;
    void close() override {
        yCDebug(MPI_CARRIER, "[MpiP2PStream @ %s] Closing stream", name.c_str());
        terminate = true;
    }
};


#endif // YARP_MPIP2PSTREAM_H

/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2010 Daniel Krieg <krieg@fias.uni-frankfurt.de>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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

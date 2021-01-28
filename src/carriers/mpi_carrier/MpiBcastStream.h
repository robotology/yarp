/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2010 Daniel Krieg <krieg@fias.uni-frankfurt.de>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_MPIBCASTSTREAM_H
#define YARP_MPIBCASTSTREAM_H

#include "MpiStream.h"

#include <cstring>

#define CMD_JOIN -1
#define CMD_DISCONNECT -2

/**
 * Implements communication via MPI broadcast.
 */
class MpiBcastStream :
        public MpiStream
{
public:
    MpiBcastStream(std::string name, MpiComm* comm) : MpiStream(name, comm) {};
    ~MpiBcastStream() {
        yCTrace(MPI_CARRIER, "[MpiBcastStream @ %s] Destructor", name.c_str());
    }

    using MpiStream::write;
    using MpiStream::read;

    ssize_t read(yarp::os::Bytes& b) override;
    void write(const yarp::os::Bytes& b) override;
    void startJoin();
    void post() {
        comm->sema.post();
    }
    void close() override {
        yCDebug(MPI_CARRIER, "[MpiBcastStream @ %s] Closing stream", name.c_str());
    }
    void execCmd(int cmd);
};


#endif // YARP_MPIBCASTSTREAM_H

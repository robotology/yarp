/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2010 Daniel Krieg <krieg@fias.uni-frankfurt.de>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_MPIBCASTSTREAM
#define YARP_MPIBCASTSTREAM

#include <yarp/os/MpiStream.h>
#include <cstring>

#define CMD_JOIN -1
#define CMD_DISCONNECT -2

namespace yarp {
    namespace os {
        class MpiBcastStream;
    }
}

/**
 * Implements communication via MPI broadcast.
 *
 */
class yarp::os::MpiBcastStream : public MpiStream {

public:
    MpiBcastStream(std::string name, MpiComm* comm) : MpiStream(name, comm) {};
    ~MpiBcastStream() {
        #ifdef MPI_DEBUG
        printf("[MpiBcastStream @ %s] Destructor\n", name.c_str());
        #endif
    }

    using MpiStream::write;
    using MpiStream::read;

    ssize_t read(Bytes& b) override;
    void write(const Bytes& b) override;
    void startJoin();
    void post() {
        comm->sema.post();
    }
    void close() override {
        #ifdef MPI_DEBUG
        printf("[MpiBcastStream @ %s] Closing stream\n", name.c_str());
        #endif
    }
    void execCmd(int cmd);

};


#endif // YARP_MPIBCASTSTREAM

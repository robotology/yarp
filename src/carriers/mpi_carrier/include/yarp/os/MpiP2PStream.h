/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2010 Daniel Krieg <krieg@fias.uni-frankfurt.de>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_MPIP2PSTREAM
#define YARP_MPIP2PSTREAM

#include <yarp/os/MpiStream.h>

namespace yarp {
    namespace os {
        class MpiP2PStream;
    }
}

/**
 * Implements communication via MPI point-to-point send/receive.
 *
 */
class yarp::os::MpiP2PStream : public MpiStream {
public:
    MpiP2PStream(ConstString name, MpiComm* comm) : MpiStream(name,comm) {};

    using MpiStream::write;
    using MpiStream::read;

    ssize_t read(const Bytes& b) override;
    void write(const Bytes& b) override;
    void close() override {
        #ifdef MPI_DEBUG
        printf("[MpiP2PStream @ %s] Closing stream\n", name.c_str());
        #endif
        terminate = true;
    }
};


#endif // YARP_MPIP2PSTREAM

/*
 * Author: Daniel Krieg krieg@fias.uni-frankfurt.de
 * Copyright (C) 2010 Daniel Krieg
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
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

    ssize_t read(const Bytes& b);
    void write(const Bytes& b);
    void close() {
        #ifdef MPI_DEBUG
        printf("[MpiP2PStream @ %s] Closing stream\n", name.c_str());
        #endif
        terminate = true;
    }
};


#endif //_YARP_MPIP2PSTREAM_


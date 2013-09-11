// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Author: Daniel Krieg krieg@fias.uni-frankfurt.de
 * Copyright (C) 2010 Daniel Krieg
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP_MPIP2PSTREAM_
#define _YARP_MPIP2PSTREAM_

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


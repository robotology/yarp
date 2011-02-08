// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Author: Daniel Krieg krieg@fias.uni-frankfurt.de
 * Copyright (C) 2010 Daniel Krieg
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP_MPIP2PSTREAM_
#define _YARP_MPIP2PSTREAM_

#include <yarp/os/impl/MpiStream.h>

namespace yarp {
    namespace os {
        namespace impl {
            class MpiP2PStream;
        }
    }
}

/**
 * Send data via MPI.
 *
 * Uses the dynamic process management from the MPI-2 standard to
 * set up a communicator between the two processes.
 *
 * @bug No proper disconnection.
 * @warning Seems to work, but still experimental.
 */
class yarp::os::impl::MpiP2PStream : public MpiStream {
public:
    MpiP2PStream(String name, MpiComm* comm) : MpiStream(name,comm) {};
    int read(const Bytes& b);
    void write(const Bytes& b);
};


#endif //_YARP_MPIP2PSTREAM_


// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Author: Daniel Krieg krieg@fias.uni-frankfurt.de
 * Copyright (C) 2010 Daniel Krieg
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP_MPIBCASTSTREAM_
#define _YARP_MPIBCASTSTREAM_

#include <yarp/os/impl/MpiStream.h>



namespace yarp {
    namespace os {
        namespace impl {
            class MpiBcastStream;
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
class yarp::os::impl::MpiBcastStream : public MpiStream {

public:
    MpiBcastStream(String name, MpiComm* comm) : MpiStream(name, comm) {};
    int read(const Bytes& b);
    void write(const Bytes& b);
    void startJoin() {
        comm->sema.wait();
        int cmd = -1;
        MPI_Bcast(&cmd, 1, MPI_INT, 0,comm->comm);
    }
    void stopJoin() {
        comm->sema.post();
    }
};


#endif //_YARP_MPIBCASTSTREAM_


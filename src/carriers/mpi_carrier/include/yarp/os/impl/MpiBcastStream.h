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
#include <string.h>



namespace yarp {
    namespace os {
        namespace impl {
            class MpiBcastStream;
        }
    }
}

/**
 * Implements communication via MPI broadcast.
 *
 */
class yarp::os::impl::MpiBcastStream : public MpiStream {

public:
    MpiBcastStream(String name, MpiComm* comm) : MpiStream(name, comm) {};
    ~MpiBcastStream() {
        #ifdef MPI_DEBUG
        printf("[MpiBcastStream @ %s] Destructor\n", name.c_str());
        #endif
    }
    int read(const Bytes& b);
    void write(const Bytes& b);
    void startJoin();
    void post() {
        comm->sema.post();
    }
    void close() {
        #ifdef MPI_DEBUG
        printf("[MpiBcastStream @ %s] Closing stream\n", name.c_str());
        #endif
    }

};


#endif //_YARP_MPIBCASTSTREAM_


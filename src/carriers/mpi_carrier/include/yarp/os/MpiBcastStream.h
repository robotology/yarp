// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Author: Daniel Krieg krieg@fias.uni-frankfurt.de
 * Copyright (C) 2010 Daniel Krieg
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP_MPIBCASTSTREAM_
#define _YARP_MPIBCASTSTREAM_

#include <yarp/os/MpiStream.h>
#include <string.h>

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
    MpiBcastStream(ConstString name, MpiComm* comm) : MpiStream(name, comm) {};
    ~MpiBcastStream() {
        #ifdef MPI_DEBUG
        printf("[MpiBcastStream @ %s] Destructor\n", name.c_str());
        #endif
    }
    ssize_t read(const Bytes& b);
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
    void execCmd(int cmd);

};


#endif //_YARP_MPIBCASTSTREAM_


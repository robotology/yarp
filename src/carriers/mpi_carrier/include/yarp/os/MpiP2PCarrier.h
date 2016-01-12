// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Author: Daniel Krieg krieg@fias.uni-frankfurt.de
 * Copyright (C) 2010 Daniel Krieg
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP_MPIP2PCARRIER
#define YARP_MPIP2PCARRIER

#include <yarp/os/MpiCarrier.h>
#include <yarp/os/MpiP2PStream.h>

namespace yarp {
    namespace os {
        class MpiP2PCarrier;
    }
}

/**
 * Carrier for port communicating via MPI point-to-point send/receive.
 *
 * Generic communication for arbitrary topology. Allows replies.
 *
 * @warning Probably all processes terminate
 * if one terminates without proper disconnect.
 * @warning Seems to work, but still experimental.
 */
class yarp::os::MpiP2PCarrier : public MpiCarrier {
public:
    MpiP2PCarrier() : MpiCarrier() {
        target = "MPI_____";
    }

    virtual ~MpiP2PCarrier() {
    }

    virtual void close() {
        #ifdef MPI_DEBUG
        printf("[MpiP2PCarrier @ %s] Closing carrier \n", name.c_str() );
        #endif
        delete comm;
    }

    virtual Carrier *create() {
        return new MpiP2PCarrier();
    }

    virtual void createStream(bool sender) {
        comm = new MpiComm(route);
        stream = new MpiP2PStream(route, comm);
    }

    virtual ConstString getName() {
        return "mpi";
    }

    virtual bool supportReply() {
        return true;
    }
};

#endif //_YARP_MPIP2PCARRIER_

/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2010 Daniel Krieg <krieg@fias.uni-frankfurt.de>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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

    virtual void close() override {
        #ifdef MPI_DEBUG
        printf("[MpiP2PCarrier @ %s] Closing carrier \n", name.c_str() );
        #endif
        delete comm;
    }

    virtual Carrier *create() override {
        return new MpiP2PCarrier();
    }

    virtual void createStream(bool sender) override {
        comm = new MpiComm(route);
        stream = new MpiP2PStream(route, comm);
    }

    virtual ConstString getName() override {
        return "mpi";
    }

    virtual bool supportReply() override {
        return true;
    }
};

#endif // YARP_MPIP2PCARRIER

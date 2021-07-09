/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2010 Daniel Krieg <krieg@fias.uni-frankfurt.de>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_MPIP2PCARRIER_H
#define YARP_MPIP2PCARRIER_H

#include "MpiCarrier.h"
#include "MpiP2PStream.h"

/**
 * Carrier for port communicating via MPI point-to-point send/receive.
 *
 * Generic communication for arbitrary topology. Allows replies.
 *
 * @warning Probably all processes terminate
 * if one terminates without proper disconnect.
 * @warning Seems to work, but still experimental.
 */
class MpiP2PCarrier :
        public MpiCarrier
{
public:
    MpiP2PCarrier() :
            MpiCarrier()
    {
        target = "MPI_____";
    }

    virtual ~MpiP2PCarrier() {
    }

    void close() override {
        yCDebug(MPI_CARRIER, "[MpiP2PCarrier @ %s] Closing carrier", name.c_str() );
        delete comm;
    }

    Carrier *create() const override {
        return new MpiP2PCarrier();
    }

    void createStream(bool sender) override {
        comm = new MpiComm(route);
        stream = new MpiP2PStream(route, comm);
    }

    std::string getName() const override {
        return "mpi";
    }

    bool supportReply() const override {
        return true;
    }
};

#endif // YARP_MPIP2PCARRIER_H

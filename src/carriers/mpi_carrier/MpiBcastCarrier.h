/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2010 Daniel Krieg <krieg@fias.uni-frankfurt.de>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_MPIBCASTCARRIER_H
#define YARP_MPIBCASTCARRIER_H

#include "MpiCarrier.h"
#include "MpiBcastStream.h"

#include <yarp/os/Election.h>


/**
 * Carrier for port communicating via MPI broadcast.
 *
 * Effective collective operations in the one-sender-multiple-receiver scenario.
 * Allows only one-way communication (no replies).
 *
 * @warning Probably all processes terminate
 * if one terminates without proper disconnect.
 * @warning Seems to work, but still experimental.
 */
class MpiBcastCarrier :
        public MpiCarrier
{
private:
    static yarp::os::ElectionOf<yarp::os::PeerRecord<MpiBcastCarrier> >* caster;
    static yarp::os::ElectionOf<yarp::os::PeerRecord<MpiBcastCarrier> >& getCaster();

    bool electionMember;

public:
    MpiBcastCarrier() : MpiCarrier(), electionMember(false) {
        target = "MPIBCAST";
    }

    virtual ~MpiBcastCarrier();

    void close() override;

    Carrier *create() const override {
        return new MpiBcastCarrier();
    }

    void createStream(bool sender) override;

    std::string getName() const override {
        return "bcast";
    }

    bool supportReply() const override {
        return false;
    }

    bool isBroadcast() const override {
        return true;
    }

    void prepareDisconnect() override;

    bool expectReplyToHeader(yarp::os::ConnectionState&  proto) override {
        bool ok = MpiCarrier::expectReplyToHeader(proto);
        MpiBcastStream *mpiStream = dynamic_cast<MpiBcastStream*> (stream);
        if(mpiStream)
            mpiStream->post();
        return ok;
    }

    bool isActive() const override;

    virtual bool isElect() const;
};

#endif // YARP_MPIBCASTCARRIER_H

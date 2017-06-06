/*
 * Author: Daniel Krieg krieg@fias.uni-frankfurt.de
 * Copyright (C) 2010 Daniel Krieg
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP_MPIBCASTCARRIER
#define YARP_MPIBCASTCARRIER

#include <yarp/os/MpiCarrier.h>
#include <yarp/os/MpiBcastStream.h>

#include <yarp/os/Election.h>


namespace yarp {
    namespace os {
        class MpiBcastCarrier;
    }
}

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
class yarp::os::MpiBcastCarrier : public MpiCarrier {
private:
    static yarp::os::ElectionOf<yarp::os::PeerRecord<MpiBcastCarrier> >* caster;
    static yarp::os::ElectionOf<yarp::os::PeerRecord<MpiBcastCarrier> >& getCaster();

    bool electionMember;

public:
    MpiBcastCarrier() : MpiCarrier(), electionMember(false) {
        target = "MPIBCAST";
    }

    virtual ~MpiBcastCarrier();

    virtual void close() override;

    virtual Carrier *create() override {
        return new MpiBcastCarrier();
    }

    virtual void createStream(bool sender) override;

    virtual ConstString getName() override {
        return "bcast";
    }

    virtual bool supportReply() override {
        return false;
    }

    virtual bool isBroadcast() override {
        return true;
    }

    virtual void prepareDisconnect() override;

    virtual bool expectReplyToHeader(ConnectionState&  proto) override {
        bool ok = MpiCarrier::expectReplyToHeader(proto);
        dynamic_cast<MpiBcastStream*> (stream)->post();
        return ok;
    }

    virtual bool isActive() override;

    virtual bool isElect();
};

#endif //_YARP_MPIBCASTCARRIER_

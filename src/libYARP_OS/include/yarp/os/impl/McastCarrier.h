/*
 * Copyright (C) 2006, 2007 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_IMPL_MCASTCARRIER_H
#define YARP_OS_IMPL_MCASTCARRIER_H

#include <yarp/os/AbstractCarrier.h>
#include <yarp/os/impl/UdpCarrier.h>
#include <yarp/os/impl/DgramTwoWayStream.h>
#include <yarp/os/impl/Logger.h>

#include <yarp/os/Election.h>
#include <yarp/os/impl/SplitString.h>
#include <yarp/os/impl/PlatformSize.h>

#include <cstdio>

namespace yarp {
    namespace os {
        namespace impl {
            class McastCarrier;
        }
    }
}

/**
 * Communicating between two ports via MCAST.
 */
class yarp::os::impl::McastCarrier : public UdpCarrier {
protected:
    Contact mcastAddress;
    ConstString mcastName;
    ConstString key;

    static ElectionOf<PeerRecord<McastCarrier> > *caster;

    static ElectionOf<PeerRecord<McastCarrier> >& getCaster();

public:

    McastCarrier();

    virtual ~McastCarrier();

    virtual Carrier *create() override;
    virtual ConstString getName() override;

    virtual int getSpecifierCode() override;
    virtual bool sendHeader(ConnectionState& proto) override;
    virtual bool expectExtraHeader(ConnectionState& proto) override;
    virtual bool becomeMcast(ConnectionState& proto, bool sender);
    virtual bool respondToHeader(ConnectionState& proto) override;
    virtual bool expectReplyToHeader(ConnectionState& proto) override;

    void addSender(const ConstString& key);
    void addRemove(const ConstString& key);
    bool isElect();

    virtual bool isActive() override;
    virtual bool isBroadcast() override;
};

#endif // YARP_OS_IMPL_MCASTCARRIER_H

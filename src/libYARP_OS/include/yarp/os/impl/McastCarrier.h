// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2007 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_MCASTCARRIER_
#define _YARP2_MCASTCARRIER_

#include <yarp/os/AbstractCarrier.h>
#include <yarp/os/impl/UdpCarrier.h>
#include <yarp/os/impl/DgramTwoWayStream.h>
#include <yarp/os/impl/Logger.h>

#include <yarp/os/Election.h>
#include <yarp/os/impl/SplitString.h>
#include <yarp/os/impl/PlatformSize.h>

#include <stdio.h>

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
    String mcastName;
    String key;

    static ElectionOf<PeerRecord<McastCarrier> > *caster;

    static ElectionOf<PeerRecord<McastCarrier> >& getCaster();

public:

    McastCarrier();

    virtual ~McastCarrier();

    virtual Carrier *create();
    virtual String getName();

    virtual int getSpecifierCode();
    virtual bool sendHeader(ConnectionState& proto);
    virtual bool expectExtraHeader(ConnectionState& proto);
    virtual bool becomeMcast(ConnectionState& proto, bool sender);
    virtual bool respondToHeader(ConnectionState& proto);
    virtual bool expectReplyToHeader(ConnectionState& proto);

    void addSender(const String& key);
    void addRemove(const String& key);
    bool isElect();

    virtual bool isActive();
    virtual bool isBroadcast();
};

#endif

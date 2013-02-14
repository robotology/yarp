// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2007 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_MCASTCARRIER_
#define _YARP2_MCASTCARRIER_

#include <yarp/os/impl/AbstractCarrier.h>
#include <yarp/os/impl/UdpCarrier.h>
#include <yarp/os/impl/DgramTwoWayStream.h>
#include <yarp/os/impl/Logger.h>

#include <yarp/os/impl/Election.h>
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
    Address mcastAddress;
    String mcastName;
    String key;

    static ElectionOf<McastCarrier,PeerRecord> *caster;

    static ElectionOf<McastCarrier,PeerRecord>& getCaster();

public:

    McastCarrier();

    virtual ~McastCarrier();

    virtual Carrier *create();
    virtual String getName();

    virtual int getSpecifierCode();
    virtual bool sendHeader(Protocol& proto);
    virtual bool expectExtraHeader(Protocol& proto);
    virtual bool becomeMcast(Protocol& proto, bool sender);
    virtual bool respondToHeader(Protocol& proto);
    virtual bool expectReplyToHeader(Protocol& proto);

    void addSender(const String& key);
    void addRemove(const String& key);
    bool isElect();

    virtual bool isActive();
    virtual bool isBroadcast();
};

#endif

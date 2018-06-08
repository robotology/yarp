/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_MCASTCARRIER_H
#define YARP_OS_IMPL_MCASTCARRIER_H

#include <yarp/os/AbstractCarrier.h>
#include <yarp/os/impl/UdpCarrier.h>
#include <yarp/os/impl/DgramTwoWayStream.h>
#include <yarp/os/impl/Logger.h>

#include <yarp/os/Election.h>
#include <yarp/os/impl/SplitString.h>

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
    std::string mcastName;
    std::string key;
    DgramTwoWayStream *stream;
    Contact local;

    static ElectionOf<PeerRecord<McastCarrier> > *caster;

    static ElectionOf<PeerRecord<McastCarrier> >& getCaster();

public:

    McastCarrier();

    virtual ~McastCarrier();

    virtual Carrier *create() override;
    virtual std::string getName() override;

    virtual int getSpecifierCode() override;
    virtual bool sendHeader(ConnectionState& proto) override;
    virtual bool expectExtraHeader(ConnectionState& proto) override;
    virtual bool respondToHeader(ConnectionState& proto) override;
    virtual bool expectReplyToHeader(ConnectionState& proto) override;

    bool becomeMcast(ConnectionState& proto, bool sender);
    void addSender(const std::string& key);
    void removeSender(const std::string& key);
    bool isElect();
    /**
     * @brief takeElection, this function is called when the elect mcast
     * carrier dies and pass the write buffers to another one.
     * @return true if the join of mcast is successful false otherwise.
     */
    bool takeElection();

    virtual bool isActive() override;
    virtual bool isBroadcast() override;
};

#endif // YARP_OS_IMPL_MCASTCARRIER_H

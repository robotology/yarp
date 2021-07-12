/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_MCASTCARRIER_H
#define YARP_OS_IMPL_MCASTCARRIER_H

#include <yarp/os/AbstractCarrier.h>
#include <yarp/os/Election.h>
#include <yarp/os/impl/DgramTwoWayStream.h>
#include <yarp/os/impl/UdpCarrier.h>

#include <cstdio>

namespace yarp {
namespace os {
namespace impl {

/**
 * Communicating between two ports via MCAST.
 */
class McastCarrier :
        public UdpCarrier
{
protected:
    Contact mcastAddress;
    std::string mcastName;
    std::string key;
    DgramTwoWayStream* stream;
    Contact local;

    static ElectionOf<PeerRecord<McastCarrier>>* caster;

    static ElectionOf<PeerRecord<McastCarrier>>& getCaster();

public:
    McastCarrier();

    virtual ~McastCarrier();

    Carrier* create() const override;
    std::string getName() const override;

    int getSpecifierCode() const override;

    bool sendHeader(ConnectionState& proto) override;
    bool expectExtraHeader(ConnectionState& proto) override;
    bool respondToHeader(ConnectionState& proto) override;
    bool expectReplyToHeader(ConnectionState& proto) override;

    bool becomeMcast(ConnectionState& proto, bool sender);
    void addSender(const std::string& key);
    void removeSender(const std::string& key);
    bool isElect() const;
    /**
     * @brief takeElection, this function is called when the elect mcast
     * carrier dies and pass the write buffers to another one.
     * @return true if the join of mcast is successful false otherwise.
     */
    bool takeElection();

    bool isActive() const override;
    bool isBroadcast() const override;
};

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_MCASTCARRIER_H

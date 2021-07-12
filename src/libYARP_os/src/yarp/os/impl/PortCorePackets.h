/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_PORTCOREPACKETS_H
#define YARP_OS_IMPL_PORTCOREPACKETS_H

#include <yarp/os/impl/PortCorePacket.h>

#include <yarp/os/Log.h>

#include <list>

namespace yarp {
namespace os {
namespace impl {

/**
 * A collection of messages being transmitted over connections.
 * This tracks uses of the messages for memory management purposes.
 * We call messages "packets" for no particular reason.
 */
class PortCorePackets
{
private:
    std::list<PortCorePacket*> inactive; // unused packets we may reuse
    std::list<PortCorePacket*> active;   // a list of packets being sent
public:
    virtual ~PortCorePackets();

    /**
     * @return the number of packets currently being sent.
     */
    size_t getCount();

    /**
     * Get a packet that we can prepare for sending.  If a previously sent
     * packet that is not being used is available, we take that.  Otherwise
     * we create one.
     *
     * @return an unused or freshly created packet
     */
    PortCorePacket* getFreePacket();

    /**
     * Force the given packet into an inactive state.  See checkPacket() for
     * a less drastic way to nudge a packet onwards in its lifecycle.
     * @param packet the packet to work on
     * @param clear whether to reset the contents of the packet
     */
    void freePacket(PortCorePacket* packet, bool clear = true);

    /**
     * Send a completion notification if a packet has finished being
     * sent on all connections.
     * @param packet the packet to work on
     * @return true if the packet has finished being sent
     */
    bool completePacket(PortCorePacket* packet);

    /**
     * Move a packet to the inactive state if it has finished being
     * sent on all connections.
     * @param packet the packet to work on
     * @return true if the packet was made inactive
     */
    bool checkPacket(PortCorePacket* packet);
};


} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_PORTCOREPACKETS_H

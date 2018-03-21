/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_PORTCOREPACKETS_H
#define YARP_OS_IMPL_PORTCOREPACKETS_H

#include <yarp/os/impl/PortCorePacket.h>
#ifdef YARP_HAS_ACE
#  include <ace/config.h>
#  include <ace/String_Base.h>
#endif
#include <list>
#include <cstdio>

namespace yarp {
    namespace os {
        namespace impl {
            class PortCorePackets;
        }
    }
}

/**
 *
 * A collection of messages being transmitted over connections.
 * This tracks uses of the messages for memory management purposes.
 * We call messages "packets" for no particular reason.
 *
 */
class yarp::os::impl::PortCorePackets
{
private:
    std::list<PortCorePacket*> inactive; // unused packets we may reuse
    std::list<PortCorePacket*> active;   // a list of packets being sent
public:

    virtual ~PortCorePackets()
    {
        while (!inactive.empty()) {
            delete inactive.back();
            inactive.pop_back();
        }
        while (!active.empty()) {
            delete active.back();
            active.pop_back();
        }
    }

    /**
     * @return the number of packets currently being sent.
     */
    int getCount()
    {
        return (int)active.size();
    }

    /**
     * Get a packet that we can prepare for sending.  If a previously sent
     * packet that is not being used is available, we take that.  Otherwise
     * we create one.
     *
     * @return an unused or freshly created packet
     */
    PortCorePacket *getFreePacket()
    {
        if (inactive.empty()) {
            PortCorePacket *obj = nullptr;
            obj = new PortCorePacket();
            yAssert(obj!=nullptr);
            inactive.push_back(obj);
        }
        PortCorePacket *next = inactive.front();
        if (next == nullptr) {
            fprintf(stderr, "*** YARP consistency check failed.\n");
            fprintf(stderr, "*** There has been a low-level failure in \"PortCorePackets\".\n");
            fprintf(stderr, "*** This typically occurs when ports are accessed in a non-threadsafe way.\n");
            fprintf(stderr, "*** For help: https://github.com/robotology/yarp/issues/new\n");
            yAssert(1==0);
        }
        yAssert(next!=nullptr);
        inactive.remove(next);
        active.push_back(next);
        return next;
    }

    /**
     * Force the given packet into an inactive state.  See checkPacket() for
     * a less drastic way to nudge a packet onwards in its lifecycle.
     * @param packet the packet to work on
     * @param clear whether to reset the contents of the packet
     */
    void freePacket(PortCorePacket *packet, bool clear=true)
    {
        if (packet!=nullptr) {
            if (clear) {
                packet->reset();
            }
            packet->completed = true;
            active.remove(packet);
            inactive.push_back(packet);
        }
    }

    /**
     * Send a completion notification if a packet has finished being
     * sent on all connections.
     * @param packet the packet to work on
     * @return true if the packet has finished being sent
     */
    bool completePacket(PortCorePacket *packet)
    {
        if (packet!=nullptr) {
            if (packet->getCount()<=0) {
                packet->complete();
                return true;
            }
        }
        return false;
    }

    /**
     * Move a packet to the inactive state if it has finished being
     * sent on all connections.
     * @param packet the packet to work on
     * @return true if the packet was made inactive
     */
    bool checkPacket(PortCorePacket *packet)
    {
        if (packet!=nullptr) {
            if (packet->getCount()<=0) {
                packet->complete();
                freePacket(packet);
                return true;
            }
        }
        return false;
    }

};

#endif // YARP_OS_IMPL_PORTCOREPACKETS_H

/*
 * Copyright (C) 2006, 2008 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_PORTCOREPACKETS
#define YARP2_PORTCOREPACKETS

#include <yarp/os/impl/PortCorePacket.h>
#include <yarp/os/impl/PlatformList.h>
#ifdef YARP_HAS_ACE
#  include <ace/config.h>
#  include <ace/String_Base.h>
#endif
#include <stdio.h>

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
class yarp::os::impl::PortCorePackets {
private:
    PLATFORM_LIST(PortCorePacket) inactive; // unused packets we may reuse
    PLATFORM_LIST(PortCorePacket) active;   // a list of packets being sent
public:

    virtual ~PortCorePackets() {
        PLATFORM_LIST_CLEAR(inactive);
        PLATFORM_LIST_CLEAR(active);
    }

    /**
     *
     * @return the number of packets currently being sent.
     *
     */
    int getCount() {
        return (int)active.size();
    }

    /**
     *
     * Get a packet that we can prepare for sending.  If a previously sent
     * packet that is not being used is available, we take that.  Otherwise
     * we create one.
     *
     * @return an unused or freshly created packet
     *
     */
    PortCorePacket *getFreePacket() {
        if (PLATFORM_LIST_EMPTY(inactive)) {
            PortCorePacket *obj = NULL;
#if defined(YARP_HAS_ACE) && !defined(YARP_HAS_CXX11)
            size_t obj_size = sizeof (PortCorePacket);
            ACE_NEW_MALLOC_RETURN (obj,
                                   (PortCorePacket *)
                                   ACE_Allocator::instance()->malloc(obj_size),
                                   PortCorePacket(), 0);
#else
            obj = new PortCorePacket();
#endif
            yAssert(obj!=NULL);
            PLATFORM_LIST_PUSH_BACK(inactive,obj);
        }
        PortCorePacket *next = NULL;
        PLATFORM_LIST_GET(inactive,next);
        if (next==NULL) {
            fprintf(stderr,"*** YARP consistency check failed.\n");
            fprintf(stderr,"*** There has been a low-level failure in \"PortCorePackets\".\n");
            fprintf(stderr,"*** This typically occurs when ports are accessed in a non-threadsafe way.\n");
            fprintf(stderr,"*** For help: https://github.com/robotology/yarp/issues/new\n");
            yAssert(1==0);
        }
        yAssert(next!=NULL);
        inactive.remove(next);
        PLATFORM_LIST_PUSH_BACK(active,next);
        return next;
    }

    /**
     *
     * Force the given packet into an inactive state.  See checkPacket() for
     * a less drastic way to nudge a packet onwards in its lifecycle.
     * @param packet the packet to work on
     * @param clear whether to reset the contents of the packet
     *
     */
    void freePacket(PortCorePacket *packet, bool clear=true) {
        if (packet!=NULL) {
            if (clear) {
                packet->reset();
            }
            packet->completed = true;
            active.remove(packet);
            PLATFORM_LIST_PUSH_BACK(inactive,packet);
        }
    }

    /**
     *
     * Send a completion notification if a packet has finished being
     * sent on all connections.
     * @param packet the packet to work on
     * @return true if the packet has finished being sent
     *
     */
    bool completePacket(PortCorePacket *packet) {
        if (packet!=NULL) {
            if (packet->getCount()<=0) {
                packet->complete();
                return true;
            }
        }
        return false;
    }

    /**
     *
     * Move a packet to the inactive state if it has finished being
     * sent on all connections.
     * @param packet the packet to work on
     * @return true if the packet was made inactive
     *
     */
    bool checkPacket(PortCorePacket *packet) {
        if (packet!=NULL) {
            if (packet->getCount()<=0) {
                packet->complete();
                freePacket(packet);
                return true;
            }
        }
        return false;
    }

};

#endif

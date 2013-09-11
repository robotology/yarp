// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2008 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_PORTCOREPACKET_
#define _YARP2_PORTCOREPACKET_

#include <yarp/os/PortWriter.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/NetType.h>

#include <yarp/os/impl/PlatformList.h>
#ifdef YARP_HAS_ACE
#include <ace/config.h>
#include <ace/String_Base.h>
#endif

#include <stdio.h>

namespace yarp {
    namespace os {
        namespace impl {
            class PortCorePacket;
            class PortCorePackets;
        }
    }
}

/**
 *
 * A single message, potentially being transmitted on multiple connections.
 *
 */
class yarp::os::impl::PortCorePacket {
public:
    PortCorePacket *prev_, *next_;
    yarp::os::PortWriter *content;
    yarp::os::PortWriter *callback;
    int ct;
    bool owned;
    bool ownedCallback;
    bool completed;

    PortCorePacket() {
        prev_ = next_ = NULL;
        content = NULL;
        callback = NULL;
        owned = false;
        ownedCallback = false;
        reset();
    }

    virtual ~PortCorePacket() {
        complete();
        reset();
    }

    int getCount() {
        return ct;
    }

    void inc() {
        ct++;
    }

    void dec() {
        ct--;
    }

    yarp::os::PortWriter *getContent() {
        return content;
    }

    yarp::os::PortWriter *getCallback() {
        return (callback!=0/*NULL*/)?callback:content;
    }

    void setContent(yarp::os::PortWriter *writable, bool owned = false,
                    yarp::os::PortWriter *callback = NULL,
                    bool ownedCallback = false) {
        content = writable;
        this->callback = callback;
        ct = 1;
        this->owned = owned;
        this->ownedCallback = ownedCallback;
        completed = false;
    }

    void reset() {
        if (owned) {
            delete content;
        }
        if (ownedCallback) {
            delete callback;
        }
        content = NULL;
        callback = NULL;
        ct = 0;
        owned = false;
        ownedCallback = false;
        completed = false;
    }

    void complete() {
        if (!completed) {
            if (getContent()!=NULL) {
                YARP_DEBUG(Logger::get(), "Sending an onCompletion message");
                getCallback()->onCompletion();
                completed = true;
            }
        }
    }
};

/**
 *
 * A collection of messages being transmitted over connections.
 *
 */
class yarp::os::impl::PortCorePackets {
private:
    PLATFORM_LIST(PortCorePacket) inactive, active;
public:

    int getCount() {
        return (int)active.size();
    }

    PortCorePacket *getFreePacket() {
        //YARP_INFO(Logger::get(),String("inactive count is ") + 
        //     NetType::toString(inactive.size()));
        //YARP_INFO(Logger::get(),String("active count is ") + 
        //     NetType::toString(active.size()));
        if (PLATFORM_LIST_EMPTY(inactive)) {
            PortCorePacket *obj = NULL;
#ifdef YARP_HAS_ACE
            size_t obj_size = sizeof (PortCorePacket);
            ACE_NEW_MALLOC_RETURN (obj,
                                   (PortCorePacket *)
                                   ACE_Allocator::instance()->malloc(obj_size),
                                   PortCorePacket(), 0);
#else
            obj = new PortCorePacket();
#endif
            YARP_ASSERT(obj!=NULL);
            PLATFORM_LIST_PUSH_BACK(inactive,obj);
        }
        PortCorePacket *next = NULL;
        PLATFORM_LIST_GET(inactive,next);
        if (next==NULL) {
            fprintf(stderr,"*** YARP consistency check failed.\n");
            fprintf(stderr,"*** There has been a low-level failure in \"PortCorePackets\".\n");
            fprintf(stderr,"*** This typically occurs when ports are accessed in a non-threadsafe way.\n");
            fprintf(stderr,"*** If you need help debugging, email paulfitz@alum.mit.edu\n");

            /*
            //warning: casting size_t to unsigned int to remove annoying warning
            //did not find a better way to handle this that was portable across
            //different compilers (%z does not seem to be supported in gcc 4.1 
            //or msvc). Lorenzo
            fprintf(stderr,"  number of active packets: %u\n", (unsigned int) active.size());
            fprintf(stderr,"  number of inactive packets: %u\n", (unsigned int) inactive.size());
            fprintf(stderr,"  active packets:\n");

            unsigned int i=0;
            for (i=0; i<active.size(); i++) {
                PortCorePacket *p = NULL;
                active.get(p);
                fprintf(stderr,"    %ld %ld %ld / %d\n", (long int)p,
                        (long int)(p->getContent()),
                        (long int)(p->getCallback()),
                        p->getCount());
            }
            fprintf(stderr,"  inactive packets:\n");
            for (i=0; i<inactive.size(); i++) {
                PortCorePacket *p = NULL;
                inactive.get(p);
                fprintf(stderr,"    %ld %ld %ld / %d\n", (long int)p,
                        (long int)(p->getContent()),
                        (long int)(p->getCallback()),
                        p->getCount());
            }
            fflush(stderr);
            */
            YARP_ASSERT(1==0);
        }
        YARP_ASSERT(next!=NULL);
        inactive.remove(next);
        PLATFORM_LIST_PUSH_BACK(active,next);
        return next;
    }

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

    bool completePacket(PortCorePacket *packet) {
        if (packet!=NULL) {
            if (packet->getCount()<=0) {
                packet->complete();
                return true;
            }
        }
        return false;
    }

    bool checkPacket(PortCorePacket *packet) {
        if (packet!=NULL) {
            //YARP_INFO(Logger::get(), String("packet use count is ") +
            //NetType::toString(packet->getCount()));
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

// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2008 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_PORTCOREPACKET_
#define _YARP2_PORTCOREPACKET_

#include <yarp/Writable.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/NetType.h>

#include <ace/Containers_T.h>

#include <stdio.h>

namespace yarp {
    namespace os {
        namespace impl {
            class PortCorePacket;
            class PortCorePackets;
        }
    }
}

class yarp::os::impl::PortCorePacket {
public:
    PortCorePacket *prev_, *next_;
    Writable *content;
    Writable *callback;
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

    Writable *getContent() {
        return content;
    }

    Writable *getCallback() {
        return (callback!=0/*NULL*/)?callback:content;
    }

    void setContent(Writable *writable, bool owned = false,
                    Writable *callback = NULL,
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

class yarp::os::impl::PortCorePackets {
private:
    ACE_Double_Linked_List<PortCorePacket> inactive, active;
public:

    int getCount() {
        return active.size();
    }

    PortCorePacket *getFreePacket() {
        //YARP_INFO(Logger::get(),String("inactive count is ") + 
        //     NetType::toString(inactive.size()));
        //YARP_INFO(Logger::get(),String("active count is ") + 
        //     NetType::toString(active.size()));
        if (inactive.is_empty()) {
            size_t obj_size = sizeof (PortCorePacket);
            PortCorePacket *obj = NULL;
            ACE_NEW_MALLOC_RETURN (obj,
                                   (PortCorePacket *)
                                   ACE_Allocator::instance()->malloc(obj_size),
                                   PortCorePacket(), 0);
            YARP_ASSERT(obj!=NULL);
            inactive.insert_tail(obj);
        }
        PortCorePacket *next = NULL;
        inactive.get(next);
        if (next==NULL) {
            fprintf(stderr,"*** Internal YARP failure.\n");
            fprintf(stderr,"*** There has been a failure in \"PortCorePackets\".\n");
            fprintf(stderr,"*** This occurred occasionally in the past due to a bug.\n");
            fprintf(stderr,"*** That bug was believed to be fixed, but it looks like we were wrong.\n");
            fprintf(stderr,"*** Please email the following information to paulfitz@alum.mit.edu:\n");
            fprintf(stderr,"  number of active packets: %z\n", active.size());
            fprintf(stderr,"  number of inactive packets: %z\n", inactive.size());
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
            YARP_ASSERT(1==0);
        }
        YARP_ASSERT(next!=NULL);
        inactive.remove(next);
        active.insert_tail(next);
        return next;
    }

    void freePacket(PortCorePacket *packet, bool clear=true) {
        if (packet!=NULL) {
            if (clear) {
                packet->reset();
            }
            packet->completed = true;
            active.remove(packet);
            inactive.insert_tail(packet);
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

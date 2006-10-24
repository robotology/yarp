// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_PORTCOREPACKET_
#define _YARP2_PORTCOREPACKET_

#include <yarp/Writable.h>
#include <yarp/Logger.h>
#include <yarp/NetType.h>

#include <ace/Containers_T.h>

namespace yarp {
    class PortCorePacket;
    class PortCorePackets;
}

class yarp::PortCorePacket {
public:
    PortCorePacket *prev_, *next_;
    Writable *content;
    int ct;
    bool owned;
    bool completed;

    PortCorePacket() {
        prev_ = next_ = NULL;
        content = NULL;
        owned = false;
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

    void setContent(Writable *writable, bool owned = false) {
        content = writable;
        ct = 1;
        this->owned = owned;
        completed = false;
    }

    void reset() {
        if (owned) {
            delete content;
        }
        content = NULL;
        ct = 0;
        owned = false;
        completed = false;
    }

    void complete() {
        if (!completed) {
            if (getContent()!=NULL) {
                YARP_DEBUG(Logger::get(), "Sending an onCompletion message");
                getContent()->onCompletion();
                completed = true;
            }
        }
    }
};

class yarp::PortCorePackets {
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
            inactive.insert_tail(obj);
        }
        PortCorePacket *next = NULL;
        inactive.get(next);
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

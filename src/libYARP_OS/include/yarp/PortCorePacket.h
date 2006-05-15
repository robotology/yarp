// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
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

    PortCorePacket() {
        prev_ = next_ = NULL;
        content = NULL;
        owned = false;
        reset();
    }

    virtual ~PortCorePacket() {
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
    }

    void reset() {
        if (owned) {
            delete content;
        }
        content = NULL;
        ct = 0;
        owned = false;
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
        //       NetType::toString(inactive.size()));
        //YARP_INFO(Logger::get(),String("active count is ") + 
        //       NetType::toString(active.size()));
        if (inactive.is_empty()) {
            inactive.insert_tail(new PortCorePacket());
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
            active.remove(packet);
            inactive.insert_tail(packet);
        }
    }

    bool checkPacket(PortCorePacket *packet) {
        if (packet!=NULL) {
            //YARP_INFO(Logger::get(), String("packet use count is ") +
            //	NetType::toString(packet->getCount()));
            if (packet->getCount()<=0) {
                if (packet->getContent()!=NULL) {
                    YARP_DEBUG(Logger::get(), "Sending an onCompletion message");
                    packet->getContent()->onCompletion();
                }
                freePacket(packet);
                return true;
            }
        }
        return false;
    }

};

#endif

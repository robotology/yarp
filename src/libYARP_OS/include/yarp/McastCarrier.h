// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2007 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_MCASTCARRIER_
#define _YARP2_MCASTCARRIER_

#include <yarp/AbstractCarrier.h>
#include <yarp/UdpCarrier.h>
#include <yarp/DgramTwoWayStream.h>
#include <yarp/Logger.h>

#include <yarp/Election.h>
#include <yarp/SplitString.h>
#include <yarp/NameClient.h>
#include <yarp/NameConfig.h>

#include <stdio.h>

namespace yarp {
    class McastCarrier;
}

/**
 * Communicating between two ports via MCAST.
 */
class yarp::McastCarrier : public UdpCarrier {
protected:
    Address mcastAddress;
    String mcastName;
    String key;

    static ElectionOf<McastCarrier> caster;

public:

    McastCarrier() {
        key = "";
    }

    virtual ~McastCarrier() {
        if (key!="") {
            bool elect = isElect();
            addRemove(key);
            if (elect) {
                McastCarrier *peer = caster.getElect(key);
                if (peer==NULL) {
                    // time to remove registration
                    NameClient& nic = NameClient::getNameClient();
                    nic.unregisterName(mcastName);
                }
            }
        }
    }

    virtual Carrier *create() {
        return new McastCarrier();
    }

    virtual String getName() {
        return "mcast";
    }

    virtual int getSpecifierCode() {
        return 1;
    }


    virtual void sendHeader(Protocol& proto) {
        // need to do more than the default
        proto.defaultSendHeader();
        YARP_DEBUG(Logger::get(),"Adding extra mcast header");


        NameClient& nic = NameClient::getNameClient();
        Address addr;

        McastCarrier *elect = 
            caster.getElect(proto.getRoute().getFromName());
        if (elect!=NULL) {
            YARP_DEBUG(Logger::get(),"picking up peer mcast name");
            addr = elect->mcastAddress;
            mcastName = elect->mcastName;
        } else {
        
            // fetch an mcast address
            addr = nic.registerName("...",
                                    Address("...",0,"mcast","..."));
            mcastName = addr.getRegName();
        }
         
        int ip[] = { 224, 3, 1, 1 };
        int port = 11000;
        if (!addr.isValid()) {
            YARP_ERROR(Logger::get(), "name server not responding helpfully, setting mcast name arbitrarily");
        } else {
            SplitString ss(addr.getName().c_str(),'.');
            YARP_ASSERT(ss.size()==4);
            for (int i=0; i<4; i++) {
                ip[i] = NetType::toInt(ss.get(i));
            }
            port = addr.getPort();
        }

        ManagedBytes block(6);
        for (int i=0; i<4; i++) {
            ((unsigned char*)block.get())[i] = (unsigned char)ip[i];
        }
        block.get()[5] = (char)(port%256);
        block.get()[4] = (char)(port/256);
        proto.os().write(block.bytes());
        mcastAddress = addr;
    }

    virtual void expectExtraHeader(Protocol& proto) {
        YARP_DEBUG(Logger::get(),"Expecting extra mcast header");
        ManagedBytes block(6);
        int len = NetType::readFull(proto.is(),block.bytes());
        if (len!=block.length()) {
            throw new IOException("problem with MCAST header");
        }

        int ip[] = { 0, 0, 0, 0 };
        int port = -1;

        unsigned char *base = (unsigned char *)block.get();
        String add;
        for (int i=0; i<4; i++) {
            ip[i] = base[i];
            if (i!=0) { add += "."; }
            char buf[100];
            ACE_OS::sprintf(buf,"%d",ip[i]);
            add += buf;
        }
        port = 256*base[4]+base[5];
        Address addr(add,port,"mcast");
        YARP_DEBUG(Logger::get(),String("got mcast header ") + addr.toString());
        mcastAddress = addr;
    }


    virtual void becomeMcast(Protocol& proto, bool sender) {
        ACE_UNUSED_ARG(sender);
        DgramTwoWayStream *stream = new DgramTwoWayStream();
        YARP_ASSERT(stream!=NULL);
        Address remote = proto.getStreams().getRemoteAddress();
        Address local;
        local = proto.getStreams().getLocalAddress();
        //printf("  MULTICAST is being extended; some temporary status messages added\n");
        //printf("  Local: %s\n", local.toString().c_str());
        //printf("  Remote: %s\n", remote.toString().c_str());
        proto.takeStreams(NULL); // free up port from tcp
        try {
            if (sender) {
                /*
                  Multicast behavior seems a bit variable.
                  We assume here that if packages need to be broadcast
                  to targets via different network interfaces, that
                  we'll need to send independently on those two
                  interfaces.  This may or may not always be the case,
                  the author doesn't know, so is being cautious.
                 */
                key = proto.getRoute().getFromName();
                key += " on ";
                key += local.getName();
                YARP_DEBUG(Logger::get(),
                           String("multicast key: ") + key);
                addSender(key);

                // future optimization: only join when active
                stream->join(mcastAddress,sender,local);
            } else {
                stream->join(mcastAddress,sender,local);
            }
      
        } catch (IOException e) {
            delete stream;
            throw e;
        }
        proto.takeStreams(stream);
    }

    virtual void respondToHeader(Protocol& proto) {
        becomeMcast(proto,false);
    }


    virtual void expectReplyToHeader(Protocol& proto) {
        becomeMcast(proto,true);
    }

    void addSender(const String& key) {
        caster.add(key,this);
    }
  
    void addRemove(const String& key) {
        caster.remove(key,this);
    }
  
    bool isElect() {
        void *elect = caster.getElect(key);
        //void *elect = caster.getElect(mcastAddress.toString());
        return elect==this || elect==NULL;
    }


    virtual bool isActive() {
        return isElect();
    }

};

#endif



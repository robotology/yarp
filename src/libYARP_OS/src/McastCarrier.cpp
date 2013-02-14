// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/conf/system.h>
#ifdef YARP_HAS_ACE

#include <stdlib.h>
#include <yarp/os/impl/McastCarrier.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/Protocol.h>
#include <yarp/os/Network.h>

using namespace yarp::os::impl;
using namespace yarp::os;

ElectionOf<McastCarrier,PeerRecord> *McastCarrier::caster = NULL;

ElectionOf<McastCarrier,PeerRecord>& McastCarrier::getCaster() {
    NetworkBase::lock();
    if (caster==NULL) {
        caster = new ElectionOf<McastCarrier,PeerRecord>;
        NetworkBase::unlock();
        if (caster==NULL) {
            YARP_ERROR(Logger::get(), "No memory for McastCarrier::caster");
            exit(1);
        }
    } else {
        NetworkBase::unlock();
    }
    return *caster;
}


#else

int McastCarrierDummySymbol = 42;

#endif


yarp::os::impl::McastCarrier::McastCarrier() {
    key = "";
}

yarp::os::impl::McastCarrier::~McastCarrier() {
    if (key!="") {
        bool elect = isElect();
        addRemove(key);
        if (elect) {
            McastCarrier *peer = getCaster().getElect(key);
            if (peer==NULL) {
                // time to remove registration
                //NameClient& nic = NameClient::getNameClient();
                NetworkBase::unregisterName(mcastName.c_str());
            }
        }
    }
}

Carrier *yarp::os::impl::McastCarrier::create() {
    return new McastCarrier();
}

String yarp::os::impl::McastCarrier::getName() {
    return "mcast";
}

int yarp::os::impl::McastCarrier::getSpecifierCode() {
    return 1;
}


bool yarp::os::impl::McastCarrier::sendHeader(Protocol& proto) {
    // need to do more than the default
    bool ok = proto.defaultSendHeader();
    if (!ok) return false;

    YARP_DEBUG(Logger::get(),"Adding extra mcast header");

    Address addr;

    Address alt = proto.getStreams().getLocalAddress();
    String altKey =
        proto.getRoute().getFromName() +
        "/net=" + alt.getName();
    //printf("Key should be %s\n", altKey.c_str());
    McastCarrier *elect = getCaster().getElect(altKey);
    if (elect!=NULL) {
        YARP_DEBUG(Logger::get(),"picking up peer mcast name");
        addr = elect->mcastAddress;
        mcastName = elect->mcastName;
    } else {

        // fetch an mcast address
        Address target("...",0,"mcast","...");
        addr = Address::fromContact(NetworkBase::registerContact(target.toContact()));

        mcastName = addr.getRegName();
        if (addr.isValid()) {
            // mark owner of mcast address
            NetworkBase::setProperty(proto.getRoute().getFromName().c_str(),
                                        "owns",
                                        Value(mcastName.c_str()));
            // nic.send(String("NAME_SERVER set ") + proto.getRoute().getFromName() + " owns " + mcastName);
        }
    }

    int ip[] = { 224, 3, 1, 1 };
    int port = 11000;
    if (addr.isValid()) {
        SplitString ss(addr.getName().c_str(),'.');
        if (ss.size()!=4) {
            addr = Address();
        } else {
            YARP_ASSERT(ss.size()==4);
            for (int i=0; i<4; i++) {
                ip[i] = NetType::toInt(ss.get(i));
            }
            port = addr.getPort();
        }
    }

    if (!addr.isValid()) {
        YARP_ERROR(Logger::get(), "Name server not responding helpfully, setting mcast name arbitrarily.");
        YARP_ERROR(Logger::get(), "Only a single mcast address supported in this mode.");
        addr = Address("224.3.1.1",11000,"mcast","/tmp/mcast");
    }

    ManagedBytes block(6);
    for (int i=0; i<4; i++) {
        ((unsigned char*)block.get())[i] = (unsigned char)ip[i];
    }
    block.get()[5] = (char)(port%256);
    block.get()[4] = (char)(port/256);
    proto.os().write(block.bytes());
    mcastAddress = addr;
    return true;
}

bool yarp::os::impl::McastCarrier::expectExtraHeader(Protocol& proto) {
    YARP_DEBUG(Logger::get(),"Expecting extra mcast header");
    ManagedBytes block(6);
    ssize_t len = NetType::readFull(proto.is(),block.bytes());
    if ((size_t)len!=block.length()) {
        //throw new IOException("problem with MCAST header");
        YARP_ERROR(Logger::get(),"problem with MCAST header");
        return false;
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

    return true;
}


bool yarp::os::impl::McastCarrier::becomeMcast(Protocol& proto, bool sender) {
    ACE_UNUSED_ARG(sender);
    DgramTwoWayStream *stream = new DgramTwoWayStream();
    YARP_ASSERT(stream!=NULL);
    Address remote = proto.getStreams().getRemoteAddress();
    Address local;
    local = proto.getStreams().getLocalAddress();
    bool test = true;
    //(yarp::NameConfig::getEnv("YARP_MCAST_TEST")!="");
    /*
    if (test) {
        printf("  MULTICAST is being extended; some temporary status messages added\n");
        printf("  Local: %s\n", local.toString().c_str());
        printf("  Remote: %s\n", remote.toString().c_str());
    }
    */
    proto.takeStreams(NULL); // free up port from tcp

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
        if (test) {
            key += "/net=";
            key += local.getName();
        }
        YARP_DEBUG(Logger::get(),
                    String("multicast key: ") + key);
        addSender(key);
    }

    bool ok = true;
    if (isElect()||!sender) {
        if (test) {
            ok = stream->join(mcastAddress,sender,local);
        } else {
            ok = stream->join(mcastAddress,sender);
        }
    }

    if (!ok) {
        delete stream;
        return false;
    }
    proto.takeStreams(stream);
    return true;
}

bool yarp::os::impl::McastCarrier::respondToHeader(Protocol& proto) {
    return becomeMcast(proto,false);
}


bool yarp::os::impl::McastCarrier::expectReplyToHeader(Protocol& proto) {
    return becomeMcast(proto,true);
}

void yarp::os::impl::McastCarrier::addSender(const String& key) {
    getCaster().add(key,this);
}

void yarp::os::impl::McastCarrier::addRemove(const String& key) {
    getCaster().remove(key,this);
}

bool yarp::os::impl::McastCarrier::isElect() {
    void *elect = getCaster().getElect(key);
    //void *elect = caster.getElect(mcastAddress.toString());
    return elect==this || elect==NULL;
}


bool yarp::os::impl::McastCarrier::isActive() {
    return isElect();
}

bool yarp::os::impl::McastCarrier::isBroadcast() {
    return true;
}


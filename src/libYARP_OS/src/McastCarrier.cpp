/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/conf/system.h>
#include <yarp/os/impl/McastCarrier.h>
#include <stdlib.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/Network.h>

using namespace yarp::os::impl;
using namespace yarp::os;

ElectionOf<PeerRecord<McastCarrier> > *McastCarrier::caster = YARP_NULLPTR;

ElectionOf<PeerRecord<McastCarrier> >& McastCarrier::getCaster() {
    NetworkBase::lock();
    if (caster==YARP_NULLPTR) {
        caster = new ElectionOf<PeerRecord<McastCarrier> >;
        NetworkBase::unlock();
        if (caster==YARP_NULLPTR) {
            YARP_ERROR(Logger::get(), "No memory for McastCarrier::caster");
            exit(1);
        }
    } else {
        NetworkBase::unlock();
    }
    return *caster;
}


yarp::os::impl::McastCarrier::McastCarrier() {
    key = "";
}

yarp::os::impl::McastCarrier::~McastCarrier() {
    if (key!="") {
        bool elect = isElect();
        addRemove(key);
        if (elect) {
            McastCarrier *peer = getCaster().getElect(key);
            if (peer==YARP_NULLPTR) {
                // time to remove registration
                NetworkBase::unregisterName(mcastName.c_str());
            }
        }
    }
}

Carrier *yarp::os::impl::McastCarrier::create() {
    return new McastCarrier();
}

ConstString yarp::os::impl::McastCarrier::getName() {
    return "mcast";
}

int yarp::os::impl::McastCarrier::getSpecifierCode() {
    return 1;
}


bool yarp::os::impl::McastCarrier::sendHeader(ConnectionState& proto) {
    // need to do more than the default
    bool ok = defaultSendHeader(proto);
    if (!ok) return false;

    YARP_DEBUG(Logger::get(),"Adding extra mcast header");

    Contact addr;

    Contact alt = proto.getStreams().getLocalAddress();
    ConstString altKey =
        proto.getRoute().getFromName() +
        "/net=" + alt.getHost();
    McastCarrier *elect = getCaster().getElect(altKey);
    if (elect!=YARP_NULLPTR) {
        YARP_DEBUG(Logger::get(),"picking up peer mcast name");
        addr = elect->mcastAddress;
        mcastName = elect->mcastName;
    } else {

        // fetch an mcast address
        Contact target("...", "mcast", "...", 0);
        addr = NetworkBase::registerContact(target);
        mcastName = addr.getRegName();
        if (addr.isValid()) {
            // mark owner of mcast address
            NetworkBase::setProperty(proto.getRoute().getFromName().c_str(),
                                     "owns",
                                     Value(mcastName.c_str()));
        }
    }

    int ip[] = { 224, 3, 1, 1 };
    int port = 11000;
    if (addr.isValid()) {
        SplitString ss(addr.getHost().c_str(),'.');
        if (ss.size()!=4) {
            addr = Contact();
        } else {
            yAssert(ss.size()==4);
            for (int i=0; i<4; i++) {
                ip[i] = NetType::toInt(ss.get(i));
            }
            port = addr.getPort();
        }
    }

    if (!addr.isValid()) {
        YARP_ERROR(Logger::get(), "Name server not responding helpfully, setting mcast name arbitrarily.");
        YARP_ERROR(Logger::get(), "Only a single mcast address supported in this mode.");
        addr = Contact("/tmp/mcast", "mcast", "224.3.1.1", 11000);
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

bool yarp::os::impl::McastCarrier::expectExtraHeader(ConnectionState& proto) {
    YARP_DEBUG(Logger::get(),"Expecting extra mcast header");
    ManagedBytes block(6);
    YARP_SSIZE_T len = proto.is().readFull(block.bytes());
    if ((size_t)len!=block.length()) {
        YARP_ERROR(Logger::get(),"problem with MCAST header");
        return false;
    }

    int ip[] = { 0, 0, 0, 0 };
    int port = -1;

    unsigned char *base = (unsigned char *)block.get();
    ConstString add;
    for (int i=0; i<4; i++) {
        ip[i] = base[i];
        if (i!=0) { add += "."; }
        char buf[100];
        ACE_OS::sprintf(buf,"%d",ip[i]);
        add += buf;
    }
    port = 256*base[4]+base[5];
    Contact addr("mcast", add, port);
    YARP_DEBUG(Logger::get(),ConstString("got mcast header ") + addr.toURI());
    mcastAddress = addr;

    return true;
}


bool yarp::os::impl::McastCarrier::becomeMcast(ConnectionState& proto, bool sender) {
#ifndef YARP_HAS_ACE
    return false;
#else
    YARP_UNUSED(sender);
    DgramTwoWayStream *stream = new DgramTwoWayStream();
    yAssert(stream!=YARP_NULLPTR);
    Contact remote = proto.getStreams().getRemoteAddress();
    Contact local;
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
    proto.takeStreams(YARP_NULLPTR); // free up port from tcp

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
            key += local.getHost();
        }
        YARP_DEBUG(Logger::get(),
                    ConstString("multicast key: ") + key);
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
#endif
}

bool yarp::os::impl::McastCarrier::respondToHeader(ConnectionState& proto) {
    return becomeMcast(proto,false);
}


bool yarp::os::impl::McastCarrier::expectReplyToHeader(ConnectionState& proto) {
    return becomeMcast(proto,true);
}

void yarp::os::impl::McastCarrier::addSender(const ConstString& key) {
    getCaster().add(key,this);
}

void yarp::os::impl::McastCarrier::addRemove(const ConstString& key) {
    getCaster().remove(key,this);
}

bool yarp::os::impl::McastCarrier::isElect() {
    void *elect = getCaster().getElect(key);
    //void *elect = caster.getElect(mcastAddress.toString());
    return elect==this || elect==YARP_NULLPTR;
}


bool yarp::os::impl::McastCarrier::isActive() {
    return isElect();
}

bool yarp::os::impl::McastCarrier::isBroadcast() {
    return true;
}


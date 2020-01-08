/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/impl/McastCarrier.h>

#include <yarp/conf/system.h>

#include <yarp/os/ConnectionState.h>
#include <yarp/os/Network.h>
#include <yarp/os/Route.h>
#include <yarp/os/impl/Logger.h>

#include <cstdlib>

using namespace yarp::os::impl;
using namespace yarp::os;


ElectionOf<PeerRecord<McastCarrier>>* McastCarrier::caster = nullptr;

ElectionOf<PeerRecord<McastCarrier>>& McastCarrier::getCaster()
{
    NetworkBase::lock();
    if (caster == nullptr) {
        caster = new ElectionOf<PeerRecord<McastCarrier>>;
        NetworkBase::unlock();
        if (caster == nullptr) {
            YARP_ERROR(Logger::get(), "No memory for McastCarrier::caster");
            std::exit(1);
        }
    } else {
        NetworkBase::unlock();
    }
    return *caster;
}


yarp::os::impl::McastCarrier::McastCarrier()
{
    stream = nullptr;
    key = "";
}

yarp::os::impl::McastCarrier::~McastCarrier()
{
    if (!key.empty()) {
        bool elect = isElect();
        removeSender(key);
        if (elect) {
            McastCarrier* peer = getCaster().getElect(key);
            if (peer == nullptr) {
                // time to remove registration
                NetworkBase::unregisterName(mcastName);
            } else {
                if (!peer->takeElection()) {
                    YARP_ERROR(Logger::get(), "Something went wrong during the shift of the election...");
                }
            }
        }
    }
}

Carrier* yarp::os::impl::McastCarrier::create() const
{
    return new McastCarrier();
}

std::string yarp::os::impl::McastCarrier::getName() const
{
    return "mcast";
}

int yarp::os::impl::McastCarrier::getSpecifierCode() const
{
    return 1;
}


bool yarp::os::impl::McastCarrier::sendHeader(ConnectionState& proto)
{
    // need to do more than the default
    bool ok = defaultSendHeader(proto);
    if (!ok) {
        return false;
    }

    YARP_DEBUG(Logger::get(), "Adding extra mcast header");

    Contact addr;

    Contact alt = proto.getStreams().getLocalAddress();
    std::string altKey = proto.getRoute().getFromName() + "/net=" + alt.getHost();
    McastCarrier* elect = getCaster().getElect(altKey);
    if (elect != nullptr) {
        YARP_DEBUG(Logger::get(), "picking up peer mcast name");
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
                                     Value(mcastName));
        }
    }

    int ip[] = {224, 3, 1, 1};
    int port = 11000;
    if (addr.isValid()) {
        SplitString ss(addr.getHost().c_str(), '.');
        if (ss.size() != 4) {
            addr = Contact();
        } else {
            yAssert(ss.size() == 4);
            for (int i = 0; i < 4; i++) {
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
    for (int i = 0; i < 4; i++) {
        ((unsigned char*)block.get())[i] = (unsigned char)ip[i];
    }
    block.get()[5] = (char)(port % 256);
    block.get()[4] = (char)(port / 256);
    proto.os().write(block.bytes());
    mcastAddress = addr;
    return true;
}

bool yarp::os::impl::McastCarrier::expectExtraHeader(ConnectionState& proto)
{
    YARP_DEBUG(Logger::get(), "Expecting extra mcast header");
    ManagedBytes block(6);
    yarp::conf::ssize_t len = proto.is().readFull(block.bytes());
    if ((size_t)len != block.length()) {
        YARP_ERROR(Logger::get(), "problem with MCAST header");
        return false;
    }

    int ip[] = {0, 0, 0, 0};
    int port = -1;

    auto* base = (unsigned char*)block.get();
    std::string add;
    for (int i = 0; i < 4; i++) {
        ip[i] = base[i];
        if (i != 0) {
            add += ".";
        }
        char buf[100];
        sprintf(buf, "%d", ip[i]);
        add += buf;
    }
    port = 256 * base[4] + base[5];
    Contact addr("mcast", add, port);
    YARP_DEBUG(Logger::get(), std::string("got mcast header ") + addr.toURI());
    mcastAddress = addr;

    return true;
}


bool yarp::os::impl::McastCarrier::becomeMcast(ConnectionState& proto, bool sender)
{
    stream = new DgramTwoWayStream();
    yAssert(stream != nullptr);
    Contact remote = proto.getStreams().getRemoteAddress();
    local = proto.getStreams().getLocalAddress();
    //(yarp::NameConfig::getEnv("YARP_MCAST_TEST")!="");
    proto.takeStreams(nullptr); // free up port from tcp

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
        key += "/net=";
        key += local.getHost();

        YARP_DEBUG(Logger::get(),
                   std::string("multicast key: ") + key);
        addSender(key);
    }

    bool ok = true;
    if (isElect() || !sender) {
        ok = stream->join(mcastAddress, sender, local);
    }

    if (!ok) {
        delete stream;
        return false;
    }
    proto.takeStreams(stream);
    return true;
}

bool yarp::os::impl::McastCarrier::respondToHeader(ConnectionState& proto)
{
    return becomeMcast(proto, false);
}


bool yarp::os::impl::McastCarrier::expectReplyToHeader(ConnectionState& proto)
{
    return becomeMcast(proto, true);
}

void yarp::os::impl::McastCarrier::addSender(const std::string& key)
{
    getCaster().add(key, this);
}

void yarp::os::impl::McastCarrier::removeSender(const std::string& key)
{
    getCaster().remove(key, this);
}

bool yarp::os::impl::McastCarrier::isElect() const
{
    void* elect = getCaster().getElect(key);
    //void *elect = caster.getElect(mcastAddress.toString());
    return elect == this || elect == nullptr;
}

bool yarp::os::impl::McastCarrier::takeElection()
{
    if (stream != nullptr) {
        return stream->join(mcastAddress, true, local);
    }
    return false;
}


bool yarp::os::impl::McastCarrier::isActive() const
{
    return isElect();
}

bool yarp::os::impl::McastCarrier::isBroadcast() const
{
    return true;
}

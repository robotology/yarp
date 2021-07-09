/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/McastCarrier.h>

#include <yarp/conf/system.h>
#include <yarp/conf/string.h>
#include <yarp/conf/numeric.h>

#include <yarp/os/ConnectionState.h>
#include <yarp/os/Network.h>
#include <yarp/os/Route.h>
#include <yarp/os/impl/LogComponent.h>

#include <cstdlib>

using namespace yarp::os::impl;
using namespace yarp::os;

namespace {
YARP_OS_LOG_COMPONENT(MCASTCARRIER, "yarp.os.impl.McastCarrier")
} // namespace

ElectionOf<PeerRecord<McastCarrier>>* McastCarrier::caster = nullptr;

ElectionOf<PeerRecord<McastCarrier>>& McastCarrier::getCaster()
{
    NetworkBase::lock();
    if (caster == nullptr) {
        caster = new ElectionOf<PeerRecord<McastCarrier>>;
        NetworkBase::unlock();
        if (caster == nullptr) {
            yCError(MCASTCARRIER, "No memory for McastCarrier::caster");
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
                    yCError(MCASTCARRIER, "Something went wrong during the shift of the election...");
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

    yCDebug(MCASTCARRIER, "Adding extra mcast header");

    Contact addr;

    Contact alt = proto.getStreams().getLocalAddress();
    std::string altKey = proto.getRoute().getFromName() + "/net=" + alt.getHost();
    McastCarrier* elect = getCaster().getElect(altKey);
    if (elect != nullptr) {
        yCDebug(MCASTCARRIER, "picking up peer mcast name");
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

    constexpr size_t ipv4_size = 4;
    int ip[ipv4_size] = {224, 3, 1, 1};
    constexpr int default_port = 11000;
    int port = default_port;
    if (addr.isValid()) {
        auto ss = yarp::conf::string::split(addr.getHost(), '.');
        if (ss.size() != ipv4_size) {
            addr = Contact();
        } else {
            for (size_t i = 0; i < ipv4_size; ++i) {
                ip[i] = yarp::conf::numeric::from_string<int>(ss[i]);
            }
            port = addr.getPort();
        }
    }

    if (!addr.isValid()) {
        yCError(MCASTCARRIER, "Name server not responding helpfully, setting mcast name arbitrarily.");
        yCError(MCASTCARRIER, "Only a single mcast address supported in this mode.");
        addr = Contact("/tmp/mcast", "mcast", "224.3.1.1", 11000);
    }

    ManagedBytes block(6);
    for (size_t i = 0; i < ipv4_size; i++) {
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
    yCDebug(MCASTCARRIER, "Expecting extra mcast header");
    ManagedBytes block(6);
    yarp::conf::ssize_t len = proto.is().readFull(block.bytes());
    if ((size_t)len != block.length()) {
        yCError(MCASTCARRIER, "problem with MCAST header");
        return false;
    }

    constexpr size_t ipv4_size = 4;
    int ip[] = {0, 0, 0, 0};
    int port = -1;

    auto* base = (unsigned char*)block.get();
    std::string add;
    for (size_t i = 0; i < ipv4_size; i++) {
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
    yCDebug(MCASTCARRIER, "got mcast header %s", addr.toURI().c_str());
    mcastAddress = addr;

    return true;
}


bool yarp::os::impl::McastCarrier::becomeMcast(ConnectionState& proto, bool sender)
{
    stream = new DgramTwoWayStream();
    yCAssert(MCASTCARRIER, stream != nullptr);
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

        yCDebug(MCASTCARRIER, "multicast key: %s", key.c_str());
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

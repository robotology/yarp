/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/impl/PlatformStdlib.h>
#include <yarp/os/impl/ShmemCarrier.h>
#include <yarp/os/ConstString.h>
// removing old shmem version
// #include <yarp/os/impl/ShmemTwoWayStream.h>

#ifdef YARP_HAS_ACE
// new shmem implementation from Alessandro
#include <yarp/os/impl/ShmemHybridStream.h>
#endif

using namespace yarp::os;
using namespace yarp::os::impl;

yarp::os::impl::ShmemCarrier::ShmemCarrier(int version) {
    this->version = version;
}

yarp::os::Carrier *yarp::os::impl::ShmemCarrier::create() {
    return new ShmemCarrier(version);
}

yarp::os::ConstString yarp::os::impl::ShmemCarrier::getName() {
    return (version==2)?"shmem":"shmem1";
}

int yarp::os::impl::ShmemCarrier::getSpecifierCode() {
    // specifier codes are a very old yarp feature,
    // not necessary any more really, should be replaced.
    return (version==1)?2:14;
}

bool yarp::os::impl::ShmemCarrier::requireAck() {
    return true;
}

bool yarp::os::impl::ShmemCarrier::isConnectionless() {
    return false;
}

bool yarp::os::impl::ShmemCarrier::checkHeader(const Bytes& header) {
    return getSpecifier(header)%16 == getSpecifierCode();
}

void yarp::os::impl::ShmemCarrier::getHeader(const Bytes& header) {
    createStandardHeader(getSpecifierCode(), header);
}

void yarp::os::impl::ShmemCarrier::setParameters(const Bytes& header) {
}

bool yarp::os::impl::ShmemCarrier::becomeShmemVersionHybridStream(ConnectionState& proto, bool sender) {
#ifndef YARP_HAS_ACE
    return false;
#else
    ShmemHybridStream *stream = new ShmemHybridStream();
    yAssert(stream!=YARP_NULLPTR);
    Contact base;

    bool ok = true;

    if (!sender) {
        ACE_INET_Addr anywhere((u_short)0, (ACE_UINT32)INADDR_ANY);
        base = Contact(anywhere.get_host_addr(),
                       anywhere.get_port_number());
        bool ok = stream->open(base,sender)==0;
        if (ok) {
            int myPort = stream->getLocalAddress().getPort();
            writeYarpInt(myPort,proto);
            stream->accept();
            proto.takeStreams(YARP_NULLPTR);
            proto.takeStreams(stream);
        }
    } else {
        int altPort = readYarpInt(proto);
        ConstString myName = proto.getStreams().getLocalAddress().getHost();
        proto.takeStreams(YARP_NULLPTR);
        base = Contact(myName,altPort);
        ok = stream->open(base,sender)==0;
        if (ok) {
            proto.takeStreams(stream);
        }
    }

    if (!ok) {
        delete stream;
        stream = YARP_NULLPTR;
        return false;
    }

    return true;
#endif
}

bool yarp::os::impl::ShmemCarrier::becomeShmem(ConnectionState& proto, bool sender) {
    if (version==1) {
        // "classic" shmem
        //becomeShmemVersion<ShmemTwoWayStream>(proto,sender);
        //becomeShmemVersionTwoWayStream(proto,sender);
        ACE_OS::printf("Classic shmem no longer exists\n");
        ACE_OS::exit(1);
        return false;
    } else {
        // experimental shmem
        //becomeShmemVersion<ShmemHybridStream>(proto,sender);
        return becomeShmemVersionHybridStream(proto,sender);
    }
}

bool yarp::os::impl::ShmemCarrier::respondToHeader(ConnectionState& proto) {
    // i am the receiver
    return becomeShmem(proto,false);
}


bool yarp::os::impl::ShmemCarrier::expectReplyToHeader(ConnectionState& proto) {
    // i am the sender
    return becomeShmem(proto,true);
}

// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#ifndef _YARP2_UDPCARRIER_
#define _YARP2_UDPCARRIER_

#include <yarp/os/impl/AbstractCarrier.h>
#include <yarp/os/impl/DgramTwoWayStream.h>

namespace yarp {
    namespace os {
        namespace impl {
            class UdpCarrier;
        }
    }
}

/**
 * Communicating between two ports via UDP.
 */
class yarp::os::impl::UdpCarrier : public AbstractCarrier {
public:

    UdpCarrier() {
    }

    virtual Carrier *create() {
        return new UdpCarrier();
    }

    virtual String getName() {
        return "udp";
    }

    virtual int getSpecifierCode() {
        return 0;
    }

    virtual bool checkHeader(const Bytes& header) {
        return getSpecifier(header)%16 == getSpecifierCode();
    }

    virtual void getHeader(const Bytes& header) {
        createStandardHeader(getSpecifierCode(), header);
    }

    virtual void setParameters(const Bytes& header) {
    }

    virtual bool requireAck() {
        return false;
    }

    virtual bool isConnectionless() {
        return true;
    }


    virtual bool respondToHeader(Protocol& proto) {
        // I am the receiver

        // issue: need a fresh port number...
        DgramTwoWayStream *stream = new DgramTwoWayStream();
        YARP_ASSERT(stream!=NULL);

        Address remote = proto.getStreams().getRemoteAddress();
        bool ok = stream->open(remote);
        if (!ok) {
            delete stream;
            return false;
        }

        int myPort = stream->getLocalAddress().getPort();
        proto.writeYarpInt(myPort);
        proto.takeStreams(stream);

        return true;
    }

    virtual bool expectReplyToHeader(Protocol& proto) {
        // I am the sender
        int myPort = proto.getStreams().getLocalAddress().getPort();
        String myName = proto.getStreams().getLocalAddress().getName();
        String altName = proto.getStreams().getRemoteAddress().getName();

        int altPort = proto.readYarpInt();

        if (altPort==-1) {
            return false;
        }

        DgramTwoWayStream *stream = new DgramTwoWayStream();
        YARP_ASSERT(stream!=NULL);

        proto.takeStreams(NULL); // free up port from tcp
        bool ok =
            stream->open(Address(myName,myPort),Address(altName,altPort));
        if (!ok) {
            delete stream;
            return false;
        }
        proto.takeStreams(stream);
        return true;
    }

};

#endif

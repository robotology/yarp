// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_TCPCARRIER_
#define _YARP2_TCPCARRIER_

#include <yarp/os/impl/AbstractCarrier.h>

#include <ace/OS_NS_stdio.h>

namespace yarp {
    namespace os {
        namespace impl {
            class TcpCarrier;
        }
    }
}

/**
 * Communicating between two ports via TCP.
 */
class yarp::os::impl::TcpCarrier : public AbstractCarrier {
public:

    TcpCarrier(bool requireAckFlag = true) {
        this->requireAckFlag = requireAckFlag;
    }

    virtual Carrier *create() {
        return new TcpCarrier(requireAckFlag);
    }

    virtual yarp::String getName() {
        return requireAckFlag?"tcp":"fast_tcp";
    }

    int getSpecifierCode() {
        return 3;
    }

    virtual bool checkHeader(const yarp::os::Bytes& header) {
        int spec = getSpecifier(header);
        if (spec%16 == getSpecifierCode()) {
            if (((spec&128)!=0) == requireAckFlag) {
                return true;
            }
        }
        return false;
    }

    virtual void getHeader(const yarp::os::Bytes& header) {
        createStandardHeader(getSpecifierCode()+(requireAckFlag?128:0), header);
    }

    virtual void setParameters(const yarp::os::Bytes& header) {
        //int specifier = getSpecifier(header);
        //requireAckFlag = (specifier&128)!=0;
        // Now prefilter by ack flag
    }

    virtual bool requireAck() {
        return requireAckFlag;
    }

    virtual bool isConnectionless() {
        return false;
    }

    virtual bool respondToHeader(Protocol& proto) {
        int cport = proto.getStreams().getLocalAddress().getPort();
        proto.writeYarpInt(cport);
        return proto.checkStreams();
    }

    virtual bool expectReplyToHeader(Protocol& proto) {
        proto.readYarpInt(); // ignore result
        return proto.checkStreams();
    }

private:
    bool requireAckFlag;
};

#endif


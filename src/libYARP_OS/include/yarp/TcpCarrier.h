// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_TCPCARRIER_
#define _YARP2_TCPCARRIER_

#include <yarp/AbstractCarrier.h>

#include <ace/OS_NS_stdio.h>

namespace yarp {
    class TcpCarrier;
}

/**
 * Communicating between two ports via TCP.
 */
class yarp::TcpCarrier : public AbstractCarrier {
public:

    TcpCarrier(bool requireAckFlag = true) {
        this->requireAckFlag = requireAckFlag;
    }

    virtual Carrier *create() {
        return new TcpCarrier(requireAckFlag);
    }

    virtual String getName() {
        return requireAckFlag?"tcp":"fast_tcp";
    }

    int getSpecifierCode() {
        return 3;
    }

    virtual bool checkHeader(const Bytes& header) {
        int spec = getSpecifier(header);
        if (spec%16 == getSpecifierCode()) {
            if (((spec&128)!=0) == requireAckFlag) {
                return true;
            }
        }
        return false;
    }

    virtual void getHeader(const Bytes& header) {
        createStandardHeader(getSpecifierCode()+(requireAckFlag?128:0), header);
    }

    virtual void setParameters(const Bytes& header) {
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

    virtual void respondToHeader(Protocol& proto) {
        int cport = proto.getStreams().getLocalAddress().getPort();
        proto.writeYarpInt(cport);
    }

    virtual void expectReplyToHeader(Protocol& proto) {
        proto.readYarpInt(); // ignore result
    }

private:
    bool requireAckFlag;
};

#endif


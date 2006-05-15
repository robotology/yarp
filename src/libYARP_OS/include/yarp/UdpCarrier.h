// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef _YARP2_UDPCARRIER_
#define _YARP2_UDPCARRIER_

#include <yarp/AbstractCarrier.h>
#include <yarp/DgramTwoWayStream.h>

namespace yarp {
    class UdpCarrier;
}

/**
 * Communicating between two ports via UDP.
 */
class yarp::UdpCarrier : public AbstractCarrier {
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


    virtual void respondToHeader(Protocol& proto) {
        // I am the receiver

        // issue: need a fresh port number...
        DgramTwoWayStream *stream = new DgramTwoWayStream();
        YARP_ASSERT(stream!=NULL);
        try {
            Address remote = proto.getStreams().getRemoteAddress();
            stream->open(remote);

            int myPort = stream->getLocalAddress().getPort();
            proto.writeYarpInt(myPort);
      
            proto.takeStreams(stream);

        } catch (IOException e) {
            delete stream;
            throw e;
        }
    }

    virtual void expectReplyToHeader(Protocol& proto) {
        // I am the sender
        int myPort = proto.getStreams().getLocalAddress().getPort();
        String myName = proto.getStreams().getLocalAddress().getName();
        String altName = proto.getStreams().getRemoteAddress().getName();

        int altPort = proto.readYarpInt();

        DgramTwoWayStream *stream = new DgramTwoWayStream();
        YARP_ASSERT(stream!=NULL);

        proto.takeStreams(NULL); // free up port from tcp
        try {
            stream->open(Address(myName,myPort),Address(altName,altPort));
        } catch (IOException e) {
            delete stream;
            throw e;
        }
        proto.takeStreams(stream);
    }

};

#endif


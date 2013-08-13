// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_NAMESERCARRIER_
#define _YARP2_NAMESERCARRIER_

#include <yarp/os/TwoWayStream.h>
#include <yarp/os/impl/TcpCarrier.h>


namespace yarp {
    namespace os {
        namespace impl {
            class NameserCarrier;
            class NameserTwoWayStream;
        }
    }
}

/**
 * Communicating between two ports via a variant plain-text protocol
 * originally designed for the yarp name server.  For backwards
 * compatibility
 */
class yarp::os::impl::NameserTwoWayStream : public TwoWayStream, InputStream {
private:
    TwoWayStream *delegate;
    String pendingRead;
    String swallowRead;
public:
    NameserTwoWayStream(TwoWayStream *delegate);

    virtual ~NameserTwoWayStream();

    virtual InputStream& getInputStream();
    virtual OutputStream& getOutputStream();
    virtual const Contact& getLocalAddress();
    virtual const Contact& getRemoteAddress();

    virtual bool isOk();
    virtual void reset();
    virtual void close();
    virtual void beginPacket();
    virtual void endPacket();

    virtual YARP_SSIZE_T read(const yarp::os::Bytes& b);
};


/**
 * Communicating between two ports via a variant plain-text protocol
 * originally designed for the yarp name server.  For backwards
 * compatibility
 */
class yarp::os::impl::NameserCarrier : public TcpCarrier {
private:
    bool firstSend;
public:
    NameserCarrier();

    virtual String getName();
    virtual String getSpecifierName();

    virtual Carrier *create();

    virtual bool checkHeader(const Bytes& header);
    virtual void getHeader(const Bytes& header);
    virtual bool requireAck();
    virtual bool isTextMode();
    virtual bool supportReply();
    virtual bool canEscape();
    virtual bool sendHeader(Protocol& proto);
    virtual bool expectSenderSpecifier(Protocol& proto);
    virtual bool expectIndex(Protocol& proto);
    virtual bool sendAck(Protocol& proto);
    virtual bool expectAck(Protocol& proto);
    virtual bool respondToHeader(Protocol& proto);
    virtual bool expectReplyToHeader(Protocol& proto);
    virtual bool write(Protocol& proto, SizedWriter& writer);
};

#endif

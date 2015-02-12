// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Author: Daniel Krieg krieg@fias.uni-frankfurt.de
 * Copyright (C) 2010 Daniel Krieg
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP_MPICARRIER_
#define _YARP_MPICARRIER_

#include <yarp/os/all.h>

#include <yarp/os/AbstractCarrier.h>
#include <yarp/os/Bytes.h>

#include <string>
#include <iostream>

#include <yarp/os/MpiStream.h>

namespace yarp {
    namespace os {
        class MpiCarrier;
    }
}

/**
 * Abstract base carrier for managing port communication via MPI.
 *
 */
class yarp::os::MpiCarrier : public AbstractCarrier {
protected:
    MpiStream* stream;
    MpiComm* comm;
    ConstString port;
    ConstString name, other, route;
    ConstString target;
public:
    MpiCarrier() ;
    virtual ~MpiCarrier();
    virtual void close() = 0;
    virtual Carrier *create() = 0;
    virtual ConstString getName() = 0;

    virtual void createStream(bool sender) = 0;

    virtual bool isConnectionless() {
        return false;
    }

    virtual bool canEscape() {
        return true;
    }

    virtual bool supportReply() = 0;


    virtual void getHeader(const Bytes& header);
    virtual bool checkHeader(const Bytes& header);


    virtual bool sendHeader(ConnectionState& proto);
    virtual bool expectSenderSpecifier(ConnectionState& proto);

    virtual bool respondToHeader(ConnectionState& proto);
    virtual bool expectReplyToHeader(ConnectionState& proto);


    /////////////////////////////////////////////////
    // Payload time!

    bool write(ConnectionState& proto, SizedWriter& writer) {
        writer.write(proto.os());
        return proto.os().isOk();
    }

    virtual bool sendIndex(ConnectionState& proto, SizedWriter& writer) {
        return true;
    }

    virtual bool expectIndex(ConnectionState& proto) {
        return true;
    }

    /////////////////////////////////////////////////
    // Acknowledgements, we don't do them

    virtual bool sendAck(ConnectionState& proto) {
        return true;
    }

    virtual bool expectAck(ConnectionState& proto) {
        return true;
    }

};

#endif //_YARP_MPICARRIER_

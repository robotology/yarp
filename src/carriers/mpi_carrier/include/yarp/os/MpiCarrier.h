/*
 * Author: Daniel Krieg krieg@fias.uni-frankfurt.de
 * Copyright (C) 2010 Daniel Krieg
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP_MPICARRIER
#define YARP_MPICARRIER

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
    virtual void close() override = 0;
    virtual Carrier *create() override = 0;
    virtual ConstString getName() override = 0;

    virtual void createStream(bool sender) = 0;

    virtual bool isConnectionless() override {
        return false;
    }

    virtual bool canEscape() override {
        return true;
    }

    virtual bool supportReply() override = 0;


    virtual void getHeader(const Bytes& header) override;
    virtual bool checkHeader(const Bytes& header) override;


    virtual bool sendHeader(ConnectionState& proto) override;
    virtual bool expectSenderSpecifier(ConnectionState& proto) override;

    virtual bool respondToHeader(ConnectionState& proto) override;
    virtual bool expectReplyToHeader(ConnectionState& proto) override;


    /////////////////////////////////////////////////
    // Payload time!

    bool write(ConnectionState& proto, SizedWriter& writer) override {
        writer.write(proto.os());
        return proto.os().isOk();
    }

    virtual bool sendIndex(ConnectionState& proto, SizedWriter& writer) override {
        return true;
    }

    virtual bool expectIndex(ConnectionState& proto) override {
        return true;
    }

    /////////////////////////////////////////////////
    // Acknowledgements, we don't do them

    virtual bool sendAck(ConnectionState& proto) override {
        return true;
    }

    virtual bool expectAck(ConnectionState& proto) override {
        return true;
    }

};

#endif //_YARP_MPICARRIER_

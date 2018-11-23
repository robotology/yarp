/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2010 Daniel Krieg <krieg@fias.uni-frankfurt.de>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_MPICARRIER
#define YARP_MPICARRIER

#include <yarp/os/AbstractCarrier.h>
#include <yarp/os/Bytes.h>
#include <yarp/os/ConnectionState.h>
#include <yarp/os/SizedWriter.h>

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
    std::string port;
    std::string name, other, route;
    std::string target;
public:
    MpiCarrier() ;
    virtual ~MpiCarrier();
    void close() override = 0;
    Carrier *create() const override = 0;
    std::string getName() const override = 0;

    virtual void createStream(bool sender) = 0;

    bool isConnectionless() const override {
        return false;
    }

    bool canEscape() const override {
        return true;
    }

    bool supportReply() const override = 0;


    void getHeader(Bytes& header) const override;
    bool checkHeader(const Bytes& header) override;


    bool sendHeader(ConnectionState& proto) override;
    bool expectSenderSpecifier(ConnectionState& proto) override;

    bool respondToHeader(ConnectionState& proto) override;
    bool expectReplyToHeader(ConnectionState& proto) override;


    /////////////////////////////////////////////////
    // Payload time!

    bool write(ConnectionState& proto, SizedWriter& writer) override {
        writer.write(proto.os());
        return proto.os().isOk();
    }

    bool sendIndex(ConnectionState& proto, SizedWriter& writer) override {
        return true;
    }

    bool expectIndex(ConnectionState& proto) override {
        return true;
    }

    /////////////////////////////////////////////////
    // Acknowledgements, we don't do them

    bool sendAck(ConnectionState& proto) override {
        return true;
    }

    bool expectAck(ConnectionState& proto) override {
        return true;
    }

};

#endif // YARP_MPICARRIER

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2010 Daniel Krieg <krieg@fias.uni-frankfurt.de>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_MPICARRIER_H
#define YARP_MPICARRIER_H

#include <yarp/os/AbstractCarrier.h>
#include <yarp/os/Bytes.h>
#include <yarp/os/ConnectionState.h>
#include <yarp/os/SizedWriter.h>

#include <string>
#include <iostream>

#include "MpiStream.h"


/**
 * Abstract base carrier for managing port communication via MPI.
 */
class MpiCarrier :
        public yarp::os::AbstractCarrier
{
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


    void getHeader(yarp::os::Bytes& header) const override;
    bool checkHeader(const yarp::os::Bytes& header) override;


    bool sendHeader(yarp::os::ConnectionState& proto) override;
    bool expectSenderSpecifier(yarp::os::ConnectionState& proto) override;

    bool respondToHeader(yarp::os::ConnectionState& proto) override;
    bool expectReplyToHeader(yarp::os::ConnectionState& proto) override;


    /////////////////////////////////////////////////
    // Payload time!

    bool write(yarp::os::ConnectionState& proto, yarp::os::SizedWriter& writer) override {
        writer.write(proto.os());
        return proto.os().isOk();
    }

    bool sendIndex(yarp::os::ConnectionState& proto, yarp::os::SizedWriter& writer) override {
        return true;
    }

    bool expectIndex(yarp::os::ConnectionState& proto) override {
        return true;
    }

    /////////////////////////////////////////////////
    // Acknowledgements, we don't do them

    bool sendAck(yarp::os::ConnectionState& proto) override {
        return true;
    }

    bool expectAck(yarp::os::ConnectionState& proto) override {
        return true;
    }

};

#endif // YARP_MPICARRIER_H

// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Author: Daniel Krieg krieg@fias.uni-frankfurt.de
 * Copyright (C) 2010 Daniel Krieg
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/all.h>

#include <yarp/os/impl/AbstractCarrier.h>
#include <yarp/os/Bytes.h>

#include <string>
#include <iostream>

#include <yarp/os/impl/MpiStream.h>

namespace yarp {
    namespace os {
        namespace impl {
            class MpiCarrier;
        }
    }
}

/**
 * Communicating between two ports via MPI.
 *
 * @warning Seems to work, but still experimental.
 */
class yarp::os::impl::MpiCarrier : public AbstractCarrier {
private:
    MpiStream* stream;
    String port;
    String name;
public:
    MpiCarrier();
    ~MpiCarrier();
    virtual Carrier *create() {
        return new MpiCarrier();
    }
    virtual String getName() {
        return "mpi";}
    virtual bool isConnectionless() {
        return false;}
        
    virtual bool canEscape() {
        return true;}
    virtual bool supportReply() {
        return false;}
        

    virtual void getHeader(const Bytes& header);
    virtual bool checkHeader(const Bytes& header);


    virtual bool sendHeader(Protocol& proto);
    virtual bool expectSenderSpecifier(Protocol& proto);

    virtual bool respondToHeader(Protocol& proto);
    virtual bool expectReplyToHeader(Protocol& proto);



    /////////////////////////////////////////////////
    // Payload time!

    virtual bool write(Protocol& proto, SizedWriter& writer) {
        writer.write(proto.os());
        return proto.os().isOk();
    }
    virtual bool sendIndex(Protocol& proto) {
        return true; }
    virtual bool expectIndex(Protocol& proto) {
        return true; }

    /////////////////////////////////////////////////
    // Acknowledgements, we don't do them

    virtual bool sendAck(Protocol& proto) {
        return true; }
    virtual bool expectAck(Protocol& proto) {
        return true; }

};



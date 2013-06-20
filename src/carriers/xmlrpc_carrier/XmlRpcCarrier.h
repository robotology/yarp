// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef XMLRPCCARRIER_INC
#define XMLRPCCARRIER_INC

#include <yarp/os/impl/Carrier.h>
#include <yarp/os/impl/Protocol.h>
#include <yarp/os/impl/Address.h>
#include <yarp/os/impl/String.h>
#include "XmlRpcStream.h"

namespace yarp {
    namespace os {
        namespace impl {
            class XmlRpcCarrier;
        }
    }
}

/**
 *
 * This carrier enables XML/RPC message transmission.
 *
 * Example: at the time of writing, there is a public XML/RPC server at
 *   http://phpxmlrpc.sourceforge.net/server.php
 * which has several methods. One is called "examples.addtwo" and expects
 * two integers and returns an integer.  So we can do:
 *
 *   yarp name register /webserve xmlrpc+path.server.php phpxmlrpc.sourceforge.net 80
 *
 * The "80" corresponds to the usual http port number.
 * The "xmlrpc+path.server.php" means "use xmlrpc carrier, and use a request
 * path of server.php".  Often this path can be omitted, but is important
 * for this particular server.
 *
 * Then:
 *   $ yarp rpc /webserve
 *     examples.addtwo 10 20
 * will produce the output "30" if the server still exists.
 *
 */
class yarp::os::impl::XmlRpcCarrier : public Carrier {
private:
    bool firstRound;
    bool sender;
    Address host;
    String http;
    bool interpretRos;
public:
    XmlRpcCarrier() {
        firstRound = true;
        sender = false;
        interpretRos = false;
    }

    virtual Carrier *create() {
        return new XmlRpcCarrier();
    }

    virtual String getName() {
        return "xmlrpc";
    }

    virtual bool isConnectionless() {
        return false;
    }

    virtual bool canAccept() {
        return true;
    }

    virtual bool canOffer() {
        return true;
    }

    virtual bool isTextMode() {
        return true;
    }

    virtual bool canEscape() {
        return true;
    }

    virtual bool requireAck() {
        return false;
    }

    virtual bool supportReply() {
        return true;
    }

    virtual bool isLocal() {
        return false;
    }

    virtual String toString() {
        return "xmlrpc_carrier";
    }

    virtual void getHeader(const Bytes& header) {
        const char *target = "POST /RP";
        for (size_t i=0; i<8 && i<header.length(); i++) {
            header.get()[i] = target[i];
        }
    }

    virtual bool checkHeader(const Bytes& header) {
        if (header.length()!=8) {
            return false;
        }
        const char *target = "POST /";
        for (int i=0; i<6; i++) {
            if (header.get()[i] != target[i]) {
                return false;
            }
        }
        return true;
    }

    virtual void setParameters(const Bytes& header) {
        // no parameters - no carrier variants
    }


    // Now, the initial hand-shaking

    virtual bool prepareSend(Protocol& proto) {
        // nothing special to do
        return true;
    }

    virtual bool sendHeader(Protocol& proto);

    virtual bool expectSenderSpecifier(Protocol& proto);

    virtual bool expectExtraHeader(Protocol& proto) {
        // interpret any extra header information sent - optional
        return true;
    }

    bool respondToHeader(Protocol& proto);

    virtual bool expectReplyToHeader(Protocol& proto) {
        sender = true;
        XmlRpcStream *stream = new XmlRpcStream(proto.giveStreams(),sender,
                                                interpretRos);
        if (stream==NULL) { return false; }
        proto.takeStreams(stream);
        return true;
    }

    virtual bool isActive() {
        return true;
    }


    // Payload time!

    virtual bool write(Protocol& proto, SizedWriter& writer);

    virtual bool reply(Protocol& proto, SizedWriter& writer);

    virtual bool sendIndex(Protocol& proto, SizedWriter& writer) {
        return true;
    }

    virtual bool expectIndex(Protocol& proto) {
        return true;
    }

    virtual bool sendAck(Protocol& proto) {
        return true;
    }

    virtual bool expectAck(Protocol& proto) {
        return true;
    }

    virtual String getBootstrapCarrierName() { return ""; }
};

#endif

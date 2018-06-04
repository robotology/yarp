/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_XMLRPC_CARRIER_XMLRPCCARRIER_H
#define YARP_XMLRPC_CARRIER_XMLRPCCARRIER_H

#include <yarp/os/Carrier.h>
#include "XmlRpcStream.h"

namespace yarp {
    namespace os {
        class XmlRpcCarrier;
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
class yarp::os::XmlRpcCarrier : public Carrier
{
private:
    bool firstRound;
    bool sender;
    Contact host;
    std::string http;
    bool interpretRos;
public:
    XmlRpcCarrier() :
            firstRound(true),
            sender(false),
            interpretRos(false)
    {
    }

    virtual Carrier *create() override
    {
        return new XmlRpcCarrier();
    }

    virtual std::string getName() const override
    {
        return "xmlrpc";
    }

    virtual bool isConnectionless() override
    {
        return false;
    }

    virtual bool canAccept() override
    {
        return true;
    }

    virtual bool canOffer() override
    {
        return true;
    }

    virtual bool isTextMode() const override
    {
        return true;
    }

    virtual bool canEscape() override
    {
        return true;
    }

    virtual bool requireAck() override
    {
        return false;
    }

    virtual bool supportReply() override
    {
        return true;
    }

    virtual bool isLocal() override
    {
        return false;
    }

    virtual std::string toString() override
    {
        return "xmlrpc_carrier";
    }

    virtual void getHeader(const Bytes& header) override
    {
        const char *target = "POST /RP";
        for (size_t i=0; i<8 && i<header.length(); i++) {
            header.get()[i] = target[i];
        }
    }

    virtual bool checkHeader(const Bytes& header) override
    {
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

    virtual void setParameters(const Bytes& header) override
    {
        // no parameters - no carrier variants
    }


    // Now, the initial hand-shaking

    virtual bool prepareSend(ConnectionState& proto) override
    {
        // nothing special to do
        return true;
    }

    virtual bool sendHeader(ConnectionState& proto) override;

    virtual bool expectSenderSpecifier(ConnectionState& proto) override;

    virtual bool expectExtraHeader(ConnectionState& proto) override
    {
        // interpret any extra header information sent - optional
        return true;
    }

    bool respondToHeader(ConnectionState& proto) override;

    virtual bool expectReplyToHeader(ConnectionState& proto) override
    {
        sender = true;
        XmlRpcStream *stream = new XmlRpcStream(proto.giveStreams(),sender,
                                                interpretRos);
        if (stream == nullptr) {
            return false;
        }
        proto.takeStreams(stream);
        return true;
    }

    virtual bool isActive() override
    {
        return true;
    }


    // Payload time!

    virtual bool write(ConnectionState& proto, SizedWriter& writer) override;

    virtual bool reply(ConnectionState& proto, SizedWriter& writer) override;

    virtual bool sendIndex(ConnectionState& proto, SizedWriter& writer)
    {
        return true;
    }

    virtual bool expectIndex(ConnectionState& proto) override
    {
        return true;
    }

    virtual bool sendAck(ConnectionState& proto) override
    {
        return true;
    }

    virtual bool expectAck(ConnectionState& proto) override
    {
        return true;
    }

    virtual std::string getBootstrapCarrierName() override
    {
        return "";
    }

private:
    bool shouldInterpretRosMessages(ConnectionState& proto);
};

#endif // YARP_XMLRPC_CARRIER_XMLRPCCARRIER_H

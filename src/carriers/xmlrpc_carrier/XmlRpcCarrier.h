/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_XMLRPC_CARRIER_XMLRPCCARRIER_H
#define YARP_XMLRPC_CARRIER_XMLRPCCARRIER_H

#include <yarp/os/Carrier.h>
#include <yarp/os/ConnectionState.h>
#include "XmlRpcStream.h"

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
class XmlRpcCarrier :
        public yarp::os::Carrier
{
private:
    bool firstRound;
    bool sender;
    yarp::os::Contact host;
    std::string http;
    bool interpretRos;
public:
    XmlRpcCarrier() :
            firstRound(true),
            sender(false),
            interpretRos(false)
    {
    }

    Carrier *create() const override
    {
        return new XmlRpcCarrier();
    }

    std::string getName() const override
    {
        return "xmlrpc";
    }

    bool isConnectionless() const override
    {
        return false;
    }

    bool canAccept() const override
    {
        return true;
    }

    bool canOffer() const override
    {
        return true;
    }

    bool isTextMode() const override
    {
        return true;
    }

    bool canEscape() const override
    {
        return true;
    }

    bool requireAck() const override
    {
        return false;
    }

    bool supportReply() const override
    {
        return true;
    }

    bool isLocal() const override
    {
        return false;
    }

    std::string toString() const override
    {
        return "xmlrpc_carrier";
    }

    void getHeader(yarp::os::Bytes& header) const override
    {
        const char *target = "POST /RP";
        for (size_t i=0; i<8 && i<header.length(); i++) {
            header.get()[i] = target[i];
        }
    }

    bool checkHeader(const yarp::os::Bytes& header) override
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

    void setParameters(const yarp::os::Bytes& header) override
    {
        // no parameters - no carrier variants
    }


    // Now, the initial hand-shaking

    bool prepareSend(yarp::os::ConnectionState& proto) override
    {
        // nothing special to do
        return true;
    }

    bool sendHeader(yarp::os::ConnectionState& proto) override;

    bool expectSenderSpecifier(yarp::os::ConnectionState& proto) override;

    bool expectExtraHeader(yarp::os::ConnectionState& proto) override
    {
        // interpret any extra header information sent - optional
        return true;
    }

    bool respondToHeader(yarp::os::ConnectionState& proto) override;

    bool expectReplyToHeader(yarp::os::ConnectionState& proto) override
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

    bool isActive() const override
    {
        return true;
    }


    // Payload time!

    bool write(yarp::os::ConnectionState& proto, yarp::os::SizedWriter& writer) override;

    bool reply(yarp::os::ConnectionState& proto, yarp::os::SizedWriter& writer) override;

    virtual bool sendIndex(yarp::os::ConnectionState& proto, yarp::os::SizedWriter& writer)
    {
        return true;
    }

    bool expectIndex(yarp::os::ConnectionState& proto) override
    {
        return true;
    }

    bool sendAck(yarp::os::ConnectionState& proto) override
    {
        return true;
    }

    bool expectAck(yarp::os::ConnectionState& proto) override
    {
        return true;
    }

    std::string getBootstrapCarrierName() const override
    {
        return {};
    }

private:
    bool shouldInterpretRosMessages(yarp::os::ConnectionState& proto);
};

#endif // YARP_XMLRPC_CARRIER_XMLRPCCARRIER_H

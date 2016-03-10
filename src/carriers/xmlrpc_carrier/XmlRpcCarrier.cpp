/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "XmlRpcCarrier.h"

#include "XmlRpc.h"

#include <yarp/os/StringOutputStream.h>
#include <yarp/os/Name.h>
#include <yarp/os/NetType.h>
#include <yarp/os/Bottle.h>

#include <cstdio>

using namespace yarp::os;
using namespace YarpXmlRpc;

void toXmlRpcValue(Value& vin, XmlRpcValue& vout) {
    if (vin.isInt()) {
        vout = vin.asInt();
    } else if (vin.isDouble()) {
        vout = vin.asDouble();
    } else if (vin.isString()) {
        vout = std::string(vin.asString());
    } else if (vin.isVocab()) {
        vout = std::string("[") + std::string(vin.toString()) + "]";
    } else if (vin.isList()) {
        Bottle *bot = vin.asList();
        bool struc = true;
        int offset = 0;
        ConstString tag = bot->get(0).asString();
        if (tag=="list") {
            struc = false;
            offset = 1;
        } else if (tag=="dict") {
            struc = true;
            offset = 1;
        } else {
            // auto-detect
            for (int i=0; i<bot->size(); i++) {
                Value& vi = bot->get(i);
                if (!vi.isList()) {
                    struc = false;
                    break;
                }
                if (vi.asList()->size()!=2) {
                    struc = false;
                    break;
                }
            }
        }
        if (struc) {
            vout = XmlRpcValue();
            for (int i=offset; i<bot->size(); i++) {
                Bottle *boti = bot->get(i).asList();
                XmlRpcValue& vouti=vout[std::string(boti->get(0).toString())]=XmlRpcValue();
                toXmlRpcValue(boti->get(1),vouti);
            }
        } else {
            vout = XmlRpcValue();
            for (int i=offset; i<bot->size(); i++) {
                XmlRpcValue& vouti = vout[i] = XmlRpcValue();
                toXmlRpcValue(bot->get(i),vouti);
            }
        }
    }
}

bool XmlRpcCarrier::expectSenderSpecifier(ConnectionState& proto) {
    proto.setRoute(proto.getRoute().addFromName("rpc"));
    return true;
}

bool XmlRpcCarrier::write(ConnectionState& proto, SizedWriter& writer) {
    StringOutputStream sos;
    StringInputStream sis;
    writer.write(sos);
    sis.reset(sos.toString());
    ConstString header;
    if (sender) {
        header = sis.readLine();
    }
    ConstString body = sis.readLine();
    Value v;
    if (header.length()>0 && header[0]=='q') {
        body = "yarp.quit";
        // XMLRPC does not need a quit message, this should get stripped
        return false;
    }
    Bottle *bot = v.asList();
    bot->fromString(body.c_str());
    ConstString methodName;
    if (sender) {
        methodName = bot->get(0).toString();
        *bot = bot->tail();
    }
    XmlRpcValue args;
    if (bot->size()==1) {
        toXmlRpcValue(bot->get(0),args);
    } else {
        toXmlRpcValue(v,args);
    }
    std::string req;
    if (sender) {
        const Contact& addr = host.isValid()?host:proto.getStreams().getRemoteAddress();
        XmlRpcClient c(addr.getHost().c_str(),(addr.getPort()>0)?addr.getPort():80);
        c.generateRequest(methodName.c_str(),args);
        req = c.getRequest();
    } else {
        XmlRpcServerConnection c(0,NULL);
        c.generateResponse(args.toXml());
        req = c.getResponse();
    }
    int start = 0;
    if (sender) {
        if (req.length()<8) {
            fprintf(stderr, "XmlRpcCarrier fail, %s:%d\n", __FILE__, __LINE__);
            return false;
        }
        for (int i=0; i<(int)req.length(); i++) {
            if (req[i] == '\n') {
                start++;
                break;
            }
            start++;
        }
        if (!firstRound) {
            Bytes b((char*)http.c_str(),http.length());
            proto.os().write(b);
        }
        firstRound = false;
    }
    Bytes b((char*)req.c_str()+start,req.length()-start);
    proto.os().write(b);

    return proto.os().isOk();
}


bool XmlRpcCarrier::reply(ConnectionState& proto, SizedWriter& writer) {
    return write(proto,writer);
}


bool XmlRpcCarrier::shouldInterpretRosMessages(ConnectionState& proto) {
    // We need to set the interpretRos flag, which controls
    // whether ROS-style admin messages are treated as
    // admin messages or data messages in YARP.
    // In the future, they should always be data messages.
    // For now, they should be admin messages for all ports
    // except ports tagged as corresponding to ros nodes.

    bool nodelike = false;
    Contactable *port = proto.getContactable();
    Property opt;
    if (port) {
        Property *pport = port->acquireProperties(true);
        if (pport) {
            opt = *pport;
        }
        port->releaseProperties(pport);
    }
    if (opt.check("node_like")) {
        nodelike = true;
    }

    Name n(proto.getRoute().getCarrierName() + "://test");
    ConstString rospass = n.getCarrierModifier("ros");
    interpretRos = !nodelike;
    if (rospass=="1"||rospass=="on") {
        interpretRos = true;
    }
    if (rospass=="0"||rospass=="off") {
        interpretRos = false;
    }
    return interpretRos;
}

bool XmlRpcCarrier::sendHeader(ConnectionState& proto) {
    shouldInterpretRosMessages(proto);
    ConstString target = "POST /RPC2";
    Name n(proto.getRoute().getCarrierName() + "://test");
    ConstString pathValue = n.getCarrierModifier("path");
    if (pathValue!="") {
        target = "POST /";
        target += pathValue;
        // on the wider web, we should provide real host names
        host = NetworkBase::queryName(proto.getRoute().getToName());
    }
    target += " HTTP/1.1\n";
    http = target;
    Bytes b((char*)target.c_str(),target.length());
    proto.os().write(b);
    return true;
}


bool XmlRpcCarrier::respondToHeader(ConnectionState& proto) {
    shouldInterpretRosMessages(proto);
    sender = false;
    XmlRpcStream *stream = new XmlRpcStream(proto.giveStreams(),
                                            sender,
                                            interpretRos);
    if (stream==NULL) { return false; }
    proto.takeStreams(stream);
    return true;
}

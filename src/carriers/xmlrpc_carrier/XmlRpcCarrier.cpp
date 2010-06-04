// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "XmlRpcCarrier.h"
#include <yarp/os/impl/StringOutputStream.h>
#include <yarp/os/Bottle.h>

#include "XmlRpc.h"

using namespace yarp::os::impl;
using namespace yarp::os;
using namespace XmlRpc;

void toXmlRpcValue(Value& vin, XmlRpcValue& vout) {
    if (vin.isInt()) {
        vout = vin.asInt();
    } else if (vin.isDouble()) {
        vout = vin.asDouble();
    } else if (vin.isString()) {
        vout = vin.asString();
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
                XmlRpcValue& vouti=vout[boti->get(0).toString()]=XmlRpcValue();
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

bool XmlRpcCarrier::expectSenderSpecifier(Protocol& proto) {
    proto.setRoute(proto.getRoute().addFromName("rpc"));
    return true;
}

bool XmlRpcCarrier::write(Protocol& proto, SizedWriter& writer) {
    //XmlRpc::setVerbosity(10);
    StringOutputStream sos;
    StringInputStream sis;
    writer.write(sos);
    sis.reset(sos.toString());
    String header;
    if (sender) {
        header = NetType::readLine(sis);
    }
    String body = NetType::readLine(sis);
    //printf("Asked to write: hdr %s body %s\n",
    //     header.c_str(), body.c_str());
    Value v;
    //printf("HEADER %s\n", header.c_str());
    if (header[0]=='q') {
        body = "yarp.quit";
        // XMLRPC does not need a quit message, this should get stripped
    }
    Bottle *bot = v.asList();
    //Bottle aux;
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
    //printf("xmlrpc block to write is %s\n", args.toXml().c_str());
    std::string req;
    if (sender) {
        const Address& addr = proto.getStreams().getRemoteAddress();
        XmlRpcClient c(addr.getName().c_str(),addr.getPort());
        c.generateRequest(methodName.c_str(),args);
        req = c.getRequest();
    } else {
        XmlRpcServerConnection c(0,NULL);
        c.generateResponse(args.toXml());
        req = c.getResponse();
    }
    int start = 0;
    //printf("converts to %s\n", req.c_str());
    if (sender) {
        if (req.length()<8) {
            fprintf(stderr, "XmlRpcCarrier fail, %s:%d\n", __FILE__, __LINE__);
            return false;
        }
        if (firstRound) {
            const char *target = "POST /RP";
            for (int i=0; i<8; i++) {
                if (req[i] != target[i]) {
                    fprintf(stderr, "XmlRpcCarrier fail, %s:%d\n", __FILE__, __LINE__);
                    return false;
                }
            }
            start = 8;
            firstRound = false;
        }
    }
    Bytes b((char*)req.c_str()+start,req.length());
    proto.os().write(b);

    return proto.os().isOk();
}


bool XmlRpcCarrier::reply(Protocol& proto, SizedWriter& writer) {
    return write(proto,writer);
}



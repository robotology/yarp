// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "XmlRpcStream.h"
#include "XmlRpcValue.h"

#include <yarp/os/Bottle.h>

using namespace yarp::os::impl;
using namespace yarp::os;
using namespace XmlRpc;
using namespace std;


Value toValue(XmlRpcValue& v, bool outer) {
    int t = v.getType();
    switch (t) {
    case XmlRpcValue::TypeInt:
        return Value((int)v);
        break;
    case XmlRpcValue::TypeDouble:
        return Value((double)v);
        break;
    case XmlRpcValue::TypeString:
        {
            string s = (string)v;
            if (s.length()==0 || s[0]!='[') {
                return Value(s.c_str());
            } else {
                Value v;
                v.fromString(s.c_str());
                return v;
            }
        }
        break;
    case XmlRpcValue::TypeArray:
        {
            Value vbot;
            Bottle *bot = vbot.asList();
            for (int i=0; i<v.size(); i++) {
                XmlRpcValue& v2 = v[i];
                if (v2.getType()!=XmlRpcValue::TypeInvalid) {
                    Value v = toValue(v2,false);
                    if (i==0) {
                        ConstString tag = v.asString();
                        if (tag=="list"||tag=="dict") {
                            if (!outer) {
                                bot->addString("list");
                            }
                        }
                    }
                    bot->add(v);
                }
            }
            return vbot;
        }
        break;
    case XmlRpcValue::TypeStruct:
        {
            Value vbot;
            Bottle *bot = vbot.asList();
            XmlRpcValue::ValueStruct& vals = v;
            bot->addString("dict");
            for (XmlRpcValue::ValueStruct::iterator it = vals.begin();
                 it!= vals.end();
                 it++) {
                XmlRpcValue& v2 = it->second;
                Bottle& sub = bot->addList();
                sub.addString(it->first.c_str());
                if (v2.getType()!=XmlRpcValue::TypeInvalid) {
                    sub.add(toValue(v2,false));
                }
            }
            return vbot;
        }
        break;
    case XmlRpcValue::TypeInvalid:
        return Value::getNullValue();
        break;
    }
    //printf("Skipping %d\n", t);
    return Value("(type not supported yet out of laziness)");
}

YARP_SSIZE_T XmlRpcStream::read(const Bytes& b) {
    //printf("XMLRPC READ\n");
    YARP_SSIZE_T result = sis.read(b);
    if (result>0) {
        //printf("RETURNING %d bytes\n", result);
        return result;
    }
    //printf("No string\n");
    if (result==0) {
        //printf("Reading...\n");
        bool ok = false;
        if (sender) {
            client.reset();
        } else {
            server.reset();
        }
        if (firstRound) {
            if (sender) {
                client.read("POST /RP");
            } else {
                server.read("POST /RP");
            }
            firstRound = false;
        }
        char buf[1000];
        Bytes bytes(buf,sizeof(buf));
        while (!ok) {
            int result2 = delegate->getInputStream().partialRead(bytes);
            if (result2<=0) {
                return result2;
            }
            string s(buf,result2);
            //printf("Giving %s to parser\n", s.c_str());
            if (sender) {
                ok = client.read(s);
            } else {
                ok = server.read(s);
            }
            if (ok) {
                //printf("got a block!\n");
                XmlRpcValue xresult;
                std::string prefix = "";
                std::string cprefix = "";
                if (sender) {
                    client.parseResponse(xresult);
                } else {
                    cprefix = server.parseRequest(xresult);
                    bool isAdmin = false;
                    if (interpretRos) {
                        if (cprefix=="publisherUpdate") {
                            isAdmin = true;
                        }
                        if (cprefix=="requestTopic") {
                            isAdmin = true;
                        }
                        if (cprefix=="getPid") {
                            isAdmin = true;
                        }
                        if (cprefix=="getBusInfo") {
                            isAdmin = true;
                        }
                    }
                    prefix = isAdmin?"a\n":"d\n";
                    prefix += cprefix;
                    prefix += " ";
                }
                //printf("xmlrpc block is %s\n", xresult.toXml().c_str());
                Value v = toValue(xresult,true);
                if (!v.isNull()) {
                    sis.reset((prefix + v.toString().c_str() + "\n").c_str());
                } else {
                    sis.reset((prefix + "\n").c_str());
                }
                //printf("String version is %s\n", sis.toString().c_str());
                result = sis.read(b);
                break;
            }
        }
    }
    //printf("RETURNING %d bytes\n", result);
    return (result>0)?result:-1;
}


void XmlRpcStream::write(const Bytes& b) {
    delegate->getOutputStream().write(b);
}

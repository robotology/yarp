/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "XmlRpcStream.h"
#include "XmlRpcValue.h"
#include "XmlRpcLogComponent.h"

#include <yarp/os/Bottle.h>
#include <yarp/os/Value.h>

using namespace yarp::os;
using namespace std;
using YarpXmlRpc::XmlRpcValue;

Value toValue(XmlRpcValue& v, bool outer)
{
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
                return Value(s);
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
                        std::string tag = v.asString();
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
            for (auto& val : vals) {
                XmlRpcValue& v2 = val.second;
                Bottle& sub = bot->addList();
                sub.addString(val.first.c_str());
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
    yCTrace(XMLRPCCARRIER, "Skipping %d", t);
    return Value("(type not supported yet out of laziness)");
}

yarp::conf::ssize_t XmlRpcStream::read(Bytes& b)
{
    yCTrace(XMLRPCCARRIER, "XMLRPC READ");
    yarp::conf::ssize_t result = sis.read(b);
    if (result>0) {
        yCTrace(XMLRPCCARRIER, "RETURNING %zd bytes", result);
        return result;
    }
    yCTrace(XMLRPCCARRIER, "No string");
    if (result==0) {
        yCTrace(XMLRPCCARRIER, "Reading...");
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
            yCTrace(XMLRPCCARRIER, "Giving %s to parser", s.c_str());
            if (sender) {
                ok = client.read(s);
            } else {
                ok = server.read(s);
            }
            if (ok) {
                yCTrace(XMLRPCCARRIER, "got a block!");
                XmlRpcValue xresult;
                std::string prefix;
                std::string cprefix;
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
                yCTrace(XMLRPCCARRIER, "xmlrpc block is %s", xresult.toXml().c_str());
                Value v = toValue(xresult,true);
                if (!v.isNull()) {
                    sis.reset(prefix + v.toString() + "\n");
                } else {
                    sis.reset(prefix + "\n");
                }
                yCTrace(XMLRPCCARRIER, "String version is %s", sis.toString().c_str());
                result = sis.read(b);
                break;
            }
        }
    }
    yCTrace(XMLRPCCARRIER, "RETURNING %zd bytes", result);
    return (result>0)?result:-1;
}


void XmlRpcStream::write(const Bytes& b)
{
    delegate->getOutputStream().write(b);
}

// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/HttpCarrier.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Property.h>

using namespace yarp;
using namespace yarp::os;

HttpTwoWayStream::HttpTwoWayStream(TwoWayStream *delegate, const char *txt) :
    delegate(delegate) {

    data = false;
    filterData = false;
    String s(txt);
    Property p;
    p.fromQuery(txt);
    if (p.check("cmd")) {
        s = p.check("cmd",Value("")).asString().c_str();
    } else if (p.check("data")) {
        s = p.check("data",Value("")).asString().c_str();
        Bottle bin(s.c_str()), bout;
        for (int i=0; i<bin.size(); i++) {
            Value& val = bin.get(i);
            bool done = false;
            if (val.isString()) {
                if (val.asString()[0]=='$') {
                    String key = val.asString().c_str();
                    key = key.substr(1,String::npos);
                    if (p.check(key.c_str())) {
                        bout.add(Value::makeValue(p.check(key.c_str(),Value("")).asString()));
                        done = true;
                    }
                }
            }
            if (!done) {
                bout.add(val);
            }
        }
        s = bout.toString().c_str();
        printf("data message: %s\n", s.c_str());
        s = String("d\n") + s;
    }
    if (s=="") {
        s = "*";
    }
    if (s[0]=='r') {
        filterData = true;
        String msg = "Reading data from port...\n";
        Bytes tmp((char*)msg.c_str(),msg.length());
        delegate->getOutputStream().write(tmp);
        delegate->getOutputStream().flush();
    }
    for (unsigned int i=0; i<s.length(); i++) {
        if (s[i]==',') {
            s[i] = '\n';
        }
        if (s[i]=='+') {
            s[i] = ' ';
        }
    }
    sis.add(s);
    sis.add("\nq\nq\nq\n");
}



void HttpCarrier::expectSenderSpecifier(Protocol& proto) {
    proto.setRoute(proto.getRoute().addFromName("web"));
    String remainder = NetType::readLine(proto.is());
    if (!urlDone) {
        for (unsigned int i=0; i<remainder.length(); i++) {
            if (remainder[i]!=' ') {
                url += remainder[i];
            } else {
                break;
            }
        }
    }

    bool done = false;
    expectPost = false;
    contentLength = 0;
    while (!done) {
        String result = NetType::readLine(proto.is());
        if (result == "") {
            done = true;
        } else {
            //printf(">>> %s\n", result.c_str());
            Bottle b;
            b.fromString(result.c_str());
            if (b.get(0).asString()=="Content-Length:") {
                //printf("]]] got length %d\n", b.get(1).asInt());
                contentLength = b.get(1).asInt();
            }
            if (b.get(0).asString()=="Content-Type:") {
                //printf("]]] got type %s\n", b.get(1).asString());
                if (b.get(1).asString()=="application/x-www-form-urlencoded") {
                    expectPost = true;
                }
            }
        }
    }

    if (expectPost) {
        //printf("[[[this is a post message of length %d]]]\n", contentLength);
        ManagedBytes blk(contentLength+1);
        Bytes start(blk.get(),contentLength);
        NetType::readFull(proto.is(),start);
        blk.get()[contentLength] = '\0';
        printf("message: %s\n", blk.get());
        input = blk.get();
    } else {
        printf("message: %s\n", url.c_str());
        input = url;
    }

    String from = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html><body bgcolor='#ffffcc'><h1>yarp port ";
    from += proto.getRoute().getToName();
    from += "</h1>\n";
    Address home = NameClient::getNameClient().getAddress();
    Address me = proto.getStreams().getLocalAddress();

    from += "<p>(<a href=\"http://";
    from += home.getName();
    from += ":";
    from += NetType::toString(home.getPort());
    from += "/d,list\">All ports</a>)&nbsp;&nbsp;\n";

    from += "(<a href=\"http://";
    from += me.getName();
    from += ":";
    from += NetType::toString(me.getPort());
    from += "/\">connections</a>)&nbsp;&nbsp;\n";

    from += "(<a href=\"http://";
    from += me.getName();
    from += ":";
    from += NetType::toString(me.getPort());
    from += "/data=help\">help</a>)&nbsp;&nbsp;\n";

    from += "(<a href=\"http://";
    from += me.getName();
    from += ":";
    from += NetType::toString(me.getPort());
    from += "/r\">read</a>)&nbsp;&nbsp;\n";

    from += "</p>\n"; 
    from += "<p>\n"; 
    from += "<form method=\"post\" action=\"http://";
    from += home.getName();
    from += ":";
    from += NetType::toString(me.getPort());
    from += "\"<input type=text name=data value=\"\"><input type=submit value=\"send data\"></form></p>\n"; 
    from += "<pre>\n";
    Bytes b2((char*)from.c_str(),from.length());
    proto.os().write(b2);
    proto.os().flush();
    // Message gets finished by the stream
}

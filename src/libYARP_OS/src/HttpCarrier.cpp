// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/impl/HttpCarrier.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Property.h>

using namespace yarp::os::impl;
using namespace yarp::os;


static String quoteFree(const String& src) {
    String result = "";
    for (unsigned int i=0; i<src.length(); i++) {
        char ch = src[i];
        if (ch=='"') {
            result += "&quot;";
        } else {
            result += ch;
        }
    }
    return result;
}

void HttpTwoWayStream::apply(char ch) {
    if (ch=='\r') { return; }
    if (ch == '\n') {
        proc = "";
        Address addr = NameClient::extractAddress(part);
        if (addr.isValid()) {
            if (addr.getCarrierName()=="tcp"&&
                (YARP_STRSTR(addr.getRegName(),"/quit")==String::npos)) {
                proc += "<a href=\"http://";
                proc += addr.getName();
                proc += ":";
                proc += NetType::toString(addr.getPort());
                proc += "\">";
                proc += addr.getRegName();
                proc += "</A> "; 
                unsigned int len = addr.getRegName().length();
                unsigned int target = 30;
                if (len<target) {
                    for (unsigned int i=0; i<target-len; i++) {
                        proc += " ";
                    }
                }
                proc += "(";
                proc += addr.toString().c_str();
                proc += ")";
                proc += "\n";
            } else {
                // Don't show non tcp connections
                //proc += part;
                //proc += "\n";
            }
        } else {
            if ((part[0]=='\"'&&part[1]=='[')||(part[0]=='+')) {
                // translate this to a form
                if (part[0]=='+') { part[0] = ' '; }
                String org = part;
                part = "<p><form method=post action='/form'>";
				unsigned int i=0;
                for (i=0; i<org.length(); i++) {
                    if (org[i]=='"') {
                        org[i] = ' ';
                    }
                }
                part += "<input type=hidden name=data value=\"";
                part += org;
                part += "\">";
                part += org;
                org += " ";
                bool arg = false;
                String var = "";
                for (i=0; i<org.length(); i++) {
                    char ch = org[i];
                    if (arg) {
                        if ((ch>='A'&&ch<='Z')||
                            (ch>='a'&&ch<='z')||
                            (ch>='0'&&ch<='9')||
                            (ch=='_')) {
                            var += ch;
                        } else {
                            arg = false;
                            part += "\n    ";
                            part += var;
                            part += " ";
                            part += "<input type=text name=";
                            part += var;
                            part += " size=5 value=\"\">";
                            var = "";
                        }
                    }
                    if (ch=='$') {
                        arg = true;
                    }
                }
                part += "<input type=submit value=\"go\">";
                part += "</form></p>";
            }
            proc += part;
            proc += "\n";
        }
        if (data||!filterData) {
            Bytes tmp((char*)proc.c_str(),proc.length());
            delegate->getOutputStream().write(tmp);
            delegate->getOutputStream().flush();
        }
        data = false;
        if (proc[0] == 'd' || proc[0] == 'D') {
            data = true;
        }
        part = "";
    } else {
        part += ch;
    }
}



HttpTwoWayStream::HttpTwoWayStream(TwoWayStream *delegate, const char *txt) :
    delegate(delegate) {

    data = false;
    filterData = false;
    String s(txt);
    String sData = "";
    Property p;
    p.fromQuery(txt);
    if (p.check("cmd")) {
        s = p.check("cmd",Value("")).asString().c_str();
    } else if (p.check("data")) {
        s = p.check("data",Value("")).asString().c_str();
        s += " ";
        String sFixed = "";
        String var = "";
        bool arg = false;
        for (unsigned int i=0; i<s.length(); i++) {
            char ch = s[i];
            if (arg) {
                if ((ch>='A'&&ch<='Z')||
                    (ch>='a'&&ch<='z')||
                    (ch>='0'&&ch<='9')||
                    (ch=='_')) {
                    var += ch;
                } else {
                    arg = false;
                    sFixed+=p.check(var.c_str(),Value("")).toString().c_str();
                    if (i!=s.length()-1) {
                        sFixed += ch; // omit padding
                    }
                    var = "";
                }
            } else {
                if (ch=='$') {
                    arg = true;
                } else {
                    if (i!=s.length()-1) {
                        sFixed += ch; // omit padding
                    }
                }
            }
        }


        Bottle bin(sFixed.c_str());
        sData = sFixed;
        s = String("d\n") + sFixed;
    }


    String from = "<input type=text name=data value=\"";
    from += quoteFree(sData.c_str());

    from += "\"><input type=submit value=\"send data\"></form></p>\n"; 
    from += "<pre>\n";
    Bytes b2((char*)from.c_str(),from.length());
    delegate->getOutputStream().write(b2);
    delegate->getOutputStream().flush();


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



bool HttpCarrier::expectSenderSpecifier(Protocol& proto) {
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
        //printf("message: %s\n", blk.get());
        input = blk.get();
    } else {
        //printf("message: %s\n", url.c_str());
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
    from += "/data=list\">All ports</a>)&nbsp;&nbsp;\n";

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
    from += me.getName();
    from += ":";
    from += NetType::toString(me.getPort());
    from += "/form\">";
    Bytes b2((char*)from.c_str(),from.length());
    proto.os().write(b2);
    proto.os().flush();
    // Message gets finished by the stream

    return proto.os().isOk();
}

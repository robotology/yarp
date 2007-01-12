// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_HTTPCARRIER_
#define _YARP2_HTTPCARRIER_

#include <yarp/TcpCarrier.h>
#include <yarp/NameClient.h>
#include <yarp/NetType.h>

namespace yarp {
    class HttpCarrier;
    class HttpTwoWayStream;
}


class yarp::HttpTwoWayStream : public TwoWayStream {
private:
    TwoWayStream *delegate;
    StringInputStream sis;
    StringOutputStream sos;
public:
    HttpTwoWayStream(TwoWayStream *delegate, const char *txt) : 
        delegate(delegate) {
        String s(txt);
        if (s=="") {
            s = "*";
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

    virtual ~HttpTwoWayStream() {
        if (delegate!=NULL) {
            delete delegate;
            delegate = NULL;
        }
    }

    virtual InputStream& getInputStream() { return sis; }
    virtual OutputStream& getOutputStream() { return sos; }


    virtual const Address& getLocalAddress() {
        return delegate->getLocalAddress();
    }

    virtual const Address& getRemoteAddress() {
        return delegate->getRemoteAddress();
    }

    virtual bool isOk() {
        return delegate->isOk();
    }

    virtual void reset() {
        delegate->reset();
    }

    virtual void close() {
        //printf("Closing\n");
        String str = sos.toString().c_str();
        //printf(">>> %s\n", str.c_str());
        String proc = "";
        String part = "";
        for (unsigned int i=0; i<str.length(); i++) {
            char ch = str[i];
            if (ch=='\r') { continue; }
            part += ch;
            if (ch == '\n') {
                Address addr = NameClient::extractAddress(part);
                if (addr.isValid()) {
                    if (addr.getCarrierName()=="tcp") {
                        proc += "<a href=\"http://";
                        proc += addr.getName();
                        proc += ":";
                        proc += NetType::toString(addr.getPort());
                        proc += "\">";
                        proc += part;
                        proc += "</A>\n";
                    } else {
                        proc += part;
                    }
                }
                part = "";
            }
        }

        Bytes tmp((char*)proc.c_str(),proc.length());
        delegate->getOutputStream().write(tmp);
        delegate->close();
    }

    virtual void beginPacket() {
        delegate->beginPacket();
    }

    virtual void endPacket() {
        delegate->endPacket();
    }
};



/**
 * Communicating via http.
 */
class yarp::HttpCarrier : public TcpCarrier {
private:
    String url;
    bool urlDone;
public:
    HttpCarrier() {
        url = "";
        urlDone = false;
    }

    virtual String getName() {
        return "http";
    }

    virtual bool checkHeader(const Bytes& header) {
        if (header.length()==8) {
            String target = "GET /";
            for (unsigned int i=0; i<target.length(); i++) {
                if (!(target[i]==header.get()[i])) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    virtual void setParameters(const Bytes& header) {
        if (header.length()==8) {
            String target = "GET /";
            for (unsigned int j=target.length(); j<8; j++) {
                char ch = header.get()[j];
                if (ch!=' ') {
                    url += ch;
                } else {
                    urlDone = true;
                    break;
                }
            }
        }
    }

    virtual void getHeader(const Bytes& header) {
        if (header.length()==8) {
            String target = "GET / HT";
            for (int i=0; i<8; i++) {
                header.get()[i] = target[i];
            }
        }   
    }

    virtual Carrier *create() {
        return new HttpCarrier();
    }

    virtual bool requireAck() {
        return false;
    }

    virtual bool isTextMode() {
        return true;
    }


    virtual bool supportReply() {
        return false;
    }

    virtual void sendHeader(Protocol& proto) {
        printf("not yet meant to work\n");
        String target = "GET / HTTP/1.1";
        Bytes b((char*)target.c_str(),8);
        proto.os().write(b);
        /*
        String target = getSpecifierName();
        Bytes b((char*)target.c_str(),8);
        proto.os().write(b);
        String from = proto.getRoute().getFromName();
        Bytes b2((char*)from.c_str(),from.length());
        proto.os().write(b2);
        proto.os().write('\r');
        proto.os().write('\n');
        proto.os().flush();
        */

    }

    void expectReplyToHeader(Protocol& proto) {
        // expect and ignore CONTENT lines
        String result = NetType::readLine(proto.is());
    }

    void expectSenderSpecifier(Protocol& proto) {
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
        while (!done) {
            String result = NetType::readLine(proto.is());
            if (result == "") {
                done = true;
            } else {
                //printf(">>> %s\n", result.c_str());
            }
        }


        String from = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html><body bgcolor='#ffffcc'><h1>yarp port ";
        from += proto.getRoute().getToName();
        from += "</h1>\n";
        Address home = NameClient::getNameClient().getAddress();
        from += "<p>(<a href=\"http://";
        from += home.getName();
        from += ":";
        from += NetType::toString(home.getPort());
        from += "/d,list\">see port index</a>)</p>\n";
        from += "\n<pre>\n";
        Bytes b2((char*)from.c_str(),from.length());
        proto.os().write(b2);
        proto.os().flush();
        //proto.os().close();

    }

    void sendIndex(Protocol& proto) {
    }

    void expectIndex(Protocol& proto) {
    }

    void sendAck(Protocol& proto) {
    }

    virtual void expectAck(Protocol& proto) {
    }

    void respondToHeader(Protocol& proto) {
        HttpTwoWayStream *stream = 
            new HttpTwoWayStream(proto.giveStreams(),url.c_str());
        proto.takeStreams(stream);
    }
};

#endif

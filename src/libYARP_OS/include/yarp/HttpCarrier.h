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


class yarp::HttpTwoWayStream : public TwoWayStream, public OutputStream {
private:
    String proc;
    String part;
    bool data;
    bool filterData;
    TwoWayStream *delegate;
    StringInputStream sis;
    StringOutputStream sos;
public:
    HttpTwoWayStream(TwoWayStream *delegate, const char *txt) : 
        delegate(delegate) {
        data = false;
        filterData = false;
        String s(txt);
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

    virtual ~HttpTwoWayStream() {
        if (delegate!=NULL) {
            delete delegate;
            delegate = NULL;
        }
    }

    virtual InputStream& getInputStream() { return sis; }
    virtual OutputStream& getOutputStream() { return *this; }


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

    virtual void write(const Bytes& b) { // throws
        for (int i=0; i<b.length(); i++) {
            apply(b.get()[i]);
        }
    }

    virtual void apply(char ch) {
        if (ch=='\r') { return; }
        if (ch == '\n') {
            proc = "";
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
                    proc += "\n";
                }
            } else {
                proc += part;
                proc += "\n";
            }
            if (data||!filterData) {
                Bytes tmp((char*)proc.c_str(),proc.length());
                delegate->getOutputStream().write(tmp);
                delegate->getOutputStream().flush();
            }
            data = false;
            if (proc[0] == 'd') {
                data = true;
            }
            part = "";
        } else {
            part += ch;
        }
    }

    virtual void close() {
        apply('\n');
        apply('\n');
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
        from += "/r\">read</a>)&nbsp;&nbsp;\n";

        from += "</p>\n<pre>\n";
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

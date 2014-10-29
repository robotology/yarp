// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_STREAMBLOCKREADER_
#define _YARP2_STREAMBLOCKREADER_

#include <yarp/os/InputStream.h>
#include <yarp/os/TwoWayStream.h>
#include <yarp/os/StringInputStream.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/NetType.h>
#include <yarp/os/Bytes.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/Route.h>
#include <yarp/os/Contact.h>
#include <yarp/os/impl/PlatformSize.h>
#include <yarp/os/Bottle.h>

namespace yarp {
    namespace os {
        namespace impl {
            class Protocol;
            class StreamConnectionReader;
            class BufferedConnectionWriter;
        }
    }
}


/**
 * Lets Readable objects read from the underlying InputStream
 * associated with the connection between two ports.
 */
class YARP_OS_impl_API yarp::os::impl::StreamConnectionReader : public ConnectionReader {
public:
    StreamConnectionReader() :
        ConnectionReader(),
        writer(NULL),
        in(NULL),
        str(NULL),
        protocol(NULL),
        messageLen(0),
        textMode(false),
        bareMode(false),
        valid(false),
        err(false),
        shouldDrop(false),
        writePending(false),
        ref(NULL),
        convertedTextMode(false),
        pushedIntFlag(false) {
    }

    virtual ~StreamConnectionReader();

    void reset(yarp::os::InputStream& in, TwoWayStream *str, const Route& route,
               size_t len, bool textMode, bool bareMode = false) {
        this->in = &in;
        this->str = str;
        this->route = route;
        this->messageLen = len;
        this->textMode = textMode;
        this->bareMode = bareMode;
        this->valid = true;
        ref = NULL;
        err = false;
        convertedTextMode = false;
        pushedIntFlag = false;
    }

    virtual bool setSize(size_t len) {
        reset(*in,str,route,len,textMode,bareMode);
        return true;
    }

    void setProtocol(Protocol *protocol) {
        this->protocol = protocol;
    }

    virtual bool expectBlock(const yarp::os::Bytes& b) {
        if (!isGood()) {
            return false;
        }
        yAssert(in!=NULL);
        size_t len = b.length();
        if (len==0) return true;
        //if (len<0) len = messageLen;
        if (len>0) {
            YARP_SSIZE_T rlen = in->readFull(b);
            if (rlen>=0) {
                messageLen -= len;
                return true;
            }
        }
        err = true;
        return false;
    }

    virtual bool pushInt(int x) {
        if (pushedIntFlag) return false;
        pushedIntFlag = true;
        pushedInt = x;
        return true;
    }

    virtual int expectInt() {
        if (pushedIntFlag) {
            pushedIntFlag = false;
            return pushedInt;
        }
        if (!isGood()) { return 0; }
        NetInt32 x = 0;
        yarp::os::Bytes b((char*)(&x),sizeof(x));
        yAssert(in!=NULL);
        YARP_SSIZE_T r = in->read(b);
        if (r<0 || (size_t)r<b.length()) {
            err = true;
            return 0;
        }
        messageLen -= b.length();
        return x;
    }

    virtual double expectDouble() {
        if (!isGood()) { return 0; }
        NetFloat64 x = 0;
        yarp::os::Bytes b((char*)(&x),sizeof(x));
        yAssert(in!=NULL);
        YARP_SSIZE_T r = in->read(b);
        if (r<0 || (size_t)r<b.length()) {
            err = true;
            return 0;
        }
        messageLen -= b.length();
        return x;
    }

    virtual String expectString(int len) {
        if (!isGood()) { return ""; }
        char *buf = new char[len];
        yarp::os::Bytes b(buf,len);
        yAssert(in!=NULL);
        YARP_SSIZE_T r = in->read(b);
        if (r<0 || (size_t)r<b.length()) {
            err = true;
            delete[] buf;
            return "";
        }
        messageLen -= b.length();
        String s = buf;
        delete[] buf;
        return s;
    }

    virtual String expectLine() {
        if (!isGood()) { return ""; }
        yAssert(in!=NULL);
        bool success = false;
        String result = in->readLine('\n',&success);
        if (!success) {
            err = true;
            return "";
        }
        messageLen -= result.length()+1;
        return result;
    }

    virtual bool isTextMode() {
        return textMode;
    }

    virtual bool isBareMode() {
        return bareMode;
    }

    virtual bool convertTextMode();

    virtual size_t getSize() {
        return messageLen + (pushedIntFlag?sizeof(yarp::os::NetInt32):0);
    }

    /*
      virtual OutputStream *getReplyStream() {
      if (str==NULL) {
      return NULL;
      }
      return &(str->getOutputStream());
      }
    */

    virtual yarp::os::ConnectionWriter *getWriter();

    void suppressReply() {
        str = NULL;
    }

    virtual void flushWriter();

    //virtual TwoWayStream *getStreams() {
    //return str;
    //}

    virtual yarp::os::Contact getRemoteContact() {
        if (str!=NULL) {
            Contact remote = str->getRemoteAddress();
            return remote.addName(route.getFromName());
        }
        return yarp::os::Contact::byCarrier(route.getCarrierName()).addName(route.getFromName());
    }

    virtual yarp::os::Contact getLocalContact() {
        if (str!=NULL) {
            Contact local = str->getLocalAddress();
            return local.addName(route.getToName());
        }
        return yarp::os::Contact::invalid();
    }



    virtual bool expectBlock(const char *data, size_t len) {
        return expectBlock(yarp::os::Bytes((char*)data,len));
    }

    virtual ::yarp::os::ConstString expectText(int terminatingChar) {
        if (!isGood()) { return ""; }
        yAssert(in!=NULL);
        bool lsuccess = false;
        String result = in->readLine(terminatingChar,&lsuccess);
        if (lsuccess) {
            messageLen -= result.length()+1;
        }
        return ::yarp::os::ConstString(result.c_str());
    }

    virtual bool isValid() {
        return valid;
    }

    virtual bool isError() {
        if (err) return true;
        return !isActive();
    }

    virtual bool isActive() {
        if (shouldDrop) return false;
        if (!isValid()) return false;
        if (in!=NULL) {
            if (in->isOk()) {
                return true;
            }
        }
        return false;
    }

    virtual yarp::os::Portable *getReference() {
        return ref;
    }

    virtual void setReference(yarp::os::Portable *obj) {
        ref = obj;
    }

    virtual yarp::os::Bytes readEnvelope();

    virtual void requestDrop() {
        shouldDrop = true;
    }

    bool dropRequested() {
        return shouldDrop;
    }

    virtual yarp::os::Searchable& getConnectionModifiers();

private:

    bool isGood() {
        return isActive()&&isValid()&&!isError();
    }

    BufferedConnectionWriter *writer;
    StringInputStream altStream;
    yarp::os::InputStream *in;
    TwoWayStream *str;
    Protocol *protocol;
    size_t messageLen;
    bool textMode;
    bool bareMode;
    bool valid;
    bool err;
    bool shouldDrop;
    bool writePending;
    Route route;
    yarp::os::Portable *ref;
    yarp::os::Bottle config;
    bool convertedTextMode;
    bool pushedIntFlag;
    int pushedInt;
};

#endif

// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_STREAMBLOCKREADER_
#define _YARP2_STREAMBLOCKREADER_

#include <yarp/os/impl/InputStream.h>
#include <yarp/os/impl/TwoWayStream.h>
#include <yarp/os/impl/StringInputStream.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/impl/IOException.h>
#include <yarp/os/impl/NetType.h>
#include <yarp/os/Bytes.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/Route.h>
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
    StreamConnectionReader() {
        in = NULL;
        str = NULL;
        messageLen = 0;
        textMode = false;
        writer = NULL;
        writePending = false;
        valid = false;
        ref = NULL;
        err = false;
        protocol = NULL;
        shouldDrop = false;
        convertedTextMode = false;
        pushedIntFlag = false;
    }

    virtual ~StreamConnectionReader();

    void reset(InputStream& in, TwoWayStream *str, const Route& route,
               size_t len, bool textMode) {
        this->in = &in;
        this->str = str;
        this->route = route;
        this->messageLen = len;
        this->textMode = textMode;
        this->valid = true;
        ref = NULL;
        err = false;
        convertedTextMode = false;
        pushedIntFlag = false;
    }

    void setProtocol(Protocol *protocol) {
        this->protocol = protocol;
    }

    virtual bool expectBlock(const yarp::os::Bytes& b) {
        if (!isGood()) {
            //throw IOException("read from invalid stream");
            return false;
        }
        YARP_ASSERT(in!=NULL);
        size_t len = b.length();
        if (len==0) return true;
        //if (len<0) len = messageLen;
        if (len>0) {
            ssize_t rlen = NetType::readFull(*in,b);
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
        //if (!isValid()) { throw IOException("read from invalid stream"); }
        if (!isGood()) { return 0; }
        NetType::NetInt32 x = 0;
        yarp::os::Bytes b((char*)(&x),sizeof(x));
        YARP_ASSERT(in!=NULL);
        ssize_t r = in->read(b);
        if (r<0 || (size_t)r<b.length()) {
            err = true;
            return 0;
        }
        messageLen -= b.length();
        return x;
    }

    virtual double expectDouble() {
        //if (!isValid()) { throw IOException("read from invalid stream"); }
        if (!isGood()) { return 0; }
        NetType::NetFloat64 x = 0;
        yarp::os::Bytes b((char*)(&x),sizeof(x));
        YARP_ASSERT(in!=NULL);
        ssize_t r = in->read(b);
        if (r<0 || (size_t)r<b.length()) {
            err = true;
            return 0;
        }
        messageLen -= b.length();
        return x;
    }

    virtual String expectString(int len) {
        //if (!isValid()) { throw IOException("read from invalid stream"); }
        if (!isGood()) { return ""; }
        char *buf = new char[len];
        yarp::os::Bytes b(buf,len);
        YARP_ASSERT(in!=NULL);
        ssize_t r = in->read(b);
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
        //if (!isValid()) { throw IOException("read from invalid stream"); }
        if (!isGood()) { return ""; }
        YARP_ASSERT(in!=NULL);
        bool success = false;
        String result = NetType::readLine(*in,'\n',&success);
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


    virtual bool convertTextMode();

    virtual size_t getSize() {
        return messageLen;
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
            Address remote = str->getRemoteAddress();
            return remote.addRegName(route.getFromName()).toContact();
        }
        return yarp::os::Contact::byCarrier(route.getCarrierName().c_str()).addName(route.getFromName().c_str());
    }

    virtual yarp::os::Contact getLocalContact() {
        if (str!=NULL) {
            Address local = str->getLocalAddress();
            return local.addRegName(route.getToName()).toContact();
        }
        return yarp::os::Contact::invalid();
    }



    virtual bool expectBlock(const char *data, size_t len) {
        return expectBlock(yarp::os::Bytes((char*)data,len));
    }

    virtual ::yarp::os::ConstString expectText(int terminatingChar) {
        //if (!isValid()) { throw IOException("read from invalid stream"); }
        if (!isGood()) { return ""; }
        YARP_ASSERT(in!=NULL);
        bool lsuccess = false;
        String result = NetType::readLine(*in,terminatingChar,&lsuccess);
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
    InputStream *in;
    TwoWayStream *str;
    Protocol *protocol;
    size_t messageLen;
    bool textMode;
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

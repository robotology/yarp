// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_STREAMBLOCKREADER_
#define _YARP2_STREAMBLOCKREADER_

#include <yarp/InputStream.h>
#include <yarp/TwoWayStream.h>
#include <yarp/StringInputStream.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/IOException.h>
#include <yarp/NetType.h>
#include <yarp/Bytes.h>
#include <yarp/Logger.h>
#include <yarp/Route.h>
#include <yarp/os/Contact.h>

namespace yarp {
    class StreamConnectionReader;
    class BufferedConnectionWriter;
}


/**
 * Lets Readable objects read from the underlying InputStream
 * associated with the connection between two ports.
 */
class yarp::StreamConnectionReader : public ConnectionReader {
public:
    StreamConnectionReader() {
        in = NULL;
        str = NULL;
        messageLen = 0;
        textMode = false;
        writer = NULL;
        valid = false;
        ref = NULL;
        err = false;
    }

    virtual ~StreamConnectionReader();

    void reset(InputStream& in, TwoWayStream *str, const Route& route,
               int len, bool textMode) {
        this->in = &in;
        this->str = str;    
        this->route = route;
        this->messageLen = len;
        this->textMode = textMode;
        this->valid = true;
        ref = NULL;
        err = false;
    }

    virtual bool expectBlock(const Bytes& b) {
        if (!isGood()) { 
            //throw IOException("read from invalid stream"); 
            return false;
        }
        YARP_ASSERT(in!=NULL);
        int len = b.length();
        if (len==0) return true;
        if (len<0) { len = messageLen; }
        if (len>0) {
            int rlen = NetType::readFull(*in,b);
            if (rlen>=0) {
                messageLen -= len;
                return true;
            }
        }
        err = true;
        return false;
    }


    virtual int expectInt() {
        //if (!isValid()) { throw IOException("read from invalid stream"); }
        if (!isGood()) { return 0; }
        NetType::NetInt32 x = 0;
        Bytes b((char*)(&x),sizeof(x));
        YARP_ASSERT(in!=NULL);
        int r = in->read(b);
        if (r<b.length()) {
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
        Bytes b((char*)(&x),sizeof(x));
        YARP_ASSERT(in!=NULL);
        int r = in->read(b);
        if (r<b.length()) {
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
        Bytes b(buf,len);
        YARP_ASSERT(in!=NULL);
        int r = in->read(b);
        if (r<b.length()) {
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

    virtual int getSize() {
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
        return yarp::os::Contact::invalid();
    }

    virtual yarp::os::Contact getLocalContact() {
        if (str!=NULL) {
            Address local = str->getLocalAddress();
            return local.addRegName(route.getToName()).toContact();
        }
        return yarp::os::Contact::invalid();
    }



    virtual bool expectBlock(const char *data, int len) {
        return expectBlock(Bytes((char*)data,len));
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
        return err;
    }

    virtual bool isActive() {
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

private:

    bool isGood() {
        return isActive()&&isValid()&&!isError();
    }

    BufferedConnectionWriter *writer;
    StringInputStream altStream;
    InputStream *in;
    TwoWayStream *str;
    int messageLen;
    bool textMode;
    bool valid;
    bool err;
    Route route;
    yarp::os::Portable *ref;
};

#endif

// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
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
    }

    virtual void expectBlock(const Bytes& b) {
        if (!isValid()) { throw IOException("read from invalid stream"); }
        YARP_ASSERT(in!=NULL);
        int len = b.length();
        if (len==0) return;
        if (len<0) { len = messageLen; }
        if (len>0) {
            NetType::readFull(*in,b);
            messageLen -= len;
            return;
        }
        throw IOException("expectBlock size conditions failed");
    }


    virtual int expectInt() {
        if (!isValid()) { throw IOException("read from invalid stream"); }
        NetType::NetInt32 x = 0;
        Bytes b((char*)(&x),sizeof(x));
        YARP_ASSERT(in!=NULL);
        in->read(b);
        messageLen -= b.length();
        return x;
    }

    virtual double expectDouble() {
        if (!isValid()) { throw IOException("read from invalid stream"); }
        NetType::NetFloat64 x = 0;
        Bytes b((char*)(&x),sizeof(x));
        YARP_ASSERT(in!=NULL);
        in->read(b);
        messageLen -= b.length();
        return x;
    }

    virtual String expectString(int len) {
        if (!isValid()) { throw IOException("read from invalid stream"); }
        char *buf = new char[len];
        Bytes b(buf,len);
        YARP_ASSERT(in!=NULL);
        in->read(b);
        messageLen -= b.length();
        String s = buf;
        delete[] buf;
        return s;
    }

    virtual String expectLine() {
        if (!isValid()) { throw IOException("read from invalid stream"); }
        YARP_ASSERT(in!=NULL);
        String result = NetType::readLine(*in);
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



    virtual void expectBlock(const char *data, int len) {
        expectBlock(Bytes((char*)data,len));
    }

    virtual ::yarp::os::ConstString expectText(int terminatingChar) {
        if (!isValid()) { throw IOException("read from invalid stream"); }
        YARP_ASSERT(in!=NULL);
        String result = NetType::readLine(*in,terminatingChar);
        messageLen -= result.length()+1;
        return ::yarp::os::ConstString(result.c_str());
    }

    virtual bool isValid() {
        return valid;
    }

private:
    BufferedConnectionWriter *writer;
    StringInputStream altStream;
    InputStream *in;
    TwoWayStream *str;
    int messageLen;
    bool textMode;
    bool valid;
    Route route;
};

#endif

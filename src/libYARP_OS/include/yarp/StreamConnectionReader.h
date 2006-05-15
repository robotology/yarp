// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef _YARP2_STREAMBLOCKREADER_
#define _YARP2_STREAMBLOCKREADER_

#include <yarp/InputStream.h>
#include <yarp/TwoWayStream.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/IOException.h>
#include <yarp/NetType.h>
#include <yarp/Bytes.h>
#include <yarp/Logger.h>
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
    }

    virtual ~StreamConnectionReader();

    void reset(InputStream& in, TwoWayStream *str, 
               int len, bool textMode) {
        this->in = &in;
        this->str = str;    
        this->messageLen = len;
        this->textMode = textMode;
    }

    virtual void expectBlock(const Bytes& b) {
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
        NetType::NetInt32 x = 0;
        Bytes b((char*)(&x),sizeof(x));
        YARP_ASSERT(in!=NULL);
        in->read(b);
        messageLen -= b.length();
        return x;
    }

    virtual String expectString(int len) {
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
        YARP_ASSERT(in!=NULL);
        String result = NetType::readLine(*in);
        messageLen -= result.length()+1;
        return result;
    }

    virtual bool isTextMode() {
        return textMode;
    }

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
            return remote.toContact();
        }
        return yarp::os::Contact::invalid();
    }

    virtual yarp::os::Contact getLocalContact() {
        YARP_ERROR(Logger::get(), 
                   "StreamConnectionReader getLocalContact not implemented");
        return yarp::os::Contact::invalid();
    }



    virtual void expectBlock(const char *data, int len) {
        expectBlock(Bytes((char*)data,len));
    }

    virtual ::yarp::os::ConstString expectText(int terminatingChar) {
        YARP_ASSERT(in!=NULL);
        String result = NetType::readLine(*in,terminatingChar);
        messageLen -= result.length()+1;
        return ::yarp::os::ConstString(result.c_str());
    }


private:
    BufferedConnectionWriter *writer;
    InputStream *in;
    TwoWayStream *str;
    int messageLen;
    bool textMode;
};

#endif

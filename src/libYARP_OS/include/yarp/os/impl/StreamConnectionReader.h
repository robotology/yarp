/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_STREAMCONNECTIONREADER_H
#define YARP_OS_IMPL_STREAMCONNECTIONREADER_H

#include <yarp/os/InputStream.h>
#include <yarp/os/TwoWayStream.h>
#include <yarp/os/StringInputStream.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/NetType.h>
#include <yarp/os/Bytes.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/Route.h>
#include <yarp/os/Contact.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/NetInt64.h>

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
class YARP_OS_impl_API yarp::os::impl::StreamConnectionReader : public ConnectionReader
{
public:
    StreamConnectionReader() :
        ConnectionReader(),
        writer(nullptr),
        in(nullptr),
        str(nullptr),
        protocol(nullptr),
        messageLen(0),
        textMode(false),
        bareMode(false),
        valid(false),
        err(false),
        shouldDrop(false),
        writePending(false),
        ref(nullptr),
        convertedTextMode(false),
        pushedIntFlag(false),
        pushedInt(-1),
        parentConnectionReader(nullptr)
    {
    }

    virtual ~StreamConnectionReader();

    void reset(yarp::os::InputStream& in, TwoWayStream *str, const Route& route,
               size_t len, bool textMode, bool bareMode = false)
    {
        this->in = &in;
        this->str = str;
        this->route = route;
        this->messageLen = len;
        this->textMode = textMode;
        this->bareMode = bareMode;
        this->valid = true;
        ref = nullptr;
        err = false;
        convertedTextMode = false;
        pushedIntFlag = false;
    }

    virtual bool setSize(size_t len) override
    {
        reset(*in, str, route, len, textMode, bareMode);
        return true;
    }

    void setProtocol(Protocol *protocol)
    {
        this->protocol = protocol;
    }

    virtual bool expectBlock(const yarp::os::Bytes& b)
    {
        if (!isGood()) {
            return false;
        }
        yAssert(in!=nullptr);
        size_t len = b.length();
        if (len==0) {
            return true;
        }
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

    virtual bool pushInt(int x) override
    {
        if (pushedIntFlag) {
            return false;
        }
        pushedIntFlag = true;
        pushedInt = x;
        return true;
    }

    virtual int expectInt() override
    {
        if (pushedIntFlag) {
            pushedIntFlag = false;
            return pushedInt;
        }
        if (!isGood()) {
            return 0;
        }
        NetInt32 x = 0;
        yarp::os::Bytes b((char*)(&x), sizeof(x));
        yAssert(in!=nullptr);
        YARP_SSIZE_T r = in->read(b);
        if (r<0 || (size_t)r<b.length()) {
            err = true;
            return 0;
        }
        messageLen -= b.length();
        return x;
    }

    virtual YARP_INT64 expectInt64() override
    {
        if (!isGood()) {
            return 0;
        }
        NetInt64 x = 0;
        yarp::os::Bytes b((char*)(&x), sizeof(x));
        yAssert(in!=nullptr);
        YARP_SSIZE_T r = in->read(b);
        if (r<0 || (size_t)r<b.length()) {
            err = true;
            return 0;
        }
        messageLen -= b.length();
        return x;
    }

    virtual double expectDouble() override
    {
        if (!isGood()) {
            return 0;
        }
        NetFloat64 x = 0;
        yarp::os::Bytes b((char*)(&x), sizeof(x));
        yAssert(in!=nullptr);
        YARP_SSIZE_T r = in->read(b);
        if (r<0 || (size_t)r<b.length()) {
            err = true;
            return 0;
        }
        messageLen -= b.length();
        return x;
    }

    virtual ConstString expectString(int len)
    {
        if (!isGood()) {
            return "";
        }
        char *buf = new char[len];
        yarp::os::Bytes b(buf, len);
        yAssert(in!=nullptr);
        YARP_SSIZE_T r = in->read(b);
        if (r<0 || (size_t)r<b.length()) {
            err = true;
            delete[] buf;
            return "";
        }
        messageLen -= b.length();
        ConstString s = buf;
        delete[] buf;
        return s;
    }

    virtual ConstString expectLine()
    {
        if (!isGood()) {
            return "";
        }
        yAssert(in!=nullptr);
        bool success = false;
        ConstString result = in->readLine('\n', &success);
        if (!success) {
            err = true;
            return "";
        }
        messageLen -= result.length()+1;
        return result;
    }

    virtual bool isTextMode() override
    {
        return textMode;
    }

    virtual bool isBareMode() override
    {
        return bareMode;
    }

    virtual bool convertTextMode() override;

    virtual size_t getSize() override
    {
        return messageLen + (pushedIntFlag?sizeof(yarp::os::NetInt32):0);
    }

    /*
    virtual OutputStream *getReplyStream() override
    {
        if (str==nullptr) {
            return nullptr;
        }
        return &(str->getOutputStream());
    }
    */

    virtual yarp::os::ConnectionWriter *getWriter() override;

    void suppressReply()
    {
        str = nullptr;
    }

    virtual void flushWriter();

    // virtual TwoWayStream *getStreams() override
    // {
    // return str;
    // }

    virtual yarp::os::Contact getRemoteContact() override
    {
        if (str!=nullptr) {
            Contact remote = str->getRemoteAddress();
            remote.setName(route.getFromName());
            return remote;
        }
        Contact remote = yarp::os::Contact(route.getFromName(), route.getCarrierName());
        return remote;
    }

    virtual yarp::os::Contact getLocalContact() override
    {
        if (str!=nullptr) {
            Contact local = str->getLocalAddress();
            local.setName(route.getToName());
            return local;
        }
        return yarp::os::Contact();
    }



    virtual bool expectBlock(const char *data, size_t len) override
    {
        return expectBlock(yarp::os::Bytes((char*)data, len));
    }

    virtual ::yarp::os::ConstString expectText(int terminatingChar) override
    {
        if (!isGood()) {
            return "";
        }
        yAssert(in!=nullptr);
        bool lsuccess = false;
        ConstString result = in->readLine(terminatingChar, &lsuccess);
        if (lsuccess) {
            messageLen -= result.length()+1;
        }
        return ::yarp::os::ConstString(result.c_str());
    }

    virtual bool isValid() override
    {
        return valid;
    }

    virtual bool isError() override
    {
        if (err) {
            return true;
        }
        return !isActive();
    }

    virtual bool isActive() override
    {
        if (shouldDrop) {
            return false;
        }
        if (!isValid()) {
            return false;
        }
        if (in!=nullptr) {
            if (in->isOk()) {
                return true;
            }
        }
        return false;
    }

    virtual yarp::os::Portable *getReference() override
    {
        return ref;
    }

    virtual void setReference(yarp::os::Portable *obj)
    {
        ref = obj;
    }

    virtual yarp::os::Bytes readEnvelope() override;

    virtual void requestDrop() override
    {
        shouldDrop = true;
    }

    bool dropRequested()
    {
        return shouldDrop;
    }

    virtual yarp::os::Searchable& getConnectionModifiers() override;

    virtual void setParentConnectionReader(ConnectionReader *parentConnectionReader) override
    {
        this->parentConnectionReader = parentConnectionReader;
    }

private:

    bool isGood()
    {
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
    ConnectionReader *parentConnectionReader;
};

#endif // YARP_OS_IMPL_STREAMCONNECTIONREADER_H

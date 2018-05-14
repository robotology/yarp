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
    StreamConnectionReader();
    virtual ~StreamConnectionReader();

    void reset(yarp::os::InputStream& in, TwoWayStream *str, const Route& route,
               size_t len, bool textMode, bool bareMode = false);
    void setProtocol(Protocol *protocol);
    void suppressReply();
    bool dropRequested();

    virtual bool expectBlock(const yarp::os::Bytes& b);
    virtual std::string expectString(int len);
    virtual std::string expectLine();
    virtual void flushWriter();
    virtual void setReference(yarp::os::Portable *obj);

    /**** OVERRIDES ****/
    virtual bool setSize(size_t len) override;
    virtual size_t getSize() override;
    virtual bool pushInt(int x) override;
    virtual int expectInt() override;
    virtual YARP_INT64 expectInt64() override;
    virtual double expectDouble() override;
    virtual bool expectBlock(const char *data, size_t len) override;
    virtual std::string expectText(int terminatingChar) override;
    virtual bool isTextMode() override;
    virtual bool isBareMode() override;
    virtual bool convertTextMode() override;
    virtual yarp::os::ConnectionWriter *getWriter() override;
    virtual yarp::os::Contact getRemoteContact() override;
    virtual yarp::os::Contact getLocalContact() override;
    virtual bool isValid() override;
    virtual bool isError() override;
    virtual bool isActive() override;
    virtual yarp::os::Portable *getReference() override;
    virtual yarp::os::Bytes readEnvelope() override;
    virtual void requestDrop() override;
    virtual yarp::os::Searchable& getConnectionModifiers() override;
    virtual void setParentConnectionReader(ConnectionReader *parentConnectionReader) override;

private:

    bool isGood();

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

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_STREAMCONNECTIONREADER_H
#define YARP_OS_IMPL_STREAMCONNECTIONREADER_H

#include <yarp/os/Bottle.h>
#include <yarp/os/Bytes.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/Contact.h>
#include <yarp/os/InputStream.h>
#include <yarp/os/NetFloat32.h>
#include <yarp/os/NetFloat64.h>
#include <yarp/os/NetInt16.h>
#include <yarp/os/NetInt32.h>
#include <yarp/os/NetInt64.h>
#include <yarp/os/NetInt8.h>
#include <yarp/os/NetType.h>
#include <yarp/os/Route.h>
#include <yarp/os/StringInputStream.h>
#include <yarp/os/TwoWayStream.h>

namespace yarp {
namespace os {
namespace impl {

class BufferedConnectionWriter;
class Protocol;

/**
 * Lets Readable objects read from the underlying InputStream
 * associated with the connection between two ports.
 */
class YARP_os_impl_API StreamConnectionReader :
        public ConnectionReader
{
public:
    StreamConnectionReader();
    virtual ~StreamConnectionReader();

    void reset(yarp::os::InputStream& in, TwoWayStream* str, const Route& route, size_t len, bool textMode, bool bareMode = false);
    void setProtocol(Protocol* protocol);
    void suppressReply();
    bool dropRequested();

    virtual bool expectBlock(yarp::os::Bytes& b);
#ifndef YARP_NO_DEPRECATED // Since YARP 3.2
    using ConnectionReader::expectString;
    virtual std::string expectString(int len);
#endif
    virtual std::string expectLine();
    virtual void flushWriter();
    virtual void setReference(yarp::os::Portable* obj);

    /**** OVERRIDES ****/
    bool setSize(size_t len) override;
    size_t getSize() const override;
    bool pushInt(int x) override;
    std::int8_t expectInt8() override;
    std::int16_t expectInt16() override;
    std::int32_t expectInt32() override;
    std::int64_t expectInt64() override;
    yarp::conf::float32_t expectFloat32() override;
    yarp::conf::float64_t expectFloat64() override;
    bool expectBlock(char* data, size_t len) override;
    std::string expectText(const char terminatingChar) override;
    bool isTextMode() const override;
    bool isBareMode() const override;
    bool convertTextMode() override;
    yarp::os::ConnectionWriter* getWriter() override;
    yarp::os::Contact getRemoteContact() const override;
    yarp::os::Contact getLocalContact() const override;
    bool isValid() const override;
    bool isError() const override;
    bool isActive() const override;
    yarp::os::Portable* getReference() const override;
    yarp::os::Bytes readEnvelope() override;
    void requestDrop() override;
    const yarp::os::Searchable& getConnectionModifiers() const override;
    void setParentConnectionReader(ConnectionReader* parentConnectionReader) override;

private:
    bool isGood();

    template <typename T, typename NetT>
    T expectType();

    BufferedConnectionWriter* writer;
    StringInputStream altStream;
    yarp::os::InputStream* in;
    TwoWayStream* str;
    Protocol* protocol;
    size_t messageLen;
    bool textMode;
    bool bareMode;
    bool valid;
    bool err;
    bool shouldDrop;
    bool writePending;
    Route route;
    yarp::os::Portable* ref;
    yarp::os::Bottle config;
    bool convertedTextMode;
    bool pushedIntFlag;
    int pushedInt;
    ConnectionReader* parentConnectionReader;
};

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_STREAMCONNECTIONREADER_H

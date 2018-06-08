/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_CONNECTIONRECORDER_H
#define YARP_OS_IMPL_CONNECTIONRECORDER_H

#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/PortWriter.h>
#include <yarp/os/Bottle.h>

#include <yarp/os/impl/BufferedConnectionWriter.h>

namespace yarp {
namespace os {
namespace impl {


/**
 * A helper for recording entire message/reply transactions
 */
class ConnectionRecorder : public yarp::os::ConnectionReader,
                           public yarp::os::ConnectionWriter,
                           public yarp::os::PortWriter
{
private:
    yarp::os::ConnectionReader* reader;
    yarp::os::ConnectionWriter* writer;
    yarp::os::impl::BufferedConnectionWriter readerStore;
    yarp::os::impl::BufferedConnectionWriter writerStore;
    bool writing;
    bool wrote;
    bool skipNextInt;
    yarp::os::Bottle blank;

public:
    ConnectionRecorder();

    /**
     * Call this to wrap a specific ConnectionReader.
     */
    void init(yarp::os::ConnectionReader* wrappedReader);

    /**
     * Call this when all reading/writing has been done.
     */
    void fini();

    yarp::os::impl::BufferedConnectionWriter& getMessage();
    yarp::os::impl::BufferedConnectionWriter& getReply();
    bool hasReply();

    virtual bool expectBlock(const char* data, size_t len) override;
    virtual std::string expectText(int terminatingChar) override;
    virtual std::int8_t expectInt8() override;
    virtual std::int16_t expectInt16() override;
    virtual std::int32_t expectInt32() override;
    virtual std::int64_t expectInt64() override;
    virtual yarp::conf::float32_t expectFloat32() override;
    virtual yarp::conf::float64_t expectFloat64() override;
    virtual bool pushInt(int x) override;
    virtual bool isTextMode() override;
    virtual bool isBareMode() override;
    virtual bool convertTextMode() override;
    virtual size_t getSize() override;
    virtual yarp::os::ConnectionWriter* getWriter() override;
    virtual yarp::os::Portable* getReference() override;
    virtual yarp::os::Contact getRemoteContact() override;
    virtual yarp::os::Contact getLocalContact() override;
    virtual bool isValid() override;
    virtual bool isActive() override;
    virtual bool isError() override;
    virtual void appendBlock(const char* data, size_t len) override;
    virtual void appendInt8(std::int8_t data) override;
    virtual void appendInt16(std::int16_t data) override;
    virtual void appendInt32(std::int32_t data) override;
    virtual void appendInt64(std::int64_t data) override;
    virtual void appendFloat32(yarp::conf::float32_t data) override;
    virtual void appendFloat64(yarp::conf::float64_t data) override;
    virtual void appendString(const char* str, int terminate) override;
    virtual void appendExternalBlock(const char* data, size_t len) override;
    virtual void declareSizes(int argc, int* argv) override;
    virtual void setReplyHandler(yarp::os::PortReader& reader) override;
    virtual void setReference(yarp::os::Portable* obj) override;
    virtual bool write(yarp::os::ConnectionWriter& connection) override;
    virtual void requestDrop() override;
    virtual yarp::os::Searchable& getConnectionModifiers() override;
    virtual yarp::os::SizedWriter* getBuffer() override;
    virtual bool setSize(size_t len) override;
};

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_CONNECTIONRECORDER_H

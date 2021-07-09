/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_CONNECTIONRECORDER_H
#define YARP_OS_IMPL_CONNECTIONRECORDER_H

#include <yarp/os/Bottle.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/PortWriter.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>

namespace yarp {
namespace os {
namespace impl {


/**
 * A helper for recording entire message/reply transactions
 */
class ConnectionRecorder :
        public yarp::os::ConnectionReader,
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

    const yarp::os::impl::BufferedConnectionWriter& getMessage() const;
    const yarp::os::impl::BufferedConnectionWriter& getReply() const;
    bool hasReply() const;

    bool expectBlock(char* data, size_t len) override;
    std::string expectText(const char terminatingChar) override;
    std::int8_t expectInt8() override;
    std::int16_t expectInt16() override;
    std::int32_t expectInt32() override;
    std::int64_t expectInt64() override;
    yarp::conf::float32_t expectFloat32() override;
    yarp::conf::float64_t expectFloat64() override;
    bool pushInt(int x) override;
    bool isTextMode() const override;
    bool isBareMode() const override;
    bool convertTextMode() override;
    size_t getSize() const override;
    yarp::os::ConnectionWriter* getWriter() override;
    yarp::os::Portable* getReference() const override;
    yarp::os::Contact getRemoteContact() const override;
    yarp::os::Contact getLocalContact() const override;
    bool isValid() const override;
    bool isActive() const override;
    bool isError() const override;
    void appendBlock(const char* data, size_t len) override;
    void appendInt8(std::int8_t data) override;
    void appendInt16(std::int16_t data) override;
    void appendInt32(std::int32_t data) override;
    void appendInt64(std::int64_t data) override;
    void appendFloat32(yarp::conf::float32_t data) override;
    void appendFloat64(yarp::conf::float64_t data) override;
    void appendText(const std::string& str, const char terminate) override;
    void appendExternalBlock(const char* data, size_t len) override;
    void declareSizes(int argc, int* argv) override;
    void setReplyHandler(yarp::os::PortReader& reader) override;
    void setReference(yarp::os::Portable* obj) override;
    bool write(yarp::os::ConnectionWriter& connection) const override;
    void requestDrop() override;
    const yarp::os::Searchable& getConnectionModifiers() const override;
    yarp::os::SizedWriter* getBuffer() const override;
    bool setSize(size_t len) override;
};

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_CONNECTIONRECORDER_H

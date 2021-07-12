/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_NULLCONNECTIONWRITER_H
#define YARP_OS_NULLCONNECTIONWRITER_H

#include <yarp/os/api.h>

#include <yarp/os/ConnectionWriter.h>

namespace yarp {
namespace os {

/**
 *
 * A dummy ConnectionWriter that consumes data without effect.
 *
 */
class YARP_os_API NullConnectionWriter : public ConnectionWriter
{
public:
    void appendBlock(const char* data, size_t len) override;
    void appendInt8(std::int8_t data) override;
    void appendInt16(std::int16_t data) override;
    void appendInt32(std::int32_t data) override;
    void appendInt64(std::int64_t data) override;
    void appendFloat32(yarp::conf::float32_t data) override;
    void appendFloat64(yarp::conf::float64_t data) override;
    void appendText(const std::string& str, const char terminate = '\n') override;
    void appendExternalBlock(const char* data, size_t len) override;
    bool isTextMode() const override;
    bool isBareMode() const override;
    void declareSizes(int argc, int* argv) override;
    void setReplyHandler(PortReader& reader) override;
    void setReference(Portable* obj) override;
    bool convertTextMode() override;
    bool isValid() const override;
    bool isActive() const override;
    bool isError() const override;
    void requestDrop() override;
    bool isNull() const override;
    SizedWriter* getBuffer() const override;
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_NULLCONNECTIONWRITER_H

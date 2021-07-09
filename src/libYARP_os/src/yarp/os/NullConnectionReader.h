/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_NULLCONNECTIONREADER_H
#define YARP_OS_NULLCONNECTIONREADER_H

#include <yarp/os/api.h>

#include <yarp/os/ConnectionReader.h>
#include <yarp/os/Value.h>

namespace yarp {
namespace os {

/**
 *
 * Dummy ConnectionReader that has no data.
 *
 */
class YARP_os_API NullConnectionReader : public ConnectionReader
{
private:
    Value blank;

public:
    bool expectBlock(char* data, size_t len) override;
    std::string expectText(const char terminatingChar = '\n') override;
    std::int8_t expectInt8() override;
    std::int16_t expectInt16() override;
    std::int32_t expectInt32() override;
    std::int64_t expectInt64() override;
    yarp::conf::float32_t expectFloat32() override;
    yarp::conf::float64_t expectFloat64() override;
    bool pushInt(int x) override;
    bool isTextMode() const override;
    bool convertTextMode() override;
    size_t getSize() const override;
    ConnectionWriter* getWriter() override;
    Bytes readEnvelope() override;
    Portable* getReference() const override;
    Contact getRemoteContact() const override;
    Contact getLocalContact() const override;
    bool isValid() const override;
    bool isActive() const override;
    bool isError() const override;
    void requestDrop() override;
    const Searchable& getConnectionModifiers() const override;
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_NULLCONNECTIONREADER_H

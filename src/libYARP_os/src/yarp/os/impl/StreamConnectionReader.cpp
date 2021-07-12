/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Bottle.h>
#include <yarp/os/Log.h>
#include <yarp/os/NetType.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/impl/Protocol.h>
#include <yarp/os/impl/StreamConnectionReader.h>

using namespace yarp::os::impl;
using namespace yarp::os;

StreamConnectionReader::StreamConnectionReader() :
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

StreamConnectionReader::~StreamConnectionReader()
{
    if (writer != nullptr) {
        delete writer;
        writer = nullptr;
    }
}

void StreamConnectionReader::reset(InputStream& in,
                                   TwoWayStream* str,
                                   const Route& route,
                                   size_t len,
                                   bool textMode,
                                   bool bareMode)
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

void StreamConnectionReader::setProtocol(Protocol* protocol)
{
    this->protocol = protocol;
}

void StreamConnectionReader::suppressReply()
{
    str = nullptr;
}

bool StreamConnectionReader::dropRequested()
{
    return shouldDrop;
}

bool StreamConnectionReader::expectBlock(Bytes& b)
{
    if (!isGood()) {
        return false;
    }
    yAssert(in != nullptr);
    size_t len = b.length();
    if (len == 0) {
        return true;
    }
    //if (len<0) len = messageLen;
    if (len > 0) {
        yarp::conf::ssize_t rlen = in->readFull(b);
        if (rlen >= 0) {
            messageLen -= len;
            return true;
        }
    }
    err = true;
    return false;
}

#ifndef YARP_NO_DEPRECATED // Since YARP 3.2
std::string StreamConnectionReader::expectString(int len)
{
    if (!isGood()) {
        return {};
    }
    char* buf = new char[len];
    yarp::os::Bytes b(buf, len);
    yAssert(in != nullptr);
    yarp::conf::ssize_t r = in->read(b);
    if (r < 0 || static_cast<size_t>(r) < b.length()) {
        err = true;
        delete[] buf;
        return {};
    }
    messageLen -= b.length();
    std::string s = buf;
    delete[] buf;
    return s;
}
#endif // YARP_NO_DEPRECATED

std::string StreamConnectionReader::expectLine()
{
    if (!isGood()) {
        return {};
    }
    yAssert(in != nullptr);
    bool success = false;
    std::string result = in->readLine('\n', &success);
    if (!success) {
        err = true;
        return {};
    }
    messageLen -= result.length() + 1;
    return result;
}

void StreamConnectionReader::flushWriter()
{
    if (writer != nullptr) {
        if (writePending) {
            if (str != nullptr) {
                if (protocol != nullptr) {
                    protocol->reply(*writer);
                } else {
                    writer->write(str->getOutputStream());
                }
                writer->clear();
            }
        }
    }
    writePending = false;
}

void StreamConnectionReader::setReference(yarp::os::Portable* obj)
{
    ref = obj;
}

bool StreamConnectionReader::setSize(size_t len)
{
    reset(*in, str, route, len, textMode, bareMode);
    return true;
}

size_t StreamConnectionReader::getSize() const
{
    return messageLen + (pushedIntFlag ? sizeof(yarp::os::NetInt32) : 0);
}

bool StreamConnectionReader::pushInt(int x)
{
    if (pushedIntFlag) {
        return false;
    }
    pushedIntFlag = true;
    pushedInt = x;
    return true;
}

template <typename T, typename NetT>
inline T StreamConnectionReader::expectType()
{
    yAssert(in != nullptr);

    NetT x = 0;
    yarp::os::Bytes b(reinterpret_cast<char*>(&x), sizeof(T));
    yarp::conf::ssize_t r = in->read(b);
    if (r < 0 || static_cast<size_t>(r) < b.length()) {
        err = true;
        return 0;
    }
    messageLen -= b.length();

    return static_cast<T>(x);
}

std::int8_t StreamConnectionReader::expectInt8()
{
    if (!isGood()) {
        return 0;
    }
    return expectType<std::int8_t, NetInt8>();
}

std::int16_t StreamConnectionReader::expectInt16()
{
    if (!isGood()) {
        return 0;
    }
    return expectType<std::int16_t, NetInt16>();
}

std::int32_t StreamConnectionReader::expectInt32()
{
    if (pushedIntFlag) {
        pushedIntFlag = false;
        return pushedInt;
    }
    if (!isGood()) {
        return 0;
    }
    return expectType<std::int32_t, NetInt32>();
}

std::int64_t StreamConnectionReader::expectInt64()
{
    if (!isGood()) {
        return 0;
    }
    return expectType<std::int64_t, NetInt64>();
}

yarp::conf::float32_t StreamConnectionReader::expectFloat32()
{
    if (!isGood()) {
        return 0;
    }
    return expectType<yarp::conf::float32_t, NetFloat32>();
}

yarp::conf::float64_t StreamConnectionReader::expectFloat64()
{
    if (!isGood()) {
        return 0;
    }
    return expectType<yarp::conf::float64_t, NetFloat64>();
}

bool StreamConnectionReader::expectBlock(char* data, size_t len)
{
    yarp::os::Bytes bytes(data, len);
    return expectBlock(bytes);
}

std::string StreamConnectionReader::expectText(const char terminatingChar)
{
    if (!isGood()) {
        return {};
    }
    yAssert(in != nullptr);
    bool lsuccess = false;
    std::string result = in->readLine(terminatingChar, &lsuccess);
    if (lsuccess) {
        messageLen -= result.length() + 1;
    }
    return result;
}

bool StreamConnectionReader::isTextMode() const
{
    return textMode;
}

bool StreamConnectionReader::isBareMode() const
{
    return bareMode;
}

bool StreamConnectionReader::convertTextMode()
{
    if (isTextMode()) {
        if (!convertedTextMode) {
            Bottle bot;
            bot.read(*this);
            BufferedConnectionWriter writer;
            bot.write(writer);
            std::string s = writer.toString();
            altStream.reset(s);
            in = &altStream;
            convertedTextMode = true;
        }
    }

    return true;
}

yarp::os::ConnectionWriter* StreamConnectionReader::getWriter()
{
    if (str == nullptr) {
        return nullptr;
    }
    if (writer == nullptr) {
        writer = new BufferedConnectionWriter(isTextMode(), isBareMode());
        yAssert(writer != nullptr);
    }
    writer->clear();
    writePending = true;
    if (protocol != nullptr) {
        protocol->willReply();
    }
    return writer;
}

yarp::os::Contact StreamConnectionReader::getRemoteContact() const
{
    if (str != nullptr) {
        Contact remote = str->getRemoteAddress();
        remote.setName(route.getFromName());
        return remote;
    }
    Contact remote = yarp::os::Contact(route.getFromName(), route.getCarrierName());
    return remote;
}

yarp::os::Contact StreamConnectionReader::getLocalContact() const
{
    if (str != nullptr) {
        Contact local = str->getLocalAddress();
        local.setName(route.getToName());
        return local;
    }
    return yarp::os::Contact();
}

bool StreamConnectionReader::isValid() const
{
    return valid;
}

bool StreamConnectionReader::isError() const
{
    if (err) {
        return true;
    }
    return !isActive();
}

bool StreamConnectionReader::isActive() const
{
    if (shouldDrop) {
        return false;
    }
    if (!isValid()) {
        return false;
    }
    if (in != nullptr) {
        if (in->isOk()) {
            return true;
        }
    }
    return false;
}

yarp::os::Portable* StreamConnectionReader::getReference() const
{
    return ref;
}

Bytes StreamConnectionReader::readEnvelope()
{
    if (protocol != nullptr) {
        const std::string& env = protocol->getEnvelope();
        return {const_cast<char*>(env.c_str()), env.length()};
    }
    if (parentConnectionReader != nullptr) {
        return parentConnectionReader->readEnvelope();
    }
    return {nullptr, 0};
}

void StreamConnectionReader::requestDrop()
{
    shouldDrop = true;
}

const Searchable& StreamConnectionReader::getConnectionModifiers() const
{
    if (config.size() == 0) {
        if (protocol != nullptr) {
            const_cast<Bottle&>(config).fromString(protocol->getSenderSpecifier());
        }
    }
    return config;
}

void StreamConnectionReader::setParentConnectionReader(ConnectionReader* parentConnectionReader)
{
    this->parentConnectionReader = parentConnectionReader;
}

bool StreamConnectionReader::isGood()
{
    return isActive() && isValid() && !isError();
}

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/ConnectionRecorder.h>

#include <yarp/os/Vocab.h>

yarp::os::impl::ConnectionRecorder::ConnectionRecorder() :
        reader(nullptr),
        writer(nullptr),
        writing(false),
        wrote(false),
        skipNextInt(false)
{
}

void yarp::os::impl::ConnectionRecorder::init(ConnectionReader* wrappedReader)
{
    reader = wrappedReader;
    if (reader->isTextMode()) {
        reader->convertTextMode();
    }
    writing = false;
}

void yarp::os::impl::ConnectionRecorder::fini()
{
    if (writing) {
        if (writer->isTextMode()) {
            writer->convertTextMode();
        }
        writing = false;
    }
}

const yarp::os::impl::BufferedConnectionWriter& yarp::os::impl::ConnectionRecorder::getMessage() const
{
    return readerStore;
}

const yarp::os::impl::BufferedConnectionWriter& yarp::os::impl::ConnectionRecorder::getReply() const
{
    return writerStore;
}

bool yarp::os::impl::ConnectionRecorder::hasReply() const
{
    return wrote;
}

bool yarp::os::impl::ConnectionRecorder::expectBlock(char* data, size_t len)
{
    bool ok = reader->expectBlock(data, len);
    if (ok) {
        readerStore.appendBlock(data, len);
    }
    return ok;
}

std::string yarp::os::impl::ConnectionRecorder::expectText(const char terminatingChar)
{
    std::string str = reader->expectText(terminatingChar);
    readerStore.appendText(str, terminatingChar);
    return str;
}

std::int8_t yarp::os::impl::ConnectionRecorder::expectInt8()
{
    std::int8_t x = reader->expectInt8();
    readerStore.appendInt8(x);
    return x;
}

std::int16_t yarp::os::impl::ConnectionRecorder::expectInt16()
{
    std::int16_t x = reader->expectInt16();
    readerStore.appendInt16(x);
    return x;
}

std::int32_t yarp::os::impl::ConnectionRecorder::expectInt32()
{
    std::int32_t x = reader->expectInt32();
    if (!skipNextInt) {
        readerStore.appendInt32(x);
    } else {
        skipNextInt = false;
    }
    return x;
}

std::int64_t yarp::os::impl::ConnectionRecorder::expectInt64()
{
    std::int64_t x = reader->expectInt64();
    readerStore.appendInt64(x);
    return x;
}

yarp::conf::float32_t yarp::os::impl::ConnectionRecorder::expectFloat32()
{
    yarp::conf::float32_t x = reader->expectFloat32();
    readerStore.appendFloat32(x);
    return x;
}

yarp::conf::float64_t yarp::os::impl::ConnectionRecorder::expectFloat64()
{
    yarp::conf::float64_t x = reader->expectFloat64();
    readerStore.appendFloat64(x);
    return x;
}

bool yarp::os::impl::ConnectionRecorder::pushInt(int x)
{
    bool ok = reader->pushInt(x);
    skipNextInt = skipNextInt || ok;
    return ok;
}

bool yarp::os::impl::ConnectionRecorder::isTextMode() const
{
    return false;
}

bool yarp::os::impl::ConnectionRecorder::isBareMode() const
{
    return false;
}

bool yarp::os::impl::ConnectionRecorder::convertTextMode()
{
    return false;
}

size_t yarp::os::impl::ConnectionRecorder::getSize() const
{
    return reader->getSize();
}

yarp::os::ConnectionWriter* yarp::os::impl::ConnectionRecorder::getWriter()
{
    writer = reader->getWriter();
    writing = true;
    wrote = true;
    return this;
}

yarp::os::Portable* yarp::os::impl::ConnectionRecorder::getReference() const
{
    return reader->getReference();
}

yarp::os::Contact yarp::os::impl::ConnectionRecorder::getRemoteContact() const
{
    return reader->getRemoteContact();
}

yarp::os::Contact yarp::os::impl::ConnectionRecorder::getLocalContact() const
{
    return reader->getLocalContact();
}

bool yarp::os::impl::ConnectionRecorder::isValid() const
{
    // shared
    if (writing) {
        return writer->isValid();
    }
    return reader->isValid();
}

bool yarp::os::impl::ConnectionRecorder::isActive() const
{
    // shared
    if (writing) {
        return writer->isActive();
    }
    return reader->isActive();
}

bool yarp::os::impl::ConnectionRecorder::isError() const
{
    // shared
    if (writing) {
        return writer->isError();
    }
    return reader->isError();
}

void yarp::os::impl::ConnectionRecorder::appendBlock(const char* data, size_t len)
{
    writer->appendBlock(data, len);
    writerStore.appendBlock(data, len);
}

void yarp::os::impl::ConnectionRecorder::appendInt8(std::int8_t data)
{
    writer->appendInt8(data);
    writerStore.appendInt8(data);
}

void yarp::os::impl::ConnectionRecorder::appendInt16(std::int16_t data)
{
    writer->appendInt16(data);
    writerStore.appendInt16(data);
}

void yarp::os::impl::ConnectionRecorder::appendInt32(std::int32_t data)
{
    writer->appendInt32(data);
    writerStore.appendInt32(data);
}

void yarp::os::impl::ConnectionRecorder::appendInt64(std::int64_t data)
{
    writer->appendInt64(data);
    writerStore.appendInt64(data);
}

void yarp::os::impl::ConnectionRecorder::appendFloat32(yarp::conf::float32_t data)
{
    writer->appendFloat32(data);
    writerStore.appendFloat32(data);
}

void yarp::os::impl::ConnectionRecorder::appendFloat64(yarp::conf::float64_t data)
{
    writer->appendFloat64(data);
    writerStore.appendFloat64(data);
}

void yarp::os::impl::ConnectionRecorder::appendText(const std::string& str, const char terminate)
{
    writer->appendText(str, terminate);
    writerStore.appendText(str, terminate);
}

void yarp::os::impl::ConnectionRecorder::appendExternalBlock(const char* data, size_t len)
{
    writer->appendExternalBlock(data, len);
    writerStore.appendExternalBlock(data, len);
}

void yarp::os::impl::ConnectionRecorder::declareSizes(int argc, int* argv)
{
    writer->declareSizes(argc, argv);
}

void yarp::os::impl::ConnectionRecorder::setReplyHandler(yarp::os::PortReader& reader)
{
    writer->setReplyHandler(reader);
}

void yarp::os::impl::ConnectionRecorder::setReference(yarp::os::Portable* obj)
{
    writer->setReference(obj);
}

bool yarp::os::impl::ConnectionRecorder::write(yarp::os::ConnectionWriter& connection) const
{
    if (hasReply()) {
        connection.appendInt32(BOTTLE_TAG_LIST); // nested structure
        connection.appendInt32(3); // with three elements
        connection.appendInt32(BOTTLE_TAG_VOCAB32);
        connection.appendInt32(yarp::os::createVocab32('r', 'p', 'c'));
        bool ok = readerStore.write(connection);
        if (ok) {
            writerStore.write(connection);
        }
        return ok;
    }
    return readerStore.write(connection);
}

void yarp::os::impl::ConnectionRecorder::requestDrop()
{
}

const yarp::os::Searchable& yarp::os::impl::ConnectionRecorder::getConnectionModifiers() const
{
    return blank;
}


yarp::os::SizedWriter* yarp::os::impl::ConnectionRecorder::getBuffer() const
{
    return nullptr;
}

bool yarp::os::impl::ConnectionRecorder::setSize(size_t len)
{
    return reader->setSize(len);
}

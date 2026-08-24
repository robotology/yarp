/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/ShiftStream.h>

#include <mutex>

using yarp::os::Contact;
using yarp::os::InputStream;
using yarp::os::OutputStream;
using yarp::os::ShiftStream;
using yarp::os::TwoWayStream;

class ShiftStream::Private
{
public:
    Private();
    ~Private();
    void close();

    std::mutex streamMutex;
    TwoWayStream* stream{nullptr};
    NullStream nullStream;
};

ShiftStream::Private::Private() = default;

ShiftStream::Private::~Private()
{
    close();
}

void ShiftStream::Private::close()
{
    std::lock_guard<std::mutex> lg(streamMutex);
    if (stream != nullptr) {
        stream->close();
        delete stream;
        stream = nullptr;
    }
}


ShiftStream::ShiftStream() :
        mPriv(new Private())
{
}

ShiftStream::~ShiftStream()
{
    delete mPriv;
}

void ShiftStream::check() const
{
}

InputStream& ShiftStream::getInputStream()
{
    check();
    std::lock_guard<std::mutex> lg(mPriv->streamMutex);
    if (mPriv->stream == nullptr) {
        return mPriv->nullStream;
    }
    return mPriv->stream->getInputStream();
}

OutputStream& ShiftStream::getOutputStream()
{
    check();
    std::lock_guard<std::mutex> lg(mPriv->streamMutex);
    if (mPriv->stream == nullptr) {
        return mPriv->nullStream;
    }
    return mPriv->stream->getOutputStream();
}

const Contact& ShiftStream::getLocalAddress() const
{
    check();
    std::lock_guard<std::mutex> lg(mPriv->streamMutex);
    return (mPriv->stream == nullptr) ? mPriv->nullStream.getLocalAddress()
                                      : (mPriv->stream->getLocalAddress());
}

const Contact& ShiftStream::getRemoteAddress() const
{
    check();
    std::lock_guard<std::mutex> lg(mPriv->streamMutex);
    return (mPriv->stream == nullptr) ? mPriv->nullStream.getRemoteAddress()
                                      : (mPriv->stream->getRemoteAddress());
}

void ShiftStream::close()
{
    mPriv->close();
}

void ShiftStream::takeStream(TwoWayStream* stream)
{
    close();
    std::lock_guard<std::mutex> lg(mPriv->streamMutex);
    mPriv->stream = stream;
}

TwoWayStream* ShiftStream::giveStream()
{
    std::lock_guard<std::mutex> lg(mPriv->streamMutex);
    TwoWayStream* result = mPriv->stream;
    mPriv->stream = nullptr;
    return result;
}

TwoWayStream* ShiftStream::getStream() const
{
    std::lock_guard<std::mutex> lg(mPriv->streamMutex);
    return mPriv->stream;
}

bool ShiftStream::isEmpty() const
{
    std::lock_guard<std::mutex> lg(mPriv->streamMutex);
    return mPriv->stream == nullptr;
}

bool ShiftStream::isOk() const
{
    std::lock_guard<std::mutex> lg(mPriv->streamMutex);
    if (mPriv->stream != nullptr) {
        return mPriv->stream->isOk();
    }
    return false;
}

void ShiftStream::reset()
{
    std::lock_guard<std::mutex> lg(mPriv->streamMutex);
    if (mPriv->stream != nullptr) {
        mPriv->stream->reset();
    }
}

void ShiftStream::beginPacket()
{
    if (mPriv->stream != nullptr) {
        mPriv->stream->beginPacket();
    }
}

void ShiftStream::endPacket()
{
    if (mPriv->stream != nullptr) {
        mPriv->stream->endPacket();
    }
}

void ShiftStream::interruptInputStream()
{
    check();
    std::lock_guard<std::mutex> lg(mPriv->streamMutex);
    if (mPriv->stream != nullptr) {
        mPriv->stream->getInputStream().interrupt();
    }
}

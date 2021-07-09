/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/ShiftStream.h>

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
    if (mPriv->stream == nullptr) {
        return mPriv->nullStream;
    }
    return mPriv->stream->getInputStream();
}

OutputStream& ShiftStream::getOutputStream()
{
    check();
    if (mPriv->stream == nullptr) {
        return mPriv->nullStream;
    }
    return mPriv->stream->getOutputStream();
}

const Contact& ShiftStream::getLocalAddress() const
{
    check();
    return (mPriv->stream == nullptr) ? mPriv->nullStream.getLocalAddress()
                                      : (mPriv->stream->getLocalAddress());
}

const Contact& ShiftStream::getRemoteAddress() const
{
    check();
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
    mPriv->stream = stream;
}

TwoWayStream* ShiftStream::giveStream()
{
    TwoWayStream* result = mPriv->stream;
    mPriv->stream = nullptr;
    return result;
}

TwoWayStream* ShiftStream::getStream() const
{
    return mPriv->stream;
}

bool ShiftStream::isEmpty() const
{
    return mPriv->stream == nullptr;
}

bool ShiftStream::isOk() const
{
    if (mPriv->stream != nullptr) {
        return mPriv->stream->isOk();
    }
    return false;
}

void ShiftStream::reset()
{
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

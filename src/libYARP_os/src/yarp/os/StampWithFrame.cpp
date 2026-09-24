/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/StampWithFrame.h>

#include <yarp/os/StampWithFrameData.h>
#include <yarp/os/impl/LogComponent.h>
#include <yarp/os/Time.h>

#include <algorithm>
#include <utility>


using yarp::os::StampWithFrame;
using yarp::os::ConnectionReader;
using yarp::os::ConnectionWriter;

namespace {
YARP_OS_LOG_COMPONENT(STAMPWITHFRAME, "yarp.os.StampWithFrame");
}

class StampWithFrame::Private : public yarp::os::StampWithFrameData
{
public:
    Private(StampWithFrame::count_t _count,
            yarp::conf::float64_t _time,
            const std::string& _frameId)
    {
        sequenceNumber = _count;
        timeStamp = _time;
        frameId = _frameId;
    }

    Private() = default;

    void clear()
    {
        sequenceNumber = StampWithFrame::npos;
        timeStamp = 0.0;
        frameId.clear();
    }

    void update(yarp::conf::float64_t time)
    {
        sequenceNumber++;
        if (sequenceNumber == StampWithFrame::npos) {
            // npos is not used to store a valid StampWithFrame, just restart the
            // counter
            sequenceNumber = 0;
        }
        timeStamp = time;
    }
};


StampWithFrame::StampWithFrame(StampWithFrame::count_t count, yarp::conf::float64_t time, std::string frameId) :
        mPriv(new Private(count, time, frameId))
{
    yCAssert(STAMPWITHFRAME, mPriv->frameId.find(' ') == std::string::npos);
}

StampWithFrame::StampWithFrame() :
        mPriv(new Private())
{
}

StampWithFrame::StampWithFrame(const StampWithFrame& rhs) :
        mPriv(new Private(*(rhs.mPriv)))
{
}

StampWithFrame::StampWithFrame(StampWithFrame&& rhs) noexcept :
        mPriv(std::exchange(rhs.mPriv, nullptr))
{
}

StampWithFrame::~StampWithFrame()
{
    delete mPriv;
}

StampWithFrame& StampWithFrame::operator=(const StampWithFrame& rhs)
{
    if (&rhs != this) {
        *mPriv = *(rhs.mPriv);
    }
    return *this;
}

StampWithFrame& StampWithFrame::operator=(StampWithFrame&& rhs) noexcept
{
    if (&rhs != this) {
        std::swap(mPriv, rhs.mPriv);
    }
    return *this;
}

StampWithFrame::count_t StampWithFrame::count() const
{
    return mPriv->sequenceNumber;
}

yarp::conf::float64_t StampWithFrame::timeStamp() const
{
    return mPriv->timeStamp;
}

std::string StampWithFrame::frameId() const
{
    return mPriv->frameId;
}

bool StampWithFrame::isValid() const
{
    return mPriv->sequenceNumber != npos;
}

void StampWithFrame::update()
{
    mPriv->update(Time::now());;
}

void StampWithFrame::update(yarp::conf::float64_t time)
{
    mPriv->update(time);
}

void StampWithFrame::setFrameId(std::string frameId)
{
    mPriv->frameId = std::move(frameId);
}

bool StampWithFrame::read(ConnectionReader& connection)
{
    return mPriv->read(connection);
}

bool StampWithFrame::write(ConnectionWriter& connection) const
{
    return mPriv->write(connection);
}

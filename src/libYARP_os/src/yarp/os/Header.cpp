/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Header.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/impl/LogComponent.h>
#include <yarp/os/Time.h>

#include <algorithm>


using yarp::os::Header;
using yarp::os::ConnectionReader;
using yarp::os::ConnectionWriter;

namespace {
YARP_OS_LOG_COMPONENT(HEADER, "yarp.os.Header");
}

class Header::Private
{
public:
    Private(Header::count_t count,
            yarp::conf::float64_t time,
            std::string&& frameId) :
            sequenceNumber(count),
            timeStamp(time),
            frameId(std::move(frameId))
    {
    }

    Private() = default;

    void clear()
    {
        sequenceNumber = Header::npos;
        timeStamp = 0.0;
        frameId.clear();
    }

    void update(yarp::conf::float64_t time)
    {
        sequenceNumber++;
        if (sequenceNumber == Header::npos) {
            // npos is not used to store a valid header, just restart the
            // counter
            sequenceNumber = 0;
        }
        timeStamp = time;
    }

    Header::count_t sequenceNumber = Header::npos;
    yarp::conf::float64_t timeStamp = 0.0;
    std::string frameId;
};


Header::Header(Header::count_t count, yarp::conf::float64_t time, std::string frameId) :
        mPriv(new Private(count, time, std::move(frameId)))
{
    yCAssert(HEADER, mPriv->frameId.find(' ') == std::string::npos);
}

Header::Header() :
        mPriv(new Private())
{
}

Header::Header(const Header& rhs) :
        mPriv(new Private(*(rhs.mPriv)))
{
}

Header::Header(Header&& rhs) noexcept :
        mPriv(std::exchange(rhs.mPriv, nullptr))
{
}

Header::~Header()
{
    delete mPriv;
}

Header& Header::operator=(const Header& rhs)
{
    if (&rhs != this) {
        *mPriv = *(rhs.mPriv);
    }
    return *this;
}

Header& Header::operator=(Header&& rhs) noexcept
{
    if (&rhs != this) {
        std::swap(mPriv, rhs.mPriv);
    }
    return *this;
}

Header::count_t Header::count() const
{
    return mPriv->sequenceNumber;
}

yarp::conf::float64_t Header::timeStamp() const
{
    return mPriv->timeStamp;
}

std::string Header::frameId() const
{
    return mPriv->frameId;
}

bool Header::isValid() const
{
    return mPriv->sequenceNumber != npos;
}

bool Header::read(ConnectionReader& connection)
{
    if (connection.isTextMode()) {
        std::string stampStr = connection.expectText();
        Header::count_t seqNum;
        yarp::conf::float64_t ts;
        int used;

        int ret = std::sscanf(stampStr.c_str(), "%u %lg %n", &seqNum, &ts, &used);
        if (ret != 2 && used > 0) {
            mPriv->clear();
            return false;
        }
        mPriv->sequenceNumber = seqNum;
        mPriv->timeStamp = ts;
        mPriv->frameId = stampStr.substr((size_t)used, stampStr.find(' ', used));
    } else {
        connection.convertTextMode();

        // Read list length (must be 3 or 2 for compatibility with Stamp)
        std::int32_t header = connection.expectInt32();
        if (header != BOTTLE_TAG_LIST) {
            mPriv->clear();
            return false;
        }
        std::int32_t len = connection.expectInt32();
        if (len != 2 && len != 3) {
            mPriv->clear();
            return false;
        }

        // Read sequence number
        std::int32_t code = connection.expectInt32();
        if (code != BOTTLE_TAG_INT32) {
            mPriv->clear();
            return false;
        }
        mPriv->sequenceNumber = static_cast<Header::count_t>(connection.expectInt32());

        // Read timestamp
        code = connection.expectInt32();
        if (code != BOTTLE_TAG_FLOAT64) {
            mPriv->clear();
            return false;
        }
        mPriv->timeStamp = connection.expectFloat64();

        // Read frameId (unless receiving a Stamp)
        if (len == 3) {
            code = connection.expectInt32();
            if (code != BOTTLE_TAG_STRING) {
                mPriv->clear();
                return false;
            }
            mPriv->frameId = connection.expectString();
            yCAssert(HEADER, mPriv->frameId.find(' ') == std::string::npos);
        } else {
            mPriv->frameId.clear();
        }

        if (connection.isError()) {
            mPriv->clear();
            return false;
        }
    }
    return !connection.isError();
}


// In order to keep the Header class as much compatible as possible with
// Stamp, if the frameId is empty, it is not serialized at all.
bool Header::write(ConnectionWriter& connection) const
{
    if (connection.isTextMode()) {
        static constexpr size_t buf_size = 512;
        char buf[buf_size];
        if (!mPriv->frameId.empty()) {
            std::snprintf(buf, buf_size, "%d %.*g %s", mPriv->sequenceNumber, DBL_DIG, mPriv->timeStamp, mPriv->frameId.c_str());
        } else {
            std::snprintf(buf, buf_size, "%d %.*g", mPriv->sequenceNumber, DBL_DIG, mPriv->timeStamp);
        }
        connection.appendText(buf);
    } else {
        connection.appendInt32(BOTTLE_TAG_LIST); // nested structure
        connection.appendInt32(mPriv->frameId.empty() ? 2 : 3); // with two or three elements
        connection.appendInt32(BOTTLE_TAG_INT32);
        connection.appendInt32(static_cast<std::int32_t>(mPriv->sequenceNumber));
        connection.appendInt32(BOTTLE_TAG_FLOAT64);
        connection.appendFloat64(mPriv->timeStamp);
        if (!mPriv->frameId.empty()) {
            connection.appendInt32(BOTTLE_TAG_STRING);
            connection.appendString(mPriv->frameId);
        }
        connection.convertTextMode();
    }
    return !connection.isError();
}

void Header::update()
{
    mPriv->update(Time::now());;
}

void Header::update(yarp::conf::float64_t time)
{
    mPriv->update(time);
}

void Header::setFrameId(std::string frameId)
{
    mPriv->frameId = std::move(frameId);
}

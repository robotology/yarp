/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/WireLink.h>

#include <yarp/os/ContactStyle.h>
#include <yarp/os/DummyConnector.h>
#include <yarp/os/MessageStack.h>
#include <yarp/os/PortReader.h>
#include <yarp/os/PortWriter.h>
#include <yarp/os/UnbufferedContactable.h>

using yarp::os::WireLink;


class WireLink::Private
{
public:
    Private();

    yarp::os::UnbufferedContactable* port{nullptr};
    yarp::os::PortReader* reader{nullptr};
    yarp::os::PortReader* owner{nullptr};
    yarp::os::MessageStack stack;
    bool replies{true};
    bool can_write{false};
    bool can_read{false};

    bool attach(yarp::os::UnbufferedContactable& port,
                const yarp::os::ContactStyle& style);

    void reset();
};


WireLink::Private::Private() = default;

bool WireLink::Private::attach(yarp::os::UnbufferedContactable& port,
                               const yarp::os::ContactStyle& style)
{
    reset();
    this->port = &port;
    replies = style.expectReply;
    return true;
}

void WireLink::Private::reset()
{
    reader = nullptr;
    port = nullptr;
    replies = true;
    can_write = false;
    can_read = false;
}


WireLink::WireLink() :
        mPriv(new Private)
{
}

WireLink::~WireLink()
{
    delete mPriv;
}

bool WireLink::isValid() const
{
    return mPriv->port != nullptr || mPriv->reader != nullptr;
}

bool WireLink::setOwner(yarp::os::PortReader& owner)
{
    mPriv->owner = &owner;
    return true;
}

bool WireLink::attachAsClient(yarp::os::UnbufferedContactable& port)
{
    yarp::os::ContactStyle style;
    mPriv->attach(port, style);
    mPriv->can_write = true;
    mPriv->can_read = false;
    return true;
}

bool WireLink::attachAsClient(yarp::os::PortReader& reader)
{
    mPriv->reset();
    mPriv->reader = &reader;
    mPriv->can_write = true;
    mPriv->can_read = false;
    return true;
}

bool WireLink::attachAsServer(yarp::os::UnbufferedContactable& port)
{
    yarp::os::ContactStyle style;
    mPriv->attach(port, style);
    port.setReader(*mPriv->owner);
    mPriv->can_write = false;
    mPriv->can_read = true;
    return true;
}

bool WireLink::setStreamingMode(bool streaming)
{
    mPriv->replies = !streaming;
    return true;
}

bool WireLink::write(yarp::os::PortWriter& writer)
{
    if (mPriv->reader != nullptr) {
        DummyConnector con;
        writer.write(con.getWriter());
        return mPriv->reader->read(con.getReader());
    }
    if (!isValid()) {
        return false;
    }
    return mPriv->port->write(writer);
}

bool WireLink::write(const yarp::os::PortWriter& writer, yarp::os::PortReader& reader) const
{
    if (mPriv->reader != nullptr) {
        DummyConnector con;
        writer.write(con.getWriter());
        bool ok = mPriv->reader->read(con.getReader());
        reader.read(con.getReader());
        return ok;
    }
    if (!isValid()) {
        return false;
    }
    if (!mPriv->replies) {
        mPriv->port->write(writer);
        return false;
    }
    return mPriv->port->write(writer, reader);
}

bool WireLink::callback(yarp::os::PortWriter& writer, yarp::os::PortReader& reader, const std::string& tag)
{
    mPriv->stack.attach(reader);
    mPriv->stack.stack(writer, tag);
    return true;
}

bool WireLink::canWrite() const
{
    return mPriv->can_write;
}

bool WireLink::canRead() const
{
    return mPriv->can_read;
}

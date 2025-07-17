/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IFrameGrabberImage.h>
#include <yarp/dev/IFrameGrabberImage-inl.h>

#include <yarp/os/LogComponent.h>

YARP_LOG_COMPONENT(IFRAMEGRABBEROF, "yarp.dev.IFrameGrabberOf")

yarp::dev::IFrameGrabberImageBase::~IFrameGrabberImageBase() = default;

template class yarp::dev::IFrameGrabberOf<yarp::sig::ImageOf<yarp::sig::PixelRgb>>;
template class yarp::dev::IFrameGrabberOf<yarp::sig::ImageOf<yarp::sig::PixelMono>>;
template class yarp::dev::IFrameGrabberOf<yarp::sig::ImageOf<yarp::sig::PixelFloat>>;
template class yarp::dev::IFrameGrabberOf<yarp::sig::FlexImage>;

yarp::dev::vertex_t::~vertex_t() = default;

yarp::dev::vertex_t::vertex_t(int x_, int y_)
{
    x = x_;
    y = y_;
}

bool yarp::dev::vertex_t::read(yarp::os::ConnectionReader& connection)
{
    // auto-convert text mode interaction
    connection.convertTextMode();

    bool ok = true;
    connection.expectInt32();
    connection.expectInt32();

    connection.expectInt32();
    x = connection.expectInt32(); // 1
    connection.expectInt32();
    y = connection.expectInt32(); // 2

    return !connection.isError();
}

bool yarp::dev::vertex_t::write(yarp::os::ConnectionWriter& connection) const
{
    connection.appendInt32(BOTTLE_TAG_LIST);
    connection.appendInt32(2);
    connection.appendInt32(BOTTLE_TAG_INT32); // 1
    connection.appendInt32(x);
    connection.appendInt32(BOTTLE_TAG_INT32); // 2
    connection.appendInt32(y);
    connection.convertTextMode();
    return !connection.isError();
}

/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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

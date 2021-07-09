/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_AUDIOVISUALINTERFACES_H
#define YARP_DEV_AUDIOVISUALINTERFACES_H

#define YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_FRAMEGRABBERINTERFACES_H_ON_PURPOSE
#include <yarp/dev/FrameGrabberInterfaces.h>
#undef YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_FRAMEGRABBERINTERFACES_H_ON_PURPOSE

#include <yarp/dev/AudioGrabberInterfaces.h>

#include <yarp/dev/IAudioVisualGrabber.h>
#include <yarp/dev/IAudioVisualStream.h>
#include <yarp/dev/IFrameWriterAudioVisual.h>

#include <yarp/os/PortablePair.h>

namespace yarp {
namespace dev {

typedef yarp::os::PortablePair<yarp::sig::ImageOf<yarp::sig::PixelRgb>,
                               yarp::sig::Sound> ImageRgbSound;

} // namespace dev
} // namespace yarp

#endif // YARP_DEV_AUDIOVISUALINTERFACES_H

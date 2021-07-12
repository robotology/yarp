/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IFRAMEGRABBERRGB_H
#define YARP_DEV_IFRAMEGRABBERRGB_H

#include <yarp/conf/system.h>
#if !defined(YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_IFRAMEGRABBERRGB_H_ON_PURPOSE)
YARP_COMPILER_WARNING("<yarp/dev/IFrameGrabberRgb.h> file is deprecated")
#endif

#ifndef YARP_NO_DEPRECATED // Since YARP 3.5.0

#include <yarp/dev/api.h>

namespace yarp {
namespace dev {

/**
 * @ingroup dev_iface_media
 *
 * RGB Interface to a FrameGrabber device.
 *
 * @deprecated Since YARP 3.5
 */
class YARP_dev_DEPRECATED_API IFrameGrabberRgb
{
public:
    virtual ~IFrameGrabberRgb();
    /**
     * Get a rgb buffer from the frame grabber, if required
     * demosaicking/color reconstruction is applied
     *
     * @param buffer: pointer to the buffer to be filled (must be previously allocated)
     * @return true/false upon success/failure
     */
    virtual bool getRgbBuffer(unsigned char *buffer) = 0;

    /**
     * Return the height of each frame.
     * @return image height
     */
    virtual int height() const = 0;

    /**
     * Return the width of each frame.
     * @return image width
     */
    virtual int width() const = 0;
};

} // namespace dev
} // namespace yarp

#endif // YARP_NO_DEPRECATED

#endif // YARP_DEV_IFRAMEGRABBERRGB_H

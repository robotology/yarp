/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IFRAMEGRABBER_H
#define YARP_DEV_IFRAMEGRABBER_H

#include <yarp/conf/system.h>
#if !defined(YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_IFRAMEGRABBER_H_ON_PURPOSE)
YARP_COMPILER_WARNING("<yarp/dev/IFrameGrabber.h> file is deprecated")
#endif

#ifndef YARP_NO_DEPRECATED // Since YARP 3.5.0

#include <yarp/dev/api.h>

namespace yarp {
namespace dev {

/**
 * @ingroup dev_iface_media
 *
 * Common interface to a FrameGrabber.
 *
 * @deprecated Since YARP 3.5
 */
class YARP_dev_DEPRECATED_API IFrameGrabber
{
public:
    virtual ~IFrameGrabber();

    /**
     * Get the raw buffer from the frame grabber. The driver returns
     * a copy of the internal memory buffer acquired by the frame grabber, no
     * post processing is applied (e.g. no color reconstruction/demosaicking).
     * The user must allocate the buffer; the size of the buffer, in bytes,
     * is determined by calling getRawBufferSize().
     * @param buffer: pointer to the buffer to be filled (must be previously allocated)
     * @return true/false upon success/failure
     */
    virtual bool getRawBuffer(unsigned char *buffer)=0;

    /**
     * Get the size of the card's internal buffer, the user should use this
     * method to allocate the storage to contain a raw frame (getRawBuffer).
     * @return the size of the internal buffer, in bytes.
     **/
    virtual int getRawBufferSize()=0;

    /**
     * Return the height of each frame.
     * @return image height
     */
    virtual int height() const =0;

    /**
     * Return the width of each frame.
     * @return image width
     */
    virtual int width() const =0;
};

} // namespace dev
} // namespace yarp

#endif // YARP_NO_DEPRECATED

#endif // YARP_DEV_IFRAMEGRABBER_H

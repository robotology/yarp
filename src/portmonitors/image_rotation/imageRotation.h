/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_PORTMONITOR_IMAGEROTATION_H
#define YARP_PORTMONITOR_IMAGEROTATION_H

#include <yarp/os/Bottle.h>
#include <yarp/os/Things.h>
#include <yarp/os/MonitorObject.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/ImageUtils.h>

 /**
  * @ingroup portmonitors_lists
  * \brief `image_rotation`: Documentation to be added
  * example usage:
  * yarp connect /grabber/depth:o /yarpview/img:i tcp+recv.portmonitor+type.dll+file.image_rotation
  * yarp connect /grabber/depth:o /yarpview/img:i tcp+recv.portmonitor+type.dll+file.image_rotation+options_rotate.rotate_ccw
  */
class ImageRotation : public yarp::os::MonitorObject
{
public:
    bool create(const yarp::os::Property& options) override;
    void destroy() override;

    bool setparam(const yarp::os::Property& params) override;
    bool getparam(yarp::os::Property& params) override;

    bool accept(yarp::os::Things& thing) override;
    yarp::os::Things& update(yarp::os::Things& thing) override;

private:

    yarp::os::Bottle m_bt;
    yarp::os::Things m_th;
    yarp::sig::ImageOf<yarp::sig::PixelFloat> m_outImgFloat;
    yarp::sig::ImageOf<yarp::sig::PixelRgb> m_outImgRgb;

    yarp::sig::utils::RotateOption m_rotate_option {yarp::sig::utils::RotateOption::rotate_cw};
    yarp::sig::utils::FlipOption   m_flip_option   {yarp::sig::utils::FlipOption::flip_none};
};

#endif  // YARP_PORTMONITOR_IMAGEROTATION_H

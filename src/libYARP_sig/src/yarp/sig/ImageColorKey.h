/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SIG_IMAGE_COLORKEY_H
#define YARP_SIG_IMAGE_COLORKEY_H

#include <yarp/sig/Image.h>
#include <yarp/sig/ImageColorKeyData.h>

namespace yarp::sig {

class YARP_sig_API ImageColorKey : public yarp::sig::ImageColorKeyData
{
    public:
    //default constructor
    ImageColorKey() = default;

    void setValueAsPixelRgb(yarp::sig::PixelRgb v)
    {
        value = *reinterpret_cast<int*>(&v);
    }
    yarp::sig::PixelRgb getValueAsPixelRgb()
    {
        return *reinterpret_cast<yarp::sig::PixelRgb*>(&value);
    }
};
} // namespace yarp::sig

#endif // YARP_SIG_IMAGE_H

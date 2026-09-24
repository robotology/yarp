/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_TRANSLATION_H
#define YARP_TRANSLATION_H

#include <yarp/sig/TranslationData.h>
#include <yarp/math/api.h>

namespace yarp::math {
class Translation;
}

class YARP_math_API yarp::math::Translation : public yarp::sig::TranslationData
{
public:
    Translation() = default;
    Translation(double x, double y, double z)
    {
        t_x = x;
        t_y = y;
        t_z = z;
    }

    void set(double x, double y, double z);
};

#endif

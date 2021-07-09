/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_NAVTYPES_H
#define YARP_DEV_NAVTYPES_H

#include <yarp/math/Vec2D.h>

/**
* \file NavTypes.h contains definitions for common types used in navigation interfaces
*/
namespace yarp
{
    namespace dev
    {
        namespace Nav2D
        {
            typedef yarp::math::Vec2D<int>       XYCell_unbounded;
            typedef yarp::math::Vec2D<size_t>    XYCell;
            typedef yarp::math::Vec2D<double>    XYWorld;
        }
    }
}

#endif // YARP_DEV_NAVTYPES_H

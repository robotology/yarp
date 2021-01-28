/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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

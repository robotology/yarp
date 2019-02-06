/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/math/FrameTransform.h>

#include <cstdio>

std::string yarp::math::FrameTransform::toString() const
{
    char buff[1024];
    sprintf(buff, "%s -> %s \n tran: %f %f %f \n rot: %f %f %f %f \n\n",
                  src_frame_id.c_str(),
                  dst_frame_id.c_str(),
                  translation.tX,
                  translation.tY,
                  translation.tZ,
                  rotation.x(),
                  rotation.y(),
                  rotation.z(),
                  rotation.w());
    return std::string(buff);
}

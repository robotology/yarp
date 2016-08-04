/*
* Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
* Authors: Marco Randazzo, Andrea Ruzzenenti
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#include <yarp/math/FrameTransform.h>

#include <cstdio>

std::string yarp::math::FrameTransform::toString()
{
    char buff[1024];
    sprintf(buff, "%s -> %s \n tran: %f %f %f \n rot: %f %f %f %f \n\n",
                  src_frame_id.c_str(),
                  dst_frame_id.c_str(),
                  translation.tX,
                  translation.tY,
                  translation.tZ,
                  rotation.rX,
                  rotation.rY,
                  rotation.rZ,
                  rotation.rW);
    return std::string(buff);
}
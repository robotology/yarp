/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

struct YarpFrameTransform {
} (
  yarp.name = "yarp::math::FrameTransform"
  yarp.includefile="yarp/math/FrameTransform.h"
)

struct return_getAllTransforms
{
  1: bool retvalue;
  2: list<YarpFrameTransform> transforms_list;
}

service FrameTransformStorageSetRPC
{
  bool setTransforms(1:list<YarpFrameTransform> transforms) ;
  bool setTransform(1:YarpFrameTransform transform) ;
}

service FrameTransformStorageGetRPC
{
  return_getAllTransforms getTransforms() ;
}

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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

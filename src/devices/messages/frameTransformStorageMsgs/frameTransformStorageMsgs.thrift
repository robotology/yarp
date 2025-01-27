/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

struct YarpFrameTransform {
} (
  yarp.name = "yarp::math::FrameTransform"
  yarp.includefile="yarp/math/FrameTransform.h"
)

struct yReturnValue {
} (
  yarp.name = "yarp::dev::ReturnValue"
  yarp.includefile = "yarp/dev/ReturnValue.h"
)

struct return_getAllTransforms
{
  1: yReturnValue retvalue;
  2: list<YarpFrameTransform> transforms_list;
}

service FrameTransformStorageSetRPC
{
  yReturnValue setTransformsRPC(1:list<YarpFrameTransform> transforms) ;
  yReturnValue setTransformRPC(1:YarpFrameTransform transform);
  yReturnValue deleteTransformRPC(1:string src, 2:string dst);
  yReturnValue clearAllRPC()
}

service FrameTransformStorageGetRPC
{
  return_getAllTransforms getTransformsRPC() ;
}

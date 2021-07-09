/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.test

include "thrift/firstInterface/PointD.thrift"

typedef PointD.PointD Point3D

const i32 ANSWER = 42

service Demo {
  i32 get_answer();
  i32 add_one(1:i32 x = 0);
  i32 double_down(1: i32 x);
  PointD.PointD add_point(1: PointD.PointD x, 2: PointD.PointD y);
}

service DemoExtended extends Demo {
  Point3D multiply_point (1: Point3D x, 2:double factor)
}

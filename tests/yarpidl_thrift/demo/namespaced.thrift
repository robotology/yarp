/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

namespace yarp DemoWithNamespace


enum NSDemoEnum {
  ENUM1 = 1,
  ENUM2 = 2
}

struct NSDemoStruct {
  1: i32 x,
  2: i32 y
}

service NSDemo {
  i32 get_answer();
}

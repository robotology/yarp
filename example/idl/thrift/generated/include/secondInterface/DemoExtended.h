/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_DemoExtended
#define YARP_THRIFT_GENERATOR_DemoExtended

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <secondInterface/demo_common.h>
#include <secondInterface/Demo.h>

namespace yarp {
  namespace test {
    class DemoExtended;
  }
}


class yarp::test::DemoExtended :  public Demo {
public:
  DemoExtended();
  virtual Point3D multiply_point(const Point3D& x, const double factor);
  virtual bool read(yarp::os::ConnectionReader& connection) override;
  virtual std::vector<std::string> help(const std::string& functionName="--all");
};

#endif

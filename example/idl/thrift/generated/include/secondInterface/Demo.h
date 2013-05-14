// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_Demo
#define YARP_THRIFT_GENERATOR_Demo

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <secondInterface/demo_common.h>
#include <firstInterface/PointD.h>

namespace yarp {
  namespace test {
    class Demo;
  }
}


class yarp::test::Demo : public yarp::os::Wire {
public:
  Demo() { yarp().setOwner(*this); }
  virtual int32_t get_answer();
  virtual int32_t add_one(const int32_t x = 0);
  virtual int32_t double_down(const int32_t x);
  virtual  ::yarp::test::PointD add_point(const  ::yarp::test::PointD& x, const  ::yarp::test::PointD& y);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

#endif


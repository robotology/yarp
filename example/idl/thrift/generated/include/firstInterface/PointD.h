// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_STRUCT_PointD
#define YARP_THRIFT_GENERATOR_STRUCT_PointD

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>

namespace yarp {
  namespace test {
    class PointD;
  }
}


class yarp::test::PointD : public yarp::os::idl::WirePortable {
public:
  int32_t x;
  int32_t y;
  int32_t z;
  PointD() : x(0), y(0), z(0) {
  }
  PointD(const int32_t x,const int32_t y,const int32_t z) : x(x), y(y), z(z) {
  }
  bool read(yarp::os::idl::WireReader& reader);
  bool read(yarp::os::ConnectionReader& connection);
  bool write(yarp::os::idl::WireWriter& writer);
  bool write(yarp::os::ConnectionWriter& connection);
};

#endif


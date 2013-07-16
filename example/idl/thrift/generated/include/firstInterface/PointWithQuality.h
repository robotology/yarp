// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_STRUCT_PointWithQuality
#define YARP_THRIFT_GENERATOR_STRUCT_PointWithQuality

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <firstInterface/PointD.h>
#include <firstInterface/PointQuality.h>

namespace yarp {
  namespace test {
    class PointWithQuality;
  }
}


class yarp::test::PointWithQuality : public yarp::os::idl::WirePortable {
public:
  PointD point;
  PointQuality quality;
  PointWithQuality() : quality((PointQuality)0) {
    quality = (PointQuality)0;

  }
  PointWithQuality(const PointD& point,const PointQuality quality) : point(point), quality(quality) {
  }
  bool read(yarp::os::idl::WireReader& reader);
  bool read(yarp::os::ConnectionReader& connection);
  bool write(yarp::os::idl::WireWriter& writer);
  bool write(yarp::os::ConnectionWriter& connection);
};

#endif


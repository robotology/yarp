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
  bool read(yarp::os::idl::WireReader& reader) {
    if (!reader.read(point)) {
      reader.fail();
      return false;
    }
    int32_t ecast12;
    PointQualityVocab cvrt13;
    if (!reader.readEnum(ecast12,cvrt13)) {
      quality = UNKNOWN;
    } else {
      quality = (PointQuality)ecast12;
    }
    return !reader.isError();
  }
  bool read(yarp::os::ConnectionReader& connection) {
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListHeader(2)) return false;
    return read(reader);
  }
  bool write(yarp::os::idl::WireWriter& writer) {
    if (!writer.write(point)) return false;
    if (!writer.writeI32((int32_t)quality)) return false;
    return !writer.isError();
  }
  bool write(yarp::os::ConnectionWriter& connection) {
    yarp::os::idl::WireWriter writer(connection);
    if (!writer.writeListHeader(4)) return false;
    return write(writer);
  }
};

#endif


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
  bool read(yarp::os::idl::WireReader& reader) {
    if (!reader.readI32(x)) {
      reader.fail();
      return false;
    }
    if (!reader.readI32(y)) {
      reader.fail();
      return false;
    }
    if (!reader.readI32(z)) {
      reader.fail();
      return false;
    }
    return !reader.isError();
  }
  bool read(yarp::os::ConnectionReader& connection) {
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListHeader(3)) return false;
    return read(reader);
  }
  bool write(yarp::os::idl::WireWriter& writer) {
    if (!writer.writeI32(x)) return false;
    if (!writer.writeI32(y)) return false;
    if (!writer.writeI32(z)) return false;
    return !writer.isError();
  }
  bool write(yarp::os::ConnectionWriter& connection) {
    yarp::os::idl::WireWriter writer(connection);
    if (!writer.writeListHeader(3)) return false;
    return write(writer);
  }
};

#endif


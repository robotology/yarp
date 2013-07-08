// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <firstInterface/PointD.h>

namespace yarp { namespace test {
bool PointD::read(yarp::os::idl::WireReader& reader) {
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

bool PointD::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListHeader(3)) return false;
  return read(reader);
}

bool PointD::write(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeI32(x)) return false;
  if (!writer.writeI32(y)) return false;
  if (!writer.writeI32(z)) return false;
  return !writer.isError();
}

bool PointD::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(3)) return false;
  return write(writer);
}
}} // namespace

// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <firstInterface/PointWithQuality.h>

namespace yarp { namespace test {
bool PointWithQuality::read(yarp::os::idl::WireReader& reader) {
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

bool PointWithQuality::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListHeader(4)) return false;
  return read(reader);
}

bool PointWithQuality::write(yarp::os::idl::WireWriter& writer) {
  if (!writer.write(point)) return false;
  if (!writer.writeI32((int32_t)quality)) return false;
  return !writer.isError();
}

bool PointWithQuality::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(4)) return false;
  return write(writer);
}
}} // namespace

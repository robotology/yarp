// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <secondInterface/DemoExtended.h>
#include <yarp/os/idl/WireTypes.h>

namespace yarp { namespace test {


class DemoExtended_multiply_point : public yarp::os::Portable {
public:
  Point3D x;
  double factor;
  Point3D _return;
  virtual bool write(yarp::os::ConnectionWriter& connection) {
    yarp::os::idl::WireWriter writer(connection);
    if (!writer.writeListHeader(4)) return false;
    if (!writer.writeTag("multiply_point",1,2)) return false;
    if (!writer.write(x)) return false;
    if (!writer.writeDouble(factor)) return false;
    return true;
  }
  virtual bool read(yarp::os::ConnectionReader& connection) {
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListReturn()) return false;
    if (!reader.read(_return)) {
      reader.fail();
      return false;
    }
    return true;
  }
};

Point3D DemoExtended::multiply_point(const Point3D& x, const double factor) {
  Point3D _return;
  DemoExtended_multiply_point helper;
  helper.x = x;
  helper.factor = factor;
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}

bool DemoExtended::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  reader.expectAccept();
  if (!reader.readListHeader()) { reader.fail(); return false; }
  yarp::os::ConstString tag = reader.readTag();
  while (!reader.isError()) {
    // TODO: use quick lookup, this is just a test
    if (tag == "multiply_point") {
      Point3D x;
      double factor;
      if (!reader.read(x)) {
        reader.fail();
        return false;
      }
      if (!reader.readDouble(factor)) {
        reader.fail();
        return false;
      }
      Point3D _return;
      _return = multiply_point(x,factor);
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.write(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (reader.noMore()) { reader.fail(); return false; }
    yarp::os::ConstString next_tag = reader.readTag();
    if (next_tag=="") break;
    tag = tag + "_" + next_tag;
  }
  return false;
}
}} // namespace



// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <secondInterface/Demo.h>
#include <yarp/os/idl/WireTypes.h>

namespace yarp { namespace test {


class Demo_get_answer : public yarp::os::Portable {
public:
  int32_t _return;
  virtual bool write(yarp::os::ConnectionWriter& connection) {
    yarp::os::idl::WireWriter writer(connection);
    if (!writer.writeListHeader(2)) return false;
    if (!writer.writeTag("get_answer",1,2)) return false;
    return true;
  }
  virtual bool read(yarp::os::ConnectionReader& connection) {
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListReturn()) return false;
    if (!reader.readI32(_return)) {
      reader.fail();
      return false;
    }
    return true;
  }
};

class Demo_add_one : public yarp::os::Portable {
public:
  int32_t x;
  int32_t _return;
  virtual bool write(yarp::os::ConnectionWriter& connection) {
    yarp::os::idl::WireWriter writer(connection);
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeTag("add_one",1,2)) return false;
    if (!writer.writeI32(x)) return false;
    return true;
  }
  virtual bool read(yarp::os::ConnectionReader& connection) {
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListReturn()) return false;
    if (!reader.readI32(_return)) {
      reader.fail();
      return false;
    }
    return true;
  }
};

class Demo_double_down : public yarp::os::Portable {
public:
  int32_t x;
  int32_t _return;
  virtual bool write(yarp::os::ConnectionWriter& connection) {
    yarp::os::idl::WireWriter writer(connection);
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeTag("double_down",1,2)) return false;
    if (!writer.writeI32(x)) return false;
    return true;
  }
  virtual bool read(yarp::os::ConnectionReader& connection) {
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListReturn()) return false;
    if (!reader.readI32(_return)) {
      reader.fail();
      return false;
    }
    return true;
  }
};

class Demo_add_point : public yarp::os::Portable {
public:
   ::yarp::test::PointD x;
   ::yarp::test::PointD y;
   ::yarp::test::PointD _return;
  virtual bool write(yarp::os::ConnectionWriter& connection) {
    yarp::os::idl::WireWriter writer(connection);
    if (!writer.writeListHeader(8)) return false;
    if (!writer.writeTag("add_point",1,2)) return false;
    if (!writer.write(x)) return false;
    if (!writer.write(y)) return false;
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

int32_t Demo::get_answer() {
  int32_t _return = 0;
  Demo_get_answer helper;
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
int32_t Demo::add_one(const int32_t x) {
  int32_t _return = 0;
  Demo_add_one helper;
  helper.x = x;
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
int32_t Demo::double_down(const int32_t x) {
  int32_t _return = 0;
  Demo_double_down helper;
  helper.x = x;
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
 ::yarp::test::PointD Demo::add_point(const  ::yarp::test::PointD& x, const  ::yarp::test::PointD& y) {
   ::yarp::test::PointD _return;
  Demo_add_point helper;
  helper.x = x;
  helper.y = y;
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}

bool Demo::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  reader.expectAccept();
  if (!reader.readListHeader()) { reader.fail(); return false; }
  yarp::os::ConstString tag = reader.readTag();
  while (!reader.isError()) {
    // TODO: use quick lookup, this is just a test
    if (tag == "get_answer") {
      int32_t _return;
      _return = get_answer();
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeI32(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "add_one") {
      int32_t x;
      if (!reader.readI32(x)) {
        x = 0;
      }
      int32_t _return;
      _return = add_one(x);
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeI32(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "double_down") {
      int32_t x;
      if (!reader.readI32(x)) {
        reader.fail();
        return false;
      }
      int32_t _return;
      _return = double_down(x);
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeI32(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "add_point") {
       ::yarp::test::PointD x;
       ::yarp::test::PointD y;
      if (!reader.read(x)) {
        reader.fail();
        return false;
      }
      if (!reader.read(y)) {
        reader.fail();
        return false;
      }
       ::yarp::test::PointD _return;
      _return = add_point(x,y);
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(3)) return false;
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



// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <NSDemoStruct.h>

namespace DemoWithNamespace {
bool NSDemoStruct::read_x(yarp::os::idl::WireReader& reader) {
  if (!reader.readI32(x)) {
    reader.fail();
    return false;
  }
  return true;
}
bool NSDemoStruct::nested_read_x(yarp::os::idl::WireReader& reader) {
  if (!reader.readI32(x)) {
    reader.fail();
    return false;
  }
  return true;
}
bool NSDemoStruct::read_y(yarp::os::idl::WireReader& reader) {
  if (!reader.readI32(y)) {
    reader.fail();
    return false;
  }
  return true;
}
bool NSDemoStruct::nested_read_y(yarp::os::idl::WireReader& reader) {
  if (!reader.readI32(y)) {
    reader.fail();
    return false;
  }
  return true;
}
bool NSDemoStruct::read(yarp::os::idl::WireReader& reader) {
  if (!read_x(reader)) return false;
  if (!read_y(reader)) return false;
  return !reader.isError();
}

bool NSDemoStruct::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListHeader(2)) return false;
  return read(reader);
}

bool NSDemoStruct::write_x(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeI32(x)) return false;
  return true;
}
bool NSDemoStruct::nested_write_x(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeI32(x)) return false;
  return true;
}
bool NSDemoStruct::write_y(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeI32(y)) return false;
  return true;
}
bool NSDemoStruct::nested_write_y(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeI32(y)) return false;
  return true;
}
bool NSDemoStruct::write(yarp::os::idl::WireWriter& writer) {
  if (!write_x(writer)) return false;
  if (!write_y(writer)) return false;
  return !writer.isError();
}

bool NSDemoStruct::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(2)) return false;
  return write(writer);
}
bool NSDemoStruct::Editor::write(yarp::os::ConnectionWriter& connection) {
  if (!isValid()) return false;
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(dirty_count+1)) return false;
  if (!writer.writeString("patch")) return false;
  if (is_dirty_x) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("x")) return false;
    if (!obj->nested_write_x(writer)) return false;
  }
  if (is_dirty_y) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("y")) return false;
    if (!obj->nested_write_y(writer)) return false;
  }
  return !writer.isError();
}
bool NSDemoStruct::Editor::read(yarp::os::ConnectionReader& connection) {
  if (!isValid()) return false;
  yarp::os::idl::WireReader reader(connection);
  reader.expectAccept();
  if (!reader.readListHeader()) return false;
  int len = reader.getLength();
  if (len==0) {
    yarp::os::idl::WireWriter writer(reader);
    if (writer.isNull()) return true;
    if (!writer.writeListHeader(1)) return false;
    writer.writeString("send: 'help' or 'patch (param1 val1) (param2 val2)'");
    return true;
  }
  yarp::os::ConstString tag;
  if (!reader.readString(tag)) return false;
  if (tag=="help") {
    yarp::os::idl::WireWriter writer(reader);
    if (writer.isNull()) return true;
    if (!writer.writeListHeader(2)) return false;
    if (!writer.writeTag("many",1, 0)) return false;
    if (reader.getLength()>0) {
      yarp::os::ConstString field;
      if (!reader.readString(field)) return false;
      if (field=="x") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("int32_t x")) return false;
      }
      if (field=="y") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("int32_t y")) return false;
      }
    }
    if (!writer.writeListHeader(3)) return false;
    writer.writeString("*** Available fields:");
    writer.writeString("x");
    writer.writeString("y");
    return true;
  }
  bool nested = true;
  bool have_act = false;
  if (tag!="patch") {
    if ((len-1)%2 != 0) return false;
    len = 1 + ((len-1)/2);
    nested = false;
    have_act = true;
  }
  for (int i=1; i<len; i++) {
    if (nested && !reader.readListHeader(3)) return false;
    yarp::os::ConstString act;
    yarp::os::ConstString key;
    if (have_act) {
      act = tag;
    } else {
      if (!reader.readString(act)) return false;
    }
    if (!reader.readString(key)) return false;
    // inefficient code follows, bug paulfitz to improve it
    if (key == "x") {
      will_set_x();
      if (!obj->nested_read_x(reader)) return false;
      did_set_x();
    } else if (key == "y") {
      will_set_y();
      if (!obj->nested_read_y(reader)) return false;
      did_set_y();
    } else {
      // would be useful to have a fallback here
    }
  }
  reader.accept();
  yarp::os::idl::WireWriter writer(reader);
  if (writer.isNull()) return true;
  writer.writeListHeader(1);
  writer.writeVocab(VOCAB2('o','k'));
  return true;
}

yarp::os::ConstString NSDemoStruct::toString() {
  yarp::os::Bottle b;
  b.read(*this);
  return b.toString();
}
} // namespace

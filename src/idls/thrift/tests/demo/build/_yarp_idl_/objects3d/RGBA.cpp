// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <RGBA.h>

bool RGBA::read_rgba(yarp::os::idl::WireReader& reader) {
  if (!reader.readI32(rgba)) {
    reader.fail();
    return false;
  }
  return true;
}
bool RGBA::nested_read_rgba(yarp::os::idl::WireReader& reader) {
  if (!reader.readI32(rgba)) {
    reader.fail();
    return false;
  }
  return true;
}
bool RGBA::read(yarp::os::idl::WireReader& reader) {
  if (!read_rgba(reader)) return false;
  return !reader.isError();
}

bool RGBA::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListHeader(1)) return false;
  return read(reader);
}

bool RGBA::write_rgba(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeI32(rgba)) return false;
  return true;
}
bool RGBA::nested_write_rgba(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeI32(rgba)) return false;
  return true;
}
bool RGBA::write(yarp::os::idl::WireWriter& writer) {
  if (!write_rgba(writer)) return false;
  return !writer.isError();
}

bool RGBA::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(1)) return false;
  return write(writer);
}
bool RGBA::Editor::write(yarp::os::ConnectionWriter& connection) {
  if (!isValid()) return false;
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(dirty_count+1)) return false;
  if (!writer.writeString("patch")) return false;
  if (is_dirty_rgba) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("rgba")) return false;
    if (!obj->nested_write_rgba(writer)) return false;
  }
  return !writer.isError();
}
bool RGBA::Editor::read(yarp::os::ConnectionReader& connection) {
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
      if (field=="rgba") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("int32_t rgba")) return false;
      }
    }
    if (!writer.writeListHeader(2)) return false;
    writer.writeString("*** Available fields:");
    writer.writeString("rgba");
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
    if (key == "rgba") {
      will_set_rgba();
      if (!obj->nested_read_rgba(reader)) return false;
      did_set_rgba();
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

yarp::os::ConstString RGBA::toString() {
  yarp::os::Bottle b;
  b.read(*this);
  return b.toString();
}

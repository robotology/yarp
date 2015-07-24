// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <TestSomeMoreTypes.h>

bool TestSomeMoreTypes::read_a_bool(yarp::os::idl::WireReader& reader) {
  if (!reader.readBool(a_bool)) {
    reader.fail();
    return false;
  }
  return true;
}
bool TestSomeMoreTypes::nested_read_a_bool(yarp::os::idl::WireReader& reader) {
  if (!reader.readBool(a_bool)) {
    reader.fail();
    return false;
  }
  return true;
}
bool TestSomeMoreTypes::read_a_byte(yarp::os::idl::WireReader& reader) {
  if (!reader.readByte(a_byte)) {
    reader.fail();
    return false;
  }
  return true;
}
bool TestSomeMoreTypes::nested_read_a_byte(yarp::os::idl::WireReader& reader) {
  if (!reader.readByte(a_byte)) {
    reader.fail();
    return false;
  }
  return true;
}
bool TestSomeMoreTypes::read_a_i16(yarp::os::idl::WireReader& reader) {
  if (!reader.readI16(a_i16)) {
    reader.fail();
    return false;
  }
  return true;
}
bool TestSomeMoreTypes::nested_read_a_i16(yarp::os::idl::WireReader& reader) {
  if (!reader.readI16(a_i16)) {
    reader.fail();
    return false;
  }
  return true;
}
bool TestSomeMoreTypes::read_a_i32(yarp::os::idl::WireReader& reader) {
  if (!reader.readI32(a_i32)) {
    reader.fail();
    return false;
  }
  return true;
}
bool TestSomeMoreTypes::nested_read_a_i32(yarp::os::idl::WireReader& reader) {
  if (!reader.readI32(a_i32)) {
    reader.fail();
    return false;
  }
  return true;
}
bool TestSomeMoreTypes::read_a_i64(yarp::os::idl::WireReader& reader) {
  if (!reader.readI64(a_i64)) {
    reader.fail();
    return false;
  }
  return true;
}
bool TestSomeMoreTypes::nested_read_a_i64(yarp::os::idl::WireReader& reader) {
  if (!reader.readI64(a_i64)) {
    reader.fail();
    return false;
  }
  return true;
}
bool TestSomeMoreTypes::read_a_double(yarp::os::idl::WireReader& reader) {
  if (!reader.readDouble(a_double)) {
    reader.fail();
    return false;
  }
  return true;
}
bool TestSomeMoreTypes::nested_read_a_double(yarp::os::idl::WireReader& reader) {
  if (!reader.readDouble(a_double)) {
    reader.fail();
    return false;
  }
  return true;
}
bool TestSomeMoreTypes::read_a_string(yarp::os::idl::WireReader& reader) {
  if (!reader.readString(a_string)) {
    reader.fail();
    return false;
  }
  return true;
}
bool TestSomeMoreTypes::nested_read_a_string(yarp::os::idl::WireReader& reader) {
  if (!reader.readString(a_string)) {
    reader.fail();
    return false;
  }
  return true;
}
bool TestSomeMoreTypes::read_a_binary(yarp::os::idl::WireReader& reader) {
  if (!reader.readBinary(a_binary)) {
    reader.fail();
    return false;
  }
  return true;
}
bool TestSomeMoreTypes::nested_read_a_binary(yarp::os::idl::WireReader& reader) {
  if (!reader.readBinary(a_binary)) {
    reader.fail();
    return false;
  }
  return true;
}
bool TestSomeMoreTypes::read(yarp::os::idl::WireReader& reader) {
  if (!read_a_bool(reader)) return false;
  if (!read_a_byte(reader)) return false;
  if (!read_a_i16(reader)) return false;
  if (!read_a_i32(reader)) return false;
  if (!read_a_i64(reader)) return false;
  if (!read_a_double(reader)) return false;
  if (!read_a_string(reader)) return false;
  if (!read_a_binary(reader)) return false;
  return !reader.isError();
}

bool TestSomeMoreTypes::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListHeader(8)) return false;
  return read(reader);
}

bool TestSomeMoreTypes::write_a_bool(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeBool(a_bool)) return false;
  return true;
}
bool TestSomeMoreTypes::nested_write_a_bool(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeBool(a_bool)) return false;
  return true;
}
bool TestSomeMoreTypes::write_a_byte(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeByte(a_byte)) return false;
  return true;
}
bool TestSomeMoreTypes::nested_write_a_byte(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeByte(a_byte)) return false;
  return true;
}
bool TestSomeMoreTypes::write_a_i16(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeI16(a_i16)) return false;
  return true;
}
bool TestSomeMoreTypes::nested_write_a_i16(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeI16(a_i16)) return false;
  return true;
}
bool TestSomeMoreTypes::write_a_i32(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeI32(a_i32)) return false;
  return true;
}
bool TestSomeMoreTypes::nested_write_a_i32(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeI32(a_i32)) return false;
  return true;
}
bool TestSomeMoreTypes::write_a_i64(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeI64(a_i64)) return false;
  return true;
}
bool TestSomeMoreTypes::nested_write_a_i64(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeI64(a_i64)) return false;
  return true;
}
bool TestSomeMoreTypes::write_a_double(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeDouble(a_double)) return false;
  return true;
}
bool TestSomeMoreTypes::nested_write_a_double(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeDouble(a_double)) return false;
  return true;
}
bool TestSomeMoreTypes::write_a_string(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeString(a_string)) return false;
  return true;
}
bool TestSomeMoreTypes::nested_write_a_string(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeString(a_string)) return false;
  return true;
}
bool TestSomeMoreTypes::write_a_binary(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeBinary(a_binary)) return false;
  return true;
}
bool TestSomeMoreTypes::nested_write_a_binary(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeBinary(a_binary)) return false;
  return true;
}
bool TestSomeMoreTypes::write(yarp::os::idl::WireWriter& writer) {
  if (!write_a_bool(writer)) return false;
  if (!write_a_byte(writer)) return false;
  if (!write_a_i16(writer)) return false;
  if (!write_a_i32(writer)) return false;
  if (!write_a_i64(writer)) return false;
  if (!write_a_double(writer)) return false;
  if (!write_a_string(writer)) return false;
  if (!write_a_binary(writer)) return false;
  return !writer.isError();
}

bool TestSomeMoreTypes::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(8)) return false;
  return write(writer);
}
bool TestSomeMoreTypes::Editor::write(yarp::os::ConnectionWriter& connection) {
  if (!isValid()) return false;
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(dirty_count+1)) return false;
  if (!writer.writeString("patch")) return false;
  if (is_dirty_a_bool) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("a_bool")) return false;
    if (!obj->nested_write_a_bool(writer)) return false;
  }
  if (is_dirty_a_byte) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("a_byte")) return false;
    if (!obj->nested_write_a_byte(writer)) return false;
  }
  if (is_dirty_a_i16) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("a_i16")) return false;
    if (!obj->nested_write_a_i16(writer)) return false;
  }
  if (is_dirty_a_i32) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("a_i32")) return false;
    if (!obj->nested_write_a_i32(writer)) return false;
  }
  if (is_dirty_a_i64) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("a_i64")) return false;
    if (!obj->nested_write_a_i64(writer)) return false;
  }
  if (is_dirty_a_double) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("a_double")) return false;
    if (!obj->nested_write_a_double(writer)) return false;
  }
  if (is_dirty_a_string) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("a_string")) return false;
    if (!obj->nested_write_a_string(writer)) return false;
  }
  if (is_dirty_a_binary) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("a_binary")) return false;
    if (!obj->nested_write_a_binary(writer)) return false;
  }
  return !writer.isError();
}
bool TestSomeMoreTypes::Editor::read(yarp::os::ConnectionReader& connection) {
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
      if (field=="a_bool") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("bool a_bool")) return false;
      }
      if (field=="a_byte") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("int8_t a_byte")) return false;
      }
      if (field=="a_i16") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("int16_t a_i16")) return false;
      }
      if (field=="a_i32") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("int32_t a_i32")) return false;
      }
      if (field=="a_i64") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("int64_t a_i64")) return false;
      }
      if (field=="a_double") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("double a_double")) return false;
      }
      if (field=="a_string") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::string a_string")) return false;
      }
      if (field=="a_binary") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::string a_binary")) return false;
      }
    }
    if (!writer.writeListHeader(9)) return false;
    writer.writeString("*** Available fields:");
    writer.writeString("a_bool");
    writer.writeString("a_byte");
    writer.writeString("a_i16");
    writer.writeString("a_i32");
    writer.writeString("a_i64");
    writer.writeString("a_double");
    writer.writeString("a_string");
    writer.writeString("a_binary");
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
    if (key == "a_bool") {
      will_set_a_bool();
      if (!obj->nested_read_a_bool(reader)) return false;
      did_set_a_bool();
    } else if (key == "a_byte") {
      will_set_a_byte();
      if (!obj->nested_read_a_byte(reader)) return false;
      did_set_a_byte();
    } else if (key == "a_i16") {
      will_set_a_i16();
      if (!obj->nested_read_a_i16(reader)) return false;
      did_set_a_i16();
    } else if (key == "a_i32") {
      will_set_a_i32();
      if (!obj->nested_read_a_i32(reader)) return false;
      did_set_a_i32();
    } else if (key == "a_i64") {
      will_set_a_i64();
      if (!obj->nested_read_a_i64(reader)) return false;
      did_set_a_i64();
    } else if (key == "a_double") {
      will_set_a_double();
      if (!obj->nested_read_a_double(reader)) return false;
      did_set_a_double();
    } else if (key == "a_string") {
      will_set_a_string();
      if (!obj->nested_read_a_string(reader)) return false;
      did_set_a_string();
    } else if (key == "a_binary") {
      will_set_a_binary();
      if (!obj->nested_read_a_binary(reader)) return false;
      did_set_a_binary();
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

yarp::os::ConstString TestSomeMoreTypes::toString() {
  yarp::os::Bottle b;
  b.read(*this);
  return b.toString();
}

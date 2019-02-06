/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <firstInterface/PointD.h>

namespace yarp { namespace test {
bool PointD::read_x(yarp::os::idl::WireReader& reader) {
  if (!reader.readI32(x)) {
    reader.fail();
    return false;
  }
  return true;
}
bool PointD::nested_read_x(yarp::os::idl::WireReader& reader) {
  if (!reader.readI32(x)) {
    reader.fail();
    return false;
  }
  return true;
}
bool PointD::read_y(yarp::os::idl::WireReader& reader) {
  if (!reader.readI32(y)) {
    reader.fail();
    return false;
  }
  return true;
}
bool PointD::nested_read_y(yarp::os::idl::WireReader& reader) {
  if (!reader.readI32(y)) {
    reader.fail();
    return false;
  }
  return true;
}
bool PointD::read_z(yarp::os::idl::WireReader& reader) {
  if (!reader.readI32(z)) {
    reader.fail();
    return false;
  }
  return true;
}
bool PointD::nested_read_z(yarp::os::idl::WireReader& reader) {
  if (!reader.readI32(z)) {
    reader.fail();
    return false;
  }
  return true;
}
bool PointD::read(yarp::os::idl::WireReader& reader) {
  if (!read_x(reader)) return false;
  if (!read_y(reader)) return false;
  if (!read_z(reader)) return false;
  return !reader.isError();
}

bool PointD::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListHeader(3)) return false;
  return read(reader);
}

bool PointD::write_x(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeI32(x)) return false;
  return true;
}
bool PointD::nested_write_x(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeI32(x)) return false;
  return true;
}
bool PointD::write_y(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeI32(y)) return false;
  return true;
}
bool PointD::nested_write_y(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeI32(y)) return false;
  return true;
}
bool PointD::write_z(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeI32(z)) return false;
  return true;
}
bool PointD::nested_write_z(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeI32(z)) return false;
  return true;
}
bool PointD::write(const yarp::os::idl::WireWriter& writer) const {
  if (!write_x(writer)) return false;
  if (!write_y(writer)) return false;
  if (!write_z(writer)) return false;
  return !writer.isError();
}

bool PointD::write(yarp::os::ConnectionWriter& connection) const {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(3)) return false;
  return write(writer);
}
bool PointD::Editor::write(yarp::os::ConnectionWriter& connection) const {
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
  if (is_dirty_z) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("z")) return false;
    if (!obj->nested_write_z(writer)) return false;
  }
  return !writer.isError();
}
bool PointD::Editor::read(yarp::os::ConnectionReader& connection) {
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
  std::string tag;
  if (!reader.readString(tag)) return false;
  if (tag=="help") {
    yarp::os::idl::WireWriter writer(reader);
    if (writer.isNull()) return true;
    if (!writer.writeListHeader(2)) return false;
    if (!writer.writeTag("many",1, 0)) return false;
    if (reader.getLength()>0) {
      std::string field;
      if (!reader.readString(field)) return false;
      if (field=="x") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::int32_t x")) return false;
      }
      if (field=="y") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::int32_t y")) return false;
      }
      if (field=="z") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::int32_t z")) return false;
      }
    }
    if (!writer.writeListHeader(4)) return false;
    writer.writeString("*** Available fields:");
    writer.writeString("x");
    writer.writeString("y");
    writer.writeString("z");
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
    std::string act;
    std::string key;
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
    } else if (key == "z") {
      will_set_z();
      if (!obj->nested_read_z(reader)) return false;
      did_set_z();
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

std::string PointD::toString() const {
  yarp::os::Bottle b;
  b.read(*this);
  return b.toString();
}
}} // namespace

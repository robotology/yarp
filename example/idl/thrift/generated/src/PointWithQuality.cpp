/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <firstInterface/PointWithQuality.h>

namespace yarp { namespace test {
bool PointWithQuality::read_point(yarp::os::idl::WireReader& reader) {
  if (!reader.read(point)) {
    reader.fail();
    return false;
  }
  return true;
}
bool PointWithQuality::nested_read_point(yarp::os::idl::WireReader& reader) {
  if (!reader.readNested(point)) {
    reader.fail();
    return false;
  }
  return true;
}
bool PointWithQuality::read_quality(yarp::os::idl::WireReader& reader) {
  int32_t ecast24;
  PointQualityVocab cvrt25;
  if (!reader.readEnum(ecast24,cvrt25)) {
    quality = UNKNOWN;
  } else {
    quality = (PointQuality)ecast24;
  }
  return true;
}
bool PointWithQuality::nested_read_quality(yarp::os::idl::WireReader& reader) {
  int32_t ecast26;
  PointQualityVocab cvrt27;
  if (!reader.readEnum(ecast26,cvrt27)) {
    quality = UNKNOWN;
  } else {
    quality = (PointQuality)ecast26;
  }
  return true;
}
bool PointWithQuality::read(yarp::os::idl::WireReader& reader) {
  if (!read_point(reader)) return false;
  if (!read_quality(reader)) return false;
  return !reader.isError();
}

bool PointWithQuality::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListHeader(4)) return false;
  return read(reader);
}

bool PointWithQuality::write_point(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.write(point)) return false;
  return true;
}
bool PointWithQuality::nested_write_point(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeNested(point)) return false;
  return true;
}
bool PointWithQuality::write_quality(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeI32((int32_t)quality)) return false;
  return true;
}
bool PointWithQuality::nested_write_quality(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeI32((int32_t)quality)) return false;
  return true;
}
bool PointWithQuality::write(const yarp::os::idl::WireWriter& writer) const {
  if (!write_point(writer)) return false;
  if (!write_quality(writer)) return false;
  return !writer.isError();
}

bool PointWithQuality::write(yarp::os::ConnectionWriter& connection) const {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(4)) return false;
  return write(writer);
}
bool PointWithQuality::Editor::write(yarp::os::ConnectionWriter& connection) const {
  if (!isValid()) return false;
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(dirty_count+1)) return false;
  if (!writer.writeString("patch")) return false;
  if (is_dirty_point) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("point")) return false;
    if (!obj->nested_write_point(writer)) return false;
  }
  if (is_dirty_quality) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("quality")) return false;
    if (!obj->nested_write_quality(writer)) return false;
  }
  return !writer.isError();
}
bool PointWithQuality::Editor::read(yarp::os::ConnectionReader& connection) {
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
      if (field=="point") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("PointD point")) return false;
      }
      if (field=="quality") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("PointQuality quality")) return false;
      }
    }
    if (!writer.writeListHeader(3)) return false;
    writer.writeString("*** Available fields:");
    writer.writeString("point");
    writer.writeString("quality");
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
    if (key == "point") {
      will_set_point();
      if (!obj->nested_read_point(reader)) return false;
      did_set_point();
    } else if (key == "quality") {
      will_set_quality();
      if (!obj->nested_read_quality(reader)) return false;
      did_set_quality();
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

std::string PointWithQuality::toString() const {
  yarp::os::Bottle b;
  b.read(*this);
  return b.toString();
}
}} // namespace

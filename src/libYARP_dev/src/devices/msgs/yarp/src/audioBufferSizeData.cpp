/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <audioBufferSizeData.h>

namespace yarp { namespace dev {
bool audioBufferSizeData::read_m_samples(yarp::os::idl::WireReader& reader) {
  if (!reader.readI32(m_samples)) {
    reader.fail();
    return false;
  }
  return true;
}
bool audioBufferSizeData::nested_read_m_samples(yarp::os::idl::WireReader& reader) {
  if (!reader.readI32(m_samples)) {
    reader.fail();
    return false;
  }
  return true;
}
bool audioBufferSizeData::read_m_channels(yarp::os::idl::WireReader& reader) {
  if (!reader.readI32(m_channels)) {
    reader.fail();
    return false;
  }
  return true;
}
bool audioBufferSizeData::nested_read_m_channels(yarp::os::idl::WireReader& reader) {
  if (!reader.readI32(m_channels)) {
    reader.fail();
    return false;
  }
  return true;
}
bool audioBufferSizeData::read_m_depth(yarp::os::idl::WireReader& reader) {
  if (!reader.readI32(m_depth)) {
    reader.fail();
    return false;
  }
  return true;
}
bool audioBufferSizeData::nested_read_m_depth(yarp::os::idl::WireReader& reader) {
  if (!reader.readI32(m_depth)) {
    reader.fail();
    return false;
  }
  return true;
}
bool audioBufferSizeData::read_size(yarp::os::idl::WireReader& reader) {
  if (!reader.readI32(size)) {
    reader.fail();
    return false;
  }
  return true;
}
bool audioBufferSizeData::nested_read_size(yarp::os::idl::WireReader& reader) {
  if (!reader.readI32(size)) {
    reader.fail();
    return false;
  }
  return true;
}
bool audioBufferSizeData::read(yarp::os::idl::WireReader& reader) {
  if (!read_m_samples(reader)) return false;
  if (!read_m_channels(reader)) return false;
  if (!read_m_depth(reader)) return false;
  if (!read_size(reader)) return false;
  return !reader.isError();
}

bool audioBufferSizeData::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListHeader(4)) return false;
  return read(reader);
}

bool audioBufferSizeData::write_m_samples(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeI32(m_samples)) return false;
  return true;
}
bool audioBufferSizeData::nested_write_m_samples(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeI32(m_samples)) return false;
  return true;
}
bool audioBufferSizeData::write_m_channels(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeI32(m_channels)) return false;
  return true;
}
bool audioBufferSizeData::nested_write_m_channels(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeI32(m_channels)) return false;
  return true;
}
bool audioBufferSizeData::write_m_depth(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeI32(m_depth)) return false;
  return true;
}
bool audioBufferSizeData::nested_write_m_depth(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeI32(m_depth)) return false;
  return true;
}
bool audioBufferSizeData::write_size(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeI32(size)) return false;
  return true;
}
bool audioBufferSizeData::nested_write_size(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeI32(size)) return false;
  return true;
}
bool audioBufferSizeData::write(const yarp::os::idl::WireWriter& writer) const {
  if (!write_m_samples(writer)) return false;
  if (!write_m_channels(writer)) return false;
  if (!write_m_depth(writer)) return false;
  if (!write_size(writer)) return false;
  return !writer.isError();
}

bool audioBufferSizeData::write(yarp::os::ConnectionWriter& connection) const {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(4)) return false;
  return write(writer);
}
bool audioBufferSizeData::Editor::write(yarp::os::ConnectionWriter& connection) const {
  if (!isValid()) return false;
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(dirty_count+1)) return false;
  if (!writer.writeString("patch")) return false;
  if (is_dirty_m_samples) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("m_samples")) return false;
    if (!obj->nested_write_m_samples(writer)) return false;
  }
  if (is_dirty_m_channels) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("m_channels")) return false;
    if (!obj->nested_write_m_channels(writer)) return false;
  }
  if (is_dirty_m_depth) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("m_depth")) return false;
    if (!obj->nested_write_m_depth(writer)) return false;
  }
  if (is_dirty_size) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("size")) return false;
    if (!obj->nested_write_size(writer)) return false;
  }
  return !writer.isError();
}
bool audioBufferSizeData::Editor::read(yarp::os::ConnectionReader& connection) {
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
      if (field=="m_samples") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::int32_t m_samples")) return false;
      }
      if (field=="m_channels") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::int32_t m_channels")) return false;
      }
      if (field=="m_depth") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::int32_t m_depth")) return false;
      }
      if (field=="size") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::int32_t size")) return false;
      }
    }
    if (!writer.writeListHeader(5)) return false;
    writer.writeString("*** Available fields:");
    writer.writeString("m_samples");
    writer.writeString("m_channels");
    writer.writeString("m_depth");
    writer.writeString("size");
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
    if (key == "m_samples") {
      will_set_m_samples();
      if (!obj->nested_read_m_samples(reader)) return false;
      did_set_m_samples();
    } else if (key == "m_channels") {
      will_set_m_channels();
      if (!obj->nested_read_m_channels(reader)) return false;
      did_set_m_channels();
    } else if (key == "m_depth") {
      will_set_m_depth();
      if (!obj->nested_read_m_depth(reader)) return false;
      did_set_m_depth();
    } else if (key == "size") {
      will_set_size();
      if (!obj->nested_read_size(reader)) return false;
      did_set_size();
    } else {
      // would be useful to have a fallback here
    }
  }
  reader.accept();
  yarp::os::idl::WireWriter writer(reader);
  if (writer.isNull()) return true;
  writer.writeListHeader(1);
  writer.writeVocab(yarp::os::createVocab('o','k'));
  return true;
}

std::string audioBufferSizeData::toString() const {
  yarp::os::Bottle b;
  b.read(*this);
  return b.toString();
}
}} // namespace

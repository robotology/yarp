/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <SensorMetadata.h>

bool SensorMetadata::read_name(yarp::os::idl::WireReader& reader) {
  if (!reader.readString(name)) {
    reader.fail();
    return false;
  }
  return true;
}
bool SensorMetadata::nested_read_name(yarp::os::idl::WireReader& reader) {
  if (!reader.readString(name)) {
    reader.fail();
    return false;
  }
  return true;
}
bool SensorMetadata::read_frameName(yarp::os::idl::WireReader& reader) {
  if (!reader.readString(frameName)) {
    reader.fail();
    return false;
  }
  return true;
}
bool SensorMetadata::nested_read_frameName(yarp::os::idl::WireReader& reader) {
  if (!reader.readString(frameName)) {
    reader.fail();
    return false;
  }
  return true;
}
bool SensorMetadata::read_additionalMetadata(yarp::os::idl::WireReader& reader) {
  if (!reader.readString(additionalMetadata)) {
    reader.fail();
    return false;
  }
  return true;
}
bool SensorMetadata::nested_read_additionalMetadata(yarp::os::idl::WireReader& reader) {
  if (!reader.readString(additionalMetadata)) {
    reader.fail();
    return false;
  }
  return true;
}
bool SensorMetadata::read(yarp::os::idl::WireReader& reader) {
  if (!read_name(reader)) return false;
  if (!read_frameName(reader)) return false;
  if (!read_additionalMetadata(reader)) return false;
  return !reader.isError();
}

bool SensorMetadata::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListHeader(3)) return false;
  return read(reader);
}

bool SensorMetadata::write_name(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeString(name)) return false;
  return true;
}
bool SensorMetadata::nested_write_name(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeString(name)) return false;
  return true;
}
bool SensorMetadata::write_frameName(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeString(frameName)) return false;
  return true;
}
bool SensorMetadata::nested_write_frameName(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeString(frameName)) return false;
  return true;
}
bool SensorMetadata::write_additionalMetadata(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeString(additionalMetadata)) return false;
  return true;
}
bool SensorMetadata::nested_write_additionalMetadata(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeString(additionalMetadata)) return false;
  return true;
}
bool SensorMetadata::write(const yarp::os::idl::WireWriter& writer) const {
  if (!write_name(writer)) return false;
  if (!write_frameName(writer)) return false;
  if (!write_additionalMetadata(writer)) return false;
  return !writer.isError();
}

bool SensorMetadata::write(yarp::os::ConnectionWriter& connection) const {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(3)) return false;
  return write(writer);
}
bool SensorMetadata::Editor::write(yarp::os::ConnectionWriter& connection) const {
  if (!isValid()) return false;
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(dirty_count+1)) return false;
  if (!writer.writeString("patch")) return false;
  if (is_dirty_name) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("name")) return false;
    if (!obj->nested_write_name(writer)) return false;
  }
  if (is_dirty_frameName) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("frameName")) return false;
    if (!obj->nested_write_frameName(writer)) return false;
  }
  if (is_dirty_additionalMetadata) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("additionalMetadata")) return false;
    if (!obj->nested_write_additionalMetadata(writer)) return false;
  }
  return !writer.isError();
}
bool SensorMetadata::Editor::read(yarp::os::ConnectionReader& connection) {
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
      if (field=="name") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::string name")) return false;
      }
      if (field=="frameName") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::string frameName")) return false;
      }
      if (field=="additionalMetadata") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::string additionalMetadata")) return false;
      }
    }
    if (!writer.writeListHeader(4)) return false;
    writer.writeString("*** Available fields:");
    writer.writeString("name");
    writer.writeString("frameName");
    writer.writeString("additionalMetadata");
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
    if (key == "name") {
      will_set_name();
      if (!obj->nested_read_name(reader)) return false;
      did_set_name();
    } else if (key == "frameName") {
      will_set_frameName();
      if (!obj->nested_read_frameName(reader)) return false;
      did_set_frameName();
    } else if (key == "additionalMetadata") {
      will_set_additionalMetadata();
      if (!obj->nested_read_additionalMetadata(reader)) return false;
      did_set_additionalMetadata();
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

std::string SensorMetadata::toString() const {
  yarp::os::Bottle b;
  b.read(*this);
  return b.toString();
}

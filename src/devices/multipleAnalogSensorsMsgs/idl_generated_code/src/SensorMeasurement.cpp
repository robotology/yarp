/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <SensorMeasurement.h>

bool SensorMeasurement::read_measurement(yarp::os::idl::WireReader& reader) {
  if (!reader.read(measurement)) {
    reader.fail();
    return false;
  }
  return true;
}
bool SensorMeasurement::nested_read_measurement(yarp::os::idl::WireReader& reader) {
  if (!reader.readNested(measurement)) {
    reader.fail();
    return false;
  }
  return true;
}
bool SensorMeasurement::read_timestamp(yarp::os::idl::WireReader& reader) {
  if (!reader.readFloat64(timestamp)) {
    reader.fail();
    return false;
  }
  return true;
}
bool SensorMeasurement::nested_read_timestamp(yarp::os::idl::WireReader& reader) {
  if (!reader.readFloat64(timestamp)) {
    reader.fail();
    return false;
  }
  return true;
}
bool SensorMeasurement::read(yarp::os::idl::WireReader& reader) {
  if (!read_measurement(reader)) return false;
  if (!read_timestamp(reader)) return false;
  return !reader.isError();
}

bool SensorMeasurement::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListHeader(2)) return false;
  return read(reader);
}

bool SensorMeasurement::write_measurement(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.write(measurement)) return false;
  return true;
}
bool SensorMeasurement::nested_write_measurement(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeNested(measurement)) return false;
  return true;
}
bool SensorMeasurement::write_timestamp(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeFloat64(timestamp)) return false;
  return true;
}
bool SensorMeasurement::nested_write_timestamp(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeFloat64(timestamp)) return false;
  return true;
}
bool SensorMeasurement::write(const yarp::os::idl::WireWriter& writer) const {
  if (!write_measurement(writer)) return false;
  if (!write_timestamp(writer)) return false;
  return !writer.isError();
}

bool SensorMeasurement::write(yarp::os::ConnectionWriter& connection) const {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(2)) return false;
  return write(writer);
}
bool SensorMeasurement::Editor::write(yarp::os::ConnectionWriter& connection) const {
  if (!isValid()) return false;
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(dirty_count+1)) return false;
  if (!writer.writeString("patch")) return false;
  if (is_dirty_measurement) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("measurement")) return false;
    if (!obj->nested_write_measurement(writer)) return false;
  }
  if (is_dirty_timestamp) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("timestamp")) return false;
    if (!obj->nested_write_timestamp(writer)) return false;
  }
  return !writer.isError();
}
bool SensorMeasurement::Editor::read(yarp::os::ConnectionReader& connection) {
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
      if (field=="measurement") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("yarp::sig::Vector measurement")) return false;
      }
      if (field=="timestamp") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("double timestamp")) return false;
      }
    }
    if (!writer.writeListHeader(3)) return false;
    writer.writeString("*** Available fields:");
    writer.writeString("measurement");
    writer.writeString("timestamp");
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
    if (key == "measurement") {
      will_set_measurement();
      if (!obj->nested_read_measurement(reader)) return false;
      did_set_measurement();
    } else if (key == "timestamp") {
      will_set_timestamp();
      if (!obj->nested_read_timestamp(reader)) return false;
      did_set_timestamp();
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

std::string SensorMeasurement::toString() const {
  yarp::os::Bottle b;
  b.read(*this);
  return b.toString();
}

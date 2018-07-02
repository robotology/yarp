/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <SensorMeasurements.h>

bool SensorMeasurements::read_measurements(yarp::os::idl::WireReader& reader) {
  {
    measurements.clear();
    uint32_t _size0;
    yarp::os::idl::WireState _etype3;
    reader.readListBegin(_etype3, _size0);
    measurements.resize(_size0);
    uint32_t _i4;
    for (_i4 = 0; _i4 < _size0; ++_i4)
    {
      if (!reader.readNested(measurements[_i4])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool SensorMeasurements::nested_read_measurements(yarp::os::idl::WireReader& reader) {
  {
    measurements.clear();
    uint32_t _size5;
    yarp::os::idl::WireState _etype8;
    reader.readListBegin(_etype8, _size5);
    measurements.resize(_size5);
    uint32_t _i9;
    for (_i9 = 0; _i9 < _size5; ++_i9)
    {
      if (!reader.readNested(measurements[_i9])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool SensorMeasurements::read(yarp::os::idl::WireReader& reader) {
  if (!read_measurements(reader)) return false;
  return !reader.isError();
}

bool SensorMeasurements::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListHeader(1)) return false;
  return read(reader);
}

bool SensorMeasurements::write_measurements(const yarp::os::idl::WireWriter& writer) const {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(measurements.size()))) return false;
    std::vector<SensorMeasurement> ::const_iterator _iter10;
    for (_iter10 = measurements.begin(); _iter10 != measurements.end(); ++_iter10)
    {
      if (!writer.writeNested((*_iter10))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool SensorMeasurements::nested_write_measurements(const yarp::os::idl::WireWriter& writer) const {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(measurements.size()))) return false;
    std::vector<SensorMeasurement> ::const_iterator _iter11;
    for (_iter11 = measurements.begin(); _iter11 != measurements.end(); ++_iter11)
    {
      if (!writer.writeNested((*_iter11))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool SensorMeasurements::write(const yarp::os::idl::WireWriter& writer) const {
  if (!write_measurements(writer)) return false;
  return !writer.isError();
}

bool SensorMeasurements::write(yarp::os::ConnectionWriter& connection) const {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(1)) return false;
  return write(writer);
}
bool SensorMeasurements::Editor::write(yarp::os::ConnectionWriter& connection) const {
  if (!isValid()) return false;
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(dirty_count+1)) return false;
  if (!writer.writeString("patch")) return false;
  if (is_dirty_measurements) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("measurements")) return false;
    if (!obj->nested_write_measurements(writer)) return false;
  }
  return !writer.isError();
}
bool SensorMeasurements::Editor::read(yarp::os::ConnectionReader& connection) {
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
      if (field=="measurements") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::vector<SensorMeasurement>  measurements")) return false;
      }
    }
    if (!writer.writeListHeader(2)) return false;
    writer.writeString("*** Available fields:");
    writer.writeString("measurements");
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
    if (key == "measurements") {
      will_set_measurements();
      if (!obj->nested_read_measurements(reader)) return false;
      did_set_measurements();
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

std::string SensorMeasurements::toString() const {
  yarp::os::Bottle b;
  b.read(*this);
  return b.toString();
}

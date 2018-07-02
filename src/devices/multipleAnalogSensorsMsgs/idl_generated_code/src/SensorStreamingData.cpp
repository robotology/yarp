/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <SensorStreamingData.h>

bool SensorStreamingData::read_ThreeAxisGyroscopes(yarp::os::idl::WireReader& reader) {
  if (!reader.read(ThreeAxisGyroscopes)) {
    reader.fail();
    return false;
  }
  return true;
}
bool SensorStreamingData::nested_read_ThreeAxisGyroscopes(yarp::os::idl::WireReader& reader) {
  if (!reader.readNested(ThreeAxisGyroscopes)) {
    reader.fail();
    return false;
  }
  return true;
}
bool SensorStreamingData::read_ThreeAxisLinearAccelerometers(yarp::os::idl::WireReader& reader) {
  if (!reader.read(ThreeAxisLinearAccelerometers)) {
    reader.fail();
    return false;
  }
  return true;
}
bool SensorStreamingData::nested_read_ThreeAxisLinearAccelerometers(yarp::os::idl::WireReader& reader) {
  if (!reader.readNested(ThreeAxisLinearAccelerometers)) {
    reader.fail();
    return false;
  }
  return true;
}
bool SensorStreamingData::read_ThreeAxisMagnetometers(yarp::os::idl::WireReader& reader) {
  if (!reader.read(ThreeAxisMagnetometers)) {
    reader.fail();
    return false;
  }
  return true;
}
bool SensorStreamingData::nested_read_ThreeAxisMagnetometers(yarp::os::idl::WireReader& reader) {
  if (!reader.readNested(ThreeAxisMagnetometers)) {
    reader.fail();
    return false;
  }
  return true;
}
bool SensorStreamingData::read_OrientationSensors(yarp::os::idl::WireReader& reader) {
  if (!reader.read(OrientationSensors)) {
    reader.fail();
    return false;
  }
  return true;
}
bool SensorStreamingData::nested_read_OrientationSensors(yarp::os::idl::WireReader& reader) {
  if (!reader.readNested(OrientationSensors)) {
    reader.fail();
    return false;
  }
  return true;
}
bool SensorStreamingData::read_TemperatureSensors(yarp::os::idl::WireReader& reader) {
  if (!reader.read(TemperatureSensors)) {
    reader.fail();
    return false;
  }
  return true;
}
bool SensorStreamingData::nested_read_TemperatureSensors(yarp::os::idl::WireReader& reader) {
  if (!reader.readNested(TemperatureSensors)) {
    reader.fail();
    return false;
  }
  return true;
}
bool SensorStreamingData::read_SixAxisForceTorqueSensors(yarp::os::idl::WireReader& reader) {
  if (!reader.read(SixAxisForceTorqueSensors)) {
    reader.fail();
    return false;
  }
  return true;
}
bool SensorStreamingData::nested_read_SixAxisForceTorqueSensors(yarp::os::idl::WireReader& reader) {
  if (!reader.readNested(SixAxisForceTorqueSensors)) {
    reader.fail();
    return false;
  }
  return true;
}
bool SensorStreamingData::read_ContactLoadCellArrays(yarp::os::idl::WireReader& reader) {
  if (!reader.read(ContactLoadCellArrays)) {
    reader.fail();
    return false;
  }
  return true;
}
bool SensorStreamingData::nested_read_ContactLoadCellArrays(yarp::os::idl::WireReader& reader) {
  if (!reader.readNested(ContactLoadCellArrays)) {
    reader.fail();
    return false;
  }
  return true;
}
bool SensorStreamingData::read_EncoderArrays(yarp::os::idl::WireReader& reader) {
  if (!reader.read(EncoderArrays)) {
    reader.fail();
    return false;
  }
  return true;
}
bool SensorStreamingData::nested_read_EncoderArrays(yarp::os::idl::WireReader& reader) {
  if (!reader.readNested(EncoderArrays)) {
    reader.fail();
    return false;
  }
  return true;
}
bool SensorStreamingData::read_SkinPatches(yarp::os::idl::WireReader& reader) {
  if (!reader.read(SkinPatches)) {
    reader.fail();
    return false;
  }
  return true;
}
bool SensorStreamingData::nested_read_SkinPatches(yarp::os::idl::WireReader& reader) {
  if (!reader.readNested(SkinPatches)) {
    reader.fail();
    return false;
  }
  return true;
}
bool SensorStreamingData::read(yarp::os::idl::WireReader& reader) {
  if (!read_ThreeAxisGyroscopes(reader)) return false;
  if (!read_ThreeAxisLinearAccelerometers(reader)) return false;
  if (!read_ThreeAxisMagnetometers(reader)) return false;
  if (!read_OrientationSensors(reader)) return false;
  if (!read_TemperatureSensors(reader)) return false;
  if (!read_SixAxisForceTorqueSensors(reader)) return false;
  if (!read_ContactLoadCellArrays(reader)) return false;
  if (!read_EncoderArrays(reader)) return false;
  if (!read_SkinPatches(reader)) return false;
  return !reader.isError();
}

bool SensorStreamingData::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListHeader(9)) return false;
  return read(reader);
}

bool SensorStreamingData::write_ThreeAxisGyroscopes(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.write(ThreeAxisGyroscopes)) return false;
  return true;
}
bool SensorStreamingData::nested_write_ThreeAxisGyroscopes(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeNested(ThreeAxisGyroscopes)) return false;
  return true;
}
bool SensorStreamingData::write_ThreeAxisLinearAccelerometers(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.write(ThreeAxisLinearAccelerometers)) return false;
  return true;
}
bool SensorStreamingData::nested_write_ThreeAxisLinearAccelerometers(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeNested(ThreeAxisLinearAccelerometers)) return false;
  return true;
}
bool SensorStreamingData::write_ThreeAxisMagnetometers(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.write(ThreeAxisMagnetometers)) return false;
  return true;
}
bool SensorStreamingData::nested_write_ThreeAxisMagnetometers(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeNested(ThreeAxisMagnetometers)) return false;
  return true;
}
bool SensorStreamingData::write_OrientationSensors(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.write(OrientationSensors)) return false;
  return true;
}
bool SensorStreamingData::nested_write_OrientationSensors(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeNested(OrientationSensors)) return false;
  return true;
}
bool SensorStreamingData::write_TemperatureSensors(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.write(TemperatureSensors)) return false;
  return true;
}
bool SensorStreamingData::nested_write_TemperatureSensors(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeNested(TemperatureSensors)) return false;
  return true;
}
bool SensorStreamingData::write_SixAxisForceTorqueSensors(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.write(SixAxisForceTorqueSensors)) return false;
  return true;
}
bool SensorStreamingData::nested_write_SixAxisForceTorqueSensors(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeNested(SixAxisForceTorqueSensors)) return false;
  return true;
}
bool SensorStreamingData::write_ContactLoadCellArrays(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.write(ContactLoadCellArrays)) return false;
  return true;
}
bool SensorStreamingData::nested_write_ContactLoadCellArrays(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeNested(ContactLoadCellArrays)) return false;
  return true;
}
bool SensorStreamingData::write_EncoderArrays(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.write(EncoderArrays)) return false;
  return true;
}
bool SensorStreamingData::nested_write_EncoderArrays(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeNested(EncoderArrays)) return false;
  return true;
}
bool SensorStreamingData::write_SkinPatches(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.write(SkinPatches)) return false;
  return true;
}
bool SensorStreamingData::nested_write_SkinPatches(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeNested(SkinPatches)) return false;
  return true;
}
bool SensorStreamingData::write(const yarp::os::idl::WireWriter& writer) const {
  if (!write_ThreeAxisGyroscopes(writer)) return false;
  if (!write_ThreeAxisLinearAccelerometers(writer)) return false;
  if (!write_ThreeAxisMagnetometers(writer)) return false;
  if (!write_OrientationSensors(writer)) return false;
  if (!write_TemperatureSensors(writer)) return false;
  if (!write_SixAxisForceTorqueSensors(writer)) return false;
  if (!write_ContactLoadCellArrays(writer)) return false;
  if (!write_EncoderArrays(writer)) return false;
  if (!write_SkinPatches(writer)) return false;
  return !writer.isError();
}

bool SensorStreamingData::write(yarp::os::ConnectionWriter& connection) const {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(9)) return false;
  return write(writer);
}
bool SensorStreamingData::Editor::write(yarp::os::ConnectionWriter& connection) const {
  if (!isValid()) return false;
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(dirty_count+1)) return false;
  if (!writer.writeString("patch")) return false;
  if (is_dirty_ThreeAxisGyroscopes) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("ThreeAxisGyroscopes")) return false;
    if (!obj->nested_write_ThreeAxisGyroscopes(writer)) return false;
  }
  if (is_dirty_ThreeAxisLinearAccelerometers) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("ThreeAxisLinearAccelerometers")) return false;
    if (!obj->nested_write_ThreeAxisLinearAccelerometers(writer)) return false;
  }
  if (is_dirty_ThreeAxisMagnetometers) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("ThreeAxisMagnetometers")) return false;
    if (!obj->nested_write_ThreeAxisMagnetometers(writer)) return false;
  }
  if (is_dirty_OrientationSensors) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("OrientationSensors")) return false;
    if (!obj->nested_write_OrientationSensors(writer)) return false;
  }
  if (is_dirty_TemperatureSensors) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("TemperatureSensors")) return false;
    if (!obj->nested_write_TemperatureSensors(writer)) return false;
  }
  if (is_dirty_SixAxisForceTorqueSensors) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("SixAxisForceTorqueSensors")) return false;
    if (!obj->nested_write_SixAxisForceTorqueSensors(writer)) return false;
  }
  if (is_dirty_ContactLoadCellArrays) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("ContactLoadCellArrays")) return false;
    if (!obj->nested_write_ContactLoadCellArrays(writer)) return false;
  }
  if (is_dirty_EncoderArrays) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("EncoderArrays")) return false;
    if (!obj->nested_write_EncoderArrays(writer)) return false;
  }
  if (is_dirty_SkinPatches) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("SkinPatches")) return false;
    if (!obj->nested_write_SkinPatches(writer)) return false;
  }
  return !writer.isError();
}
bool SensorStreamingData::Editor::read(yarp::os::ConnectionReader& connection) {
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
      if (field=="ThreeAxisGyroscopes") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("SensorMeasurements ThreeAxisGyroscopes")) return false;
      }
      if (field=="ThreeAxisLinearAccelerometers") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("SensorMeasurements ThreeAxisLinearAccelerometers")) return false;
      }
      if (field=="ThreeAxisMagnetometers") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("SensorMeasurements ThreeAxisMagnetometers")) return false;
      }
      if (field=="OrientationSensors") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("SensorMeasurements OrientationSensors")) return false;
      }
      if (field=="TemperatureSensors") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("SensorMeasurements TemperatureSensors")) return false;
      }
      if (field=="SixAxisForceTorqueSensors") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("SensorMeasurements SixAxisForceTorqueSensors")) return false;
      }
      if (field=="ContactLoadCellArrays") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("SensorMeasurements ContactLoadCellArrays")) return false;
      }
      if (field=="EncoderArrays") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("SensorMeasurements EncoderArrays")) return false;
      }
      if (field=="SkinPatches") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("SensorMeasurements SkinPatches")) return false;
      }
    }
    if (!writer.writeListHeader(10)) return false;
    writer.writeString("*** Available fields:");
    writer.writeString("ThreeAxisGyroscopes");
    writer.writeString("ThreeAxisLinearAccelerometers");
    writer.writeString("ThreeAxisMagnetometers");
    writer.writeString("OrientationSensors");
    writer.writeString("TemperatureSensors");
    writer.writeString("SixAxisForceTorqueSensors");
    writer.writeString("ContactLoadCellArrays");
    writer.writeString("EncoderArrays");
    writer.writeString("SkinPatches");
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
    if (key == "ThreeAxisGyroscopes") {
      will_set_ThreeAxisGyroscopes();
      if (!obj->nested_read_ThreeAxisGyroscopes(reader)) return false;
      did_set_ThreeAxisGyroscopes();
    } else if (key == "ThreeAxisLinearAccelerometers") {
      will_set_ThreeAxisLinearAccelerometers();
      if (!obj->nested_read_ThreeAxisLinearAccelerometers(reader)) return false;
      did_set_ThreeAxisLinearAccelerometers();
    } else if (key == "ThreeAxisMagnetometers") {
      will_set_ThreeAxisMagnetometers();
      if (!obj->nested_read_ThreeAxisMagnetometers(reader)) return false;
      did_set_ThreeAxisMagnetometers();
    } else if (key == "OrientationSensors") {
      will_set_OrientationSensors();
      if (!obj->nested_read_OrientationSensors(reader)) return false;
      did_set_OrientationSensors();
    } else if (key == "TemperatureSensors") {
      will_set_TemperatureSensors();
      if (!obj->nested_read_TemperatureSensors(reader)) return false;
      did_set_TemperatureSensors();
    } else if (key == "SixAxisForceTorqueSensors") {
      will_set_SixAxisForceTorqueSensors();
      if (!obj->nested_read_SixAxisForceTorqueSensors(reader)) return false;
      did_set_SixAxisForceTorqueSensors();
    } else if (key == "ContactLoadCellArrays") {
      will_set_ContactLoadCellArrays();
      if (!obj->nested_read_ContactLoadCellArrays(reader)) return false;
      did_set_ContactLoadCellArrays();
    } else if (key == "EncoderArrays") {
      will_set_EncoderArrays();
      if (!obj->nested_read_EncoderArrays(reader)) return false;
      did_set_EncoderArrays();
    } else if (key == "SkinPatches") {
      will_set_SkinPatches();
      if (!obj->nested_read_SkinPatches(reader)) return false;
      did_set_SkinPatches();
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

std::string SensorStreamingData::toString() const {
  yarp::os::Bottle b;
  b.read(*this);
  return b.toString();
}

// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <jointData.h>

bool jointData::read_position(yarp::os::idl::WireReader& reader) {
  {
    position.clear();
    uint32_t _size0;
    yarp::os::idl::WireState _etype3;
    reader.readListBegin(_etype3, _size0);
    position.resize(_size0);
    uint32_t _i4;
    for (_i4 = 0; _i4 < _size0; ++_i4)
    {
      if (!reader.readDouble(position[_i4])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool jointData::nested_read_position(yarp::os::idl::WireReader& reader) {
  {
    position.clear();
    uint32_t _size5;
    yarp::os::idl::WireState _etype8;
    reader.readListBegin(_etype8, _size5);
    position.resize(_size5);
    uint32_t _i9;
    for (_i9 = 0; _i9 < _size5; ++_i9)
    {
      if (!reader.readDouble(position[_i9])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool jointData::read_velocity(yarp::os::idl::WireReader& reader) {
  {
    velocity.clear();
    uint32_t _size10;
    yarp::os::idl::WireState _etype13;
    reader.readListBegin(_etype13, _size10);
    velocity.resize(_size10);
    uint32_t _i14;
    for (_i14 = 0; _i14 < _size10; ++_i14)
    {
      if (!reader.readDouble(velocity[_i14])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool jointData::nested_read_velocity(yarp::os::idl::WireReader& reader) {
  {
    velocity.clear();
    uint32_t _size15;
    yarp::os::idl::WireState _etype18;
    reader.readListBegin(_etype18, _size15);
    velocity.resize(_size15);
    uint32_t _i19;
    for (_i19 = 0; _i19 < _size15; ++_i19)
    {
      if (!reader.readDouble(velocity[_i19])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool jointData::read_acceleration(yarp::os::idl::WireReader& reader) {
  {
    acceleration.clear();
    uint32_t _size20;
    yarp::os::idl::WireState _etype23;
    reader.readListBegin(_etype23, _size20);
    acceleration.resize(_size20);
    uint32_t _i24;
    for (_i24 = 0; _i24 < _size20; ++_i24)
    {
      if (!reader.readDouble(acceleration[_i24])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool jointData::nested_read_acceleration(yarp::os::idl::WireReader& reader) {
  {
    acceleration.clear();
    uint32_t _size25;
    yarp::os::idl::WireState _etype28;
    reader.readListBegin(_etype28, _size25);
    acceleration.resize(_size25);
    uint32_t _i29;
    for (_i29 = 0; _i29 < _size25; ++_i29)
    {
      if (!reader.readDouble(acceleration[_i29])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool jointData::read_torque(yarp::os::idl::WireReader& reader) {
  {
    torque.clear();
    uint32_t _size30;
    yarp::os::idl::WireState _etype33;
    reader.readListBegin(_etype33, _size30);
    torque.resize(_size30);
    uint32_t _i34;
    for (_i34 = 0; _i34 < _size30; ++_i34)
    {
      if (!reader.readDouble(torque[_i34])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool jointData::nested_read_torque(yarp::os::idl::WireReader& reader) {
  {
    torque.clear();
    uint32_t _size35;
    yarp::os::idl::WireState _etype38;
    reader.readListBegin(_etype38, _size35);
    torque.resize(_size35);
    uint32_t _i39;
    for (_i39 = 0; _i39 < _size35; ++_i39)
    {
      if (!reader.readDouble(torque[_i39])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool jointData::read_pidOutput(yarp::os::idl::WireReader& reader) {
  {
    pidOutput.clear();
    uint32_t _size40;
    yarp::os::idl::WireState _etype43;
    reader.readListBegin(_etype43, _size40);
    pidOutput.resize(_size40);
    uint32_t _i44;
    for (_i44 = 0; _i44 < _size40; ++_i44)
    {
      if (!reader.readDouble(pidOutput[_i44])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool jointData::nested_read_pidOutput(yarp::os::idl::WireReader& reader) {
  {
    pidOutput.clear();
    uint32_t _size45;
    yarp::os::idl::WireState _etype48;
    reader.readListBegin(_etype48, _size45);
    pidOutput.resize(_size45);
    uint32_t _i49;
    for (_i49 = 0; _i49 < _size45; ++_i49)
    {
      if (!reader.readDouble(pidOutput[_i49])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool jointData::read_controlMode(yarp::os::idl::WireReader& reader) {
  {
    controlMode.clear();
    uint32_t _size50;
    yarp::os::idl::WireState _etype53;
    reader.readListBegin(_etype53, _size50);
    controlMode.resize(_size50);
    uint32_t _i54;
    for (_i54 = 0; _i54 < _size50; ++_i54)
    {
      if (!reader.readI32(controlMode[_i54])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool jointData::nested_read_controlMode(yarp::os::idl::WireReader& reader) {
  {
    controlMode.clear();
    uint32_t _size55;
    yarp::os::idl::WireState _etype58;
    reader.readListBegin(_etype58, _size55);
    controlMode.resize(_size55);
    uint32_t _i59;
    for (_i59 = 0; _i59 < _size55; ++_i59)
    {
      if (!reader.readI32(controlMode[_i59])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool jointData::read_interactionMode(yarp::os::idl::WireReader& reader) {
  {
    interactionMode.clear();
    uint32_t _size60;
    yarp::os::idl::WireState _etype63;
    reader.readListBegin(_etype63, _size60);
    interactionMode.resize(_size60);
    uint32_t _i64;
    for (_i64 = 0; _i64 < _size60; ++_i64)
    {
      if (!reader.readI32(interactionMode[_i64])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool jointData::nested_read_interactionMode(yarp::os::idl::WireReader& reader) {
  {
    interactionMode.clear();
    uint32_t _size65;
    yarp::os::idl::WireState _etype68;
    reader.readListBegin(_etype68, _size65);
    interactionMode.resize(_size65);
    uint32_t _i69;
    for (_i69 = 0; _i69 < _size65; ++_i69)
    {
      if (!reader.readI32(interactionMode[_i69])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool jointData::read(yarp::os::idl::WireReader& reader) {
  if (!read_position(reader)) return false;
  if (!read_velocity(reader)) return false;
  if (!read_acceleration(reader)) return false;
  if (!read_torque(reader)) return false;
  if (!read_pidOutput(reader)) return false;
  if (!read_controlMode(reader)) return false;
  if (!read_interactionMode(reader)) return false;
  return !reader.isError();
}

bool jointData::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListHeader(7)) return false;
  return read(reader);
}

bool jointData::write_position(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_DOUBLE, static_cast<uint32_t>(position.size()))) return false;
    std::vector<double> ::iterator _iter70;
    for (_iter70 = position.begin(); _iter70 != position.end(); ++_iter70)
    {
      if (!writer.writeDouble((*_iter70))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool jointData::nested_write_position(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_DOUBLE, static_cast<uint32_t>(position.size()))) return false;
    std::vector<double> ::iterator _iter71;
    for (_iter71 = position.begin(); _iter71 != position.end(); ++_iter71)
    {
      if (!writer.writeDouble((*_iter71))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool jointData::write_velocity(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_DOUBLE, static_cast<uint32_t>(velocity.size()))) return false;
    std::vector<double> ::iterator _iter72;
    for (_iter72 = velocity.begin(); _iter72 != velocity.end(); ++_iter72)
    {
      if (!writer.writeDouble((*_iter72))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool jointData::nested_write_velocity(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_DOUBLE, static_cast<uint32_t>(velocity.size()))) return false;
    std::vector<double> ::iterator _iter73;
    for (_iter73 = velocity.begin(); _iter73 != velocity.end(); ++_iter73)
    {
      if (!writer.writeDouble((*_iter73))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool jointData::write_acceleration(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_DOUBLE, static_cast<uint32_t>(acceleration.size()))) return false;
    std::vector<double> ::iterator _iter74;
    for (_iter74 = acceleration.begin(); _iter74 != acceleration.end(); ++_iter74)
    {
      if (!writer.writeDouble((*_iter74))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool jointData::nested_write_acceleration(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_DOUBLE, static_cast<uint32_t>(acceleration.size()))) return false;
    std::vector<double> ::iterator _iter75;
    for (_iter75 = acceleration.begin(); _iter75 != acceleration.end(); ++_iter75)
    {
      if (!writer.writeDouble((*_iter75))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool jointData::write_torque(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_DOUBLE, static_cast<uint32_t>(torque.size()))) return false;
    std::vector<double> ::iterator _iter76;
    for (_iter76 = torque.begin(); _iter76 != torque.end(); ++_iter76)
    {
      if (!writer.writeDouble((*_iter76))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool jointData::nested_write_torque(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_DOUBLE, static_cast<uint32_t>(torque.size()))) return false;
    std::vector<double> ::iterator _iter77;
    for (_iter77 = torque.begin(); _iter77 != torque.end(); ++_iter77)
    {
      if (!writer.writeDouble((*_iter77))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool jointData::write_pidOutput(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_DOUBLE, static_cast<uint32_t>(pidOutput.size()))) return false;
    std::vector<double> ::iterator _iter78;
    for (_iter78 = pidOutput.begin(); _iter78 != pidOutput.end(); ++_iter78)
    {
      if (!writer.writeDouble((*_iter78))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool jointData::nested_write_pidOutput(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_DOUBLE, static_cast<uint32_t>(pidOutput.size()))) return false;
    std::vector<double> ::iterator _iter79;
    for (_iter79 = pidOutput.begin(); _iter79 != pidOutput.end(); ++_iter79)
    {
      if (!writer.writeDouble((*_iter79))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool jointData::write_controlMode(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_INT, static_cast<uint32_t>(controlMode.size()))) return false;
    std::vector<int32_t> ::iterator _iter80;
    for (_iter80 = controlMode.begin(); _iter80 != controlMode.end(); ++_iter80)
    {
      if (!writer.writeI32((*_iter80))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool jointData::nested_write_controlMode(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_INT, static_cast<uint32_t>(controlMode.size()))) return false;
    std::vector<int32_t> ::iterator _iter81;
    for (_iter81 = controlMode.begin(); _iter81 != controlMode.end(); ++_iter81)
    {
      if (!writer.writeI32((*_iter81))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool jointData::write_interactionMode(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_INT, static_cast<uint32_t>(interactionMode.size()))) return false;
    std::vector<int32_t> ::iterator _iter82;
    for (_iter82 = interactionMode.begin(); _iter82 != interactionMode.end(); ++_iter82)
    {
      if (!writer.writeI32((*_iter82))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool jointData::nested_write_interactionMode(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_INT, static_cast<uint32_t>(interactionMode.size()))) return false;
    std::vector<int32_t> ::iterator _iter83;
    for (_iter83 = interactionMode.begin(); _iter83 != interactionMode.end(); ++_iter83)
    {
      if (!writer.writeI32((*_iter83))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool jointData::write(yarp::os::idl::WireWriter& writer) {
  if (!write_position(writer)) return false;
  if (!write_velocity(writer)) return false;
  if (!write_acceleration(writer)) return false;
  if (!write_torque(writer)) return false;
  if (!write_pidOutput(writer)) return false;
  if (!write_controlMode(writer)) return false;
  if (!write_interactionMode(writer)) return false;
  return !writer.isError();
}

bool jointData::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(7)) return false;
  return write(writer);
}
bool jointData::Editor::write(yarp::os::ConnectionWriter& connection) {
  if (!isValid()) return false;
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(dirty_count+1)) return false;
  if (!writer.writeString("patch")) return false;
  if (is_dirty_position) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("position")) return false;
    if (!obj->nested_write_position(writer)) return false;
  }
  if (is_dirty_velocity) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("velocity")) return false;
    if (!obj->nested_write_velocity(writer)) return false;
  }
  if (is_dirty_acceleration) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("acceleration")) return false;
    if (!obj->nested_write_acceleration(writer)) return false;
  }
  if (is_dirty_torque) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("torque")) return false;
    if (!obj->nested_write_torque(writer)) return false;
  }
  if (is_dirty_pidOutput) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("pidOutput")) return false;
    if (!obj->nested_write_pidOutput(writer)) return false;
  }
  if (is_dirty_controlMode) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("controlMode")) return false;
    if (!obj->nested_write_controlMode(writer)) return false;
  }
  if (is_dirty_interactionMode) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("interactionMode")) return false;
    if (!obj->nested_write_interactionMode(writer)) return false;
  }
  return !writer.isError();
}
bool jointData::Editor::read(yarp::os::ConnectionReader& connection) {
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
  if (tag!="patch") {
    yarp::os::idl::WireWriter writer(reader);
    if (writer.isNull()) return true;
    if (!writer.writeListHeader(2)) return false;
    if (!writer.writeTag("many",1, 0)) return false;
    if (tag=="help" && reader.getLength()>0) {
      yarp::os::ConstString field;
      if (!reader.readString(field)) return false;
      if (field=="position") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::vector<double>  position")) return false;
      }
      if (field=="velocity") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::vector<double>  velocity")) return false;
      }
      if (field=="acceleration") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::vector<double>  acceleration")) return false;
      }
      if (field=="torque") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::vector<double>  torque")) return false;
      }
      if (field=="pidOutput") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::vector<double>  pidOutput")) return false;
      }
      if (field=="controlMode") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::vector<int32_t>  controlMode")) return false;
      }
      if (field=="interactionMode") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::vector<int32_t>  interactionMode")) return false;
      }
    }
    if (!writer.writeListHeader(8)) return false;
    writer.writeString("*** Available fields:");
    writer.writeString("position");
    writer.writeString("velocity");
    writer.writeString("acceleration");
    writer.writeString("torque");
    writer.writeString("pidOutput");
    writer.writeString("controlMode");
    writer.writeString("interactionMode");
    return true;
  }
  for (int i=1; i<len; i++) {
    if (!reader.readListHeader(3)) return false;
    yarp::os::ConstString act;
    yarp::os::ConstString key;
    if (!reader.readString(act)) return false;
    if (!reader.readString(key)) return false;
    // inefficient code follows, bug paulfitz to improve it
    if (key == "position") {
      will_set_position();
      if (!obj->nested_read_position(reader)) return false;
      did_set_position();
    } else if (key == "velocity") {
      will_set_velocity();
      if (!obj->nested_read_velocity(reader)) return false;
      did_set_velocity();
    } else if (key == "acceleration") {
      will_set_acceleration();
      if (!obj->nested_read_acceleration(reader)) return false;
      did_set_acceleration();
    } else if (key == "torque") {
      will_set_torque();
      if (!obj->nested_read_torque(reader)) return false;
      did_set_torque();
    } else if (key == "pidOutput") {
      will_set_pidOutput();
      if (!obj->nested_read_pidOutput(reader)) return false;
      did_set_pidOutput();
    } else if (key == "controlMode") {
      will_set_controlMode();
      if (!obj->nested_read_controlMode(reader)) return false;
      did_set_controlMode();
    } else if (key == "interactionMode") {
      will_set_interactionMode();
      if (!obj->nested_read_interactionMode(reader)) return false;
      did_set_interactionMode();
    } else {
      // would be useful to have a fallback here
    }
  }
  reader.accept();
  return true;
}

yarp::os::ConstString jointData::toString() {
  yarp::os::Bottle b;
  b.read(*this);
  return b.toString();
}

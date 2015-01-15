// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <jointData.h>

bool jointData::read_jointPosition(yarp::os::idl::WireReader& reader) {
  {
    jointPosition.clear();
    uint32_t _size0;
    yarp::os::idl::WireState _etype3;
    reader.readListBegin(_etype3, _size0);
    jointPosition.resize(_size0);
    uint32_t _i4;
    for (_i4 = 0; _i4 < _size0; ++_i4)
    {
      if (!reader.readDouble(jointPosition[_i4])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool jointData::nested_read_jointPosition(yarp::os::idl::WireReader& reader) {
  {
    jointPosition.clear();
    uint32_t _size5;
    yarp::os::idl::WireState _etype8;
    reader.readListBegin(_etype8, _size5);
    jointPosition.resize(_size5);
    uint32_t _i9;
    for (_i9 = 0; _i9 < _size5; ++_i9)
    {
      if (!reader.readDouble(jointPosition[_i9])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool jointData::read_jointVelocity(yarp::os::idl::WireReader& reader) {
  {
    jointVelocity.clear();
    uint32_t _size10;
    yarp::os::idl::WireState _etype13;
    reader.readListBegin(_etype13, _size10);
    jointVelocity.resize(_size10);
    uint32_t _i14;
    for (_i14 = 0; _i14 < _size10; ++_i14)
    {
      if (!reader.readDouble(jointVelocity[_i14])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool jointData::nested_read_jointVelocity(yarp::os::idl::WireReader& reader) {
  {
    jointVelocity.clear();
    uint32_t _size15;
    yarp::os::idl::WireState _etype18;
    reader.readListBegin(_etype18, _size15);
    jointVelocity.resize(_size15);
    uint32_t _i19;
    for (_i19 = 0; _i19 < _size15; ++_i19)
    {
      if (!reader.readDouble(jointVelocity[_i19])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool jointData::read_jointAcceleration(yarp::os::idl::WireReader& reader) {
  {
    jointAcceleration.clear();
    uint32_t _size20;
    yarp::os::idl::WireState _etype23;
    reader.readListBegin(_etype23, _size20);
    jointAcceleration.resize(_size20);
    uint32_t _i24;
    for (_i24 = 0; _i24 < _size20; ++_i24)
    {
      if (!reader.readDouble(jointAcceleration[_i24])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool jointData::nested_read_jointAcceleration(yarp::os::idl::WireReader& reader) {
  {
    jointAcceleration.clear();
    uint32_t _size25;
    yarp::os::idl::WireState _etype28;
    reader.readListBegin(_etype28, _size25);
    jointAcceleration.resize(_size25);
    uint32_t _i29;
    for (_i29 = 0; _i29 < _size25; ++_i29)
    {
      if (!reader.readDouble(jointAcceleration[_i29])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool jointData::read_motorPosition(yarp::os::idl::WireReader& reader) {
  {
    motorPosition.clear();
    uint32_t _size30;
    yarp::os::idl::WireState _etype33;
    reader.readListBegin(_etype33, _size30);
    motorPosition.resize(_size30);
    uint32_t _i34;
    for (_i34 = 0; _i34 < _size30; ++_i34)
    {
      if (!reader.readDouble(motorPosition[_i34])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool jointData::nested_read_motorPosition(yarp::os::idl::WireReader& reader) {
  {
    motorPosition.clear();
    uint32_t _size35;
    yarp::os::idl::WireState _etype38;
    reader.readListBegin(_etype38, _size35);
    motorPosition.resize(_size35);
    uint32_t _i39;
    for (_i39 = 0; _i39 < _size35; ++_i39)
    {
      if (!reader.readDouble(motorPosition[_i39])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool jointData::read_motorVelocity(yarp::os::idl::WireReader& reader) {
  {
    motorVelocity.clear();
    uint32_t _size40;
    yarp::os::idl::WireState _etype43;
    reader.readListBegin(_etype43, _size40);
    motorVelocity.resize(_size40);
    uint32_t _i44;
    for (_i44 = 0; _i44 < _size40; ++_i44)
    {
      if (!reader.readDouble(motorVelocity[_i44])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool jointData::nested_read_motorVelocity(yarp::os::idl::WireReader& reader) {
  {
    motorVelocity.clear();
    uint32_t _size45;
    yarp::os::idl::WireState _etype48;
    reader.readListBegin(_etype48, _size45);
    motorVelocity.resize(_size45);
    uint32_t _i49;
    for (_i49 = 0; _i49 < _size45; ++_i49)
    {
      if (!reader.readDouble(motorVelocity[_i49])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool jointData::read_motorAcceleration(yarp::os::idl::WireReader& reader) {
  {
    motorAcceleration.clear();
    uint32_t _size50;
    yarp::os::idl::WireState _etype53;
    reader.readListBegin(_etype53, _size50);
    motorAcceleration.resize(_size50);
    uint32_t _i54;
    for (_i54 = 0; _i54 < _size50; ++_i54)
    {
      if (!reader.readDouble(motorAcceleration[_i54])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool jointData::nested_read_motorAcceleration(yarp::os::idl::WireReader& reader) {
  {
    motorAcceleration.clear();
    uint32_t _size55;
    yarp::os::idl::WireState _etype58;
    reader.readListBegin(_etype58, _size55);
    motorAcceleration.resize(_size55);
    uint32_t _i59;
    for (_i59 = 0; _i59 < _size55; ++_i59)
    {
      if (!reader.readDouble(motorAcceleration[_i59])) {
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
    uint32_t _size60;
    yarp::os::idl::WireState _etype63;
    reader.readListBegin(_etype63, _size60);
    torque.resize(_size60);
    uint32_t _i64;
    for (_i64 = 0; _i64 < _size60; ++_i64)
    {
      if (!reader.readDouble(torque[_i64])) {
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
    uint32_t _size65;
    yarp::os::idl::WireState _etype68;
    reader.readListBegin(_etype68, _size65);
    torque.resize(_size65);
    uint32_t _i69;
    for (_i69 = 0; _i69 < _size65; ++_i69)
    {
      if (!reader.readDouble(torque[_i69])) {
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
    uint32_t _size70;
    yarp::os::idl::WireState _etype73;
    reader.readListBegin(_etype73, _size70);
    pidOutput.resize(_size70);
    uint32_t _i74;
    for (_i74 = 0; _i74 < _size70; ++_i74)
    {
      if (!reader.readDouble(pidOutput[_i74])) {
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
    uint32_t _size75;
    yarp::os::idl::WireState _etype78;
    reader.readListBegin(_etype78, _size75);
    pidOutput.resize(_size75);
    uint32_t _i79;
    for (_i79 = 0; _i79 < _size75; ++_i79)
    {
      if (!reader.readDouble(pidOutput[_i79])) {
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
    uint32_t _size80;
    yarp::os::idl::WireState _etype83;
    reader.readListBegin(_etype83, _size80);
    controlMode.resize(_size80);
    uint32_t _i84;
    for (_i84 = 0; _i84 < _size80; ++_i84)
    {
      if (!reader.readI32(controlMode[_i84])) {
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
    uint32_t _size85;
    yarp::os::idl::WireState _etype88;
    reader.readListBegin(_etype88, _size85);
    controlMode.resize(_size85);
    uint32_t _i89;
    for (_i89 = 0; _i89 < _size85; ++_i89)
    {
      if (!reader.readI32(controlMode[_i89])) {
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
    uint32_t _size90;
    yarp::os::idl::WireState _etype93;
    reader.readListBegin(_etype93, _size90);
    interactionMode.resize(_size90);
    uint32_t _i94;
    for (_i94 = 0; _i94 < _size90; ++_i94)
    {
      if (!reader.readI32(interactionMode[_i94])) {
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
    uint32_t _size95;
    yarp::os::idl::WireState _etype98;
    reader.readListBegin(_etype98, _size95);
    interactionMode.resize(_size95);
    uint32_t _i99;
    for (_i99 = 0; _i99 < _size95; ++_i99)
    {
      if (!reader.readI32(interactionMode[_i99])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool jointData::read(yarp::os::idl::WireReader& reader) {
  if (!read_jointPosition(reader)) return false;
  if (!read_jointVelocity(reader)) return false;
  if (!read_jointAcceleration(reader)) return false;
  if (!read_motorPosition(reader)) return false;
  if (!read_motorVelocity(reader)) return false;
  if (!read_motorAcceleration(reader)) return false;
  if (!read_torque(reader)) return false;
  if (!read_pidOutput(reader)) return false;
  if (!read_controlMode(reader)) return false;
  if (!read_interactionMode(reader)) return false;
  return !reader.isError();
}

bool jointData::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListHeader(10)) return false;
  return read(reader);
}

bool jointData::write_jointPosition(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_DOUBLE, static_cast<uint32_t>(jointPosition.size()))) return false;
    std::vector<double> ::iterator _iter100;
    for (_iter100 = jointPosition.begin(); _iter100 != jointPosition.end(); ++_iter100)
    {
      if (!writer.writeDouble((*_iter100))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool jointData::nested_write_jointPosition(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_DOUBLE, static_cast<uint32_t>(jointPosition.size()))) return false;
    std::vector<double> ::iterator _iter101;
    for (_iter101 = jointPosition.begin(); _iter101 != jointPosition.end(); ++_iter101)
    {
      if (!writer.writeDouble((*_iter101))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool jointData::write_jointVelocity(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_DOUBLE, static_cast<uint32_t>(jointVelocity.size()))) return false;
    std::vector<double> ::iterator _iter102;
    for (_iter102 = jointVelocity.begin(); _iter102 != jointVelocity.end(); ++_iter102)
    {
      if (!writer.writeDouble((*_iter102))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool jointData::nested_write_jointVelocity(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_DOUBLE, static_cast<uint32_t>(jointVelocity.size()))) return false;
    std::vector<double> ::iterator _iter103;
    for (_iter103 = jointVelocity.begin(); _iter103 != jointVelocity.end(); ++_iter103)
    {
      if (!writer.writeDouble((*_iter103))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool jointData::write_jointAcceleration(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_DOUBLE, static_cast<uint32_t>(jointAcceleration.size()))) return false;
    std::vector<double> ::iterator _iter104;
    for (_iter104 = jointAcceleration.begin(); _iter104 != jointAcceleration.end(); ++_iter104)
    {
      if (!writer.writeDouble((*_iter104))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool jointData::nested_write_jointAcceleration(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_DOUBLE, static_cast<uint32_t>(jointAcceleration.size()))) return false;
    std::vector<double> ::iterator _iter105;
    for (_iter105 = jointAcceleration.begin(); _iter105 != jointAcceleration.end(); ++_iter105)
    {
      if (!writer.writeDouble((*_iter105))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool jointData::write_motorPosition(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_DOUBLE, static_cast<uint32_t>(motorPosition.size()))) return false;
    std::vector<double> ::iterator _iter106;
    for (_iter106 = motorPosition.begin(); _iter106 != motorPosition.end(); ++_iter106)
    {
      if (!writer.writeDouble((*_iter106))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool jointData::nested_write_motorPosition(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_DOUBLE, static_cast<uint32_t>(motorPosition.size()))) return false;
    std::vector<double> ::iterator _iter107;
    for (_iter107 = motorPosition.begin(); _iter107 != motorPosition.end(); ++_iter107)
    {
      if (!writer.writeDouble((*_iter107))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool jointData::write_motorVelocity(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_DOUBLE, static_cast<uint32_t>(motorVelocity.size()))) return false;
    std::vector<double> ::iterator _iter108;
    for (_iter108 = motorVelocity.begin(); _iter108 != motorVelocity.end(); ++_iter108)
    {
      if (!writer.writeDouble((*_iter108))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool jointData::nested_write_motorVelocity(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_DOUBLE, static_cast<uint32_t>(motorVelocity.size()))) return false;
    std::vector<double> ::iterator _iter109;
    for (_iter109 = motorVelocity.begin(); _iter109 != motorVelocity.end(); ++_iter109)
    {
      if (!writer.writeDouble((*_iter109))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool jointData::write_motorAcceleration(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_DOUBLE, static_cast<uint32_t>(motorAcceleration.size()))) return false;
    std::vector<double> ::iterator _iter110;
    for (_iter110 = motorAcceleration.begin(); _iter110 != motorAcceleration.end(); ++_iter110)
    {
      if (!writer.writeDouble((*_iter110))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool jointData::nested_write_motorAcceleration(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_DOUBLE, static_cast<uint32_t>(motorAcceleration.size()))) return false;
    std::vector<double> ::iterator _iter111;
    for (_iter111 = motorAcceleration.begin(); _iter111 != motorAcceleration.end(); ++_iter111)
    {
      if (!writer.writeDouble((*_iter111))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool jointData::write_torque(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_DOUBLE, static_cast<uint32_t>(torque.size()))) return false;
    std::vector<double> ::iterator _iter112;
    for (_iter112 = torque.begin(); _iter112 != torque.end(); ++_iter112)
    {
      if (!writer.writeDouble((*_iter112))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool jointData::nested_write_torque(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_DOUBLE, static_cast<uint32_t>(torque.size()))) return false;
    std::vector<double> ::iterator _iter113;
    for (_iter113 = torque.begin(); _iter113 != torque.end(); ++_iter113)
    {
      if (!writer.writeDouble((*_iter113))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool jointData::write_pidOutput(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_DOUBLE, static_cast<uint32_t>(pidOutput.size()))) return false;
    std::vector<double> ::iterator _iter114;
    for (_iter114 = pidOutput.begin(); _iter114 != pidOutput.end(); ++_iter114)
    {
      if (!writer.writeDouble((*_iter114))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool jointData::nested_write_pidOutput(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_DOUBLE, static_cast<uint32_t>(pidOutput.size()))) return false;
    std::vector<double> ::iterator _iter115;
    for (_iter115 = pidOutput.begin(); _iter115 != pidOutput.end(); ++_iter115)
    {
      if (!writer.writeDouble((*_iter115))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool jointData::write_controlMode(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_INT, static_cast<uint32_t>(controlMode.size()))) return false;
    std::vector<int32_t> ::iterator _iter116;
    for (_iter116 = controlMode.begin(); _iter116 != controlMode.end(); ++_iter116)
    {
      if (!writer.writeI32((*_iter116))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool jointData::nested_write_controlMode(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_INT, static_cast<uint32_t>(controlMode.size()))) return false;
    std::vector<int32_t> ::iterator _iter117;
    for (_iter117 = controlMode.begin(); _iter117 != controlMode.end(); ++_iter117)
    {
      if (!writer.writeI32((*_iter117))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool jointData::write_interactionMode(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_INT, static_cast<uint32_t>(interactionMode.size()))) return false;
    std::vector<int32_t> ::iterator _iter118;
    for (_iter118 = interactionMode.begin(); _iter118 != interactionMode.end(); ++_iter118)
    {
      if (!writer.writeI32((*_iter118))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool jointData::nested_write_interactionMode(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_INT, static_cast<uint32_t>(interactionMode.size()))) return false;
    std::vector<int32_t> ::iterator _iter119;
    for (_iter119 = interactionMode.begin(); _iter119 != interactionMode.end(); ++_iter119)
    {
      if (!writer.writeI32((*_iter119))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool jointData::write(yarp::os::idl::WireWriter& writer) {
  if (!write_jointPosition(writer)) return false;
  if (!write_jointVelocity(writer)) return false;
  if (!write_jointAcceleration(writer)) return false;
  if (!write_motorPosition(writer)) return false;
  if (!write_motorVelocity(writer)) return false;
  if (!write_motorAcceleration(writer)) return false;
  if (!write_torque(writer)) return false;
  if (!write_pidOutput(writer)) return false;
  if (!write_controlMode(writer)) return false;
  if (!write_interactionMode(writer)) return false;
  return !writer.isError();
}

bool jointData::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(10)) return false;
  return write(writer);
}
bool jointData::Editor::write(yarp::os::ConnectionWriter& connection) {
  if (!isValid()) return false;
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(dirty_count+1)) return false;
  if (!writer.writeString("patch")) return false;
  if (is_dirty_jointPosition) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("jointPosition")) return false;
    if (!obj->nested_write_jointPosition(writer)) return false;
  }
  if (is_dirty_jointVelocity) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("jointVelocity")) return false;
    if (!obj->nested_write_jointVelocity(writer)) return false;
  }
  if (is_dirty_jointAcceleration) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("jointAcceleration")) return false;
    if (!obj->nested_write_jointAcceleration(writer)) return false;
  }
  if (is_dirty_motorPosition) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("motorPosition")) return false;
    if (!obj->nested_write_motorPosition(writer)) return false;
  }
  if (is_dirty_motorVelocity) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("motorVelocity")) return false;
    if (!obj->nested_write_motorVelocity(writer)) return false;
  }
  if (is_dirty_motorAcceleration) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("motorAcceleration")) return false;
    if (!obj->nested_write_motorAcceleration(writer)) return false;
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
  if (tag=="help") {
    yarp::os::idl::WireWriter writer(reader);
    if (writer.isNull()) return true;
    if (!writer.writeListHeader(2)) return false;
    if (!writer.writeTag("many",1, 0)) return false;
    if (reader.getLength()>0) {
      yarp::os::ConstString field;
      if (!reader.readString(field)) return false;
      if (field=="jointPosition") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::vector<double>  jointPosition")) return false;
      }
      if (field=="jointVelocity") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::vector<double>  jointVelocity")) return false;
      }
      if (field=="jointAcceleration") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::vector<double>  jointAcceleration")) return false;
      }
      if (field=="motorPosition") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::vector<double>  motorPosition")) return false;
      }
      if (field=="motorVelocity") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::vector<double>  motorVelocity")) return false;
      }
      if (field=="motorAcceleration") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::vector<double>  motorAcceleration")) return false;
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
    if (!writer.writeListHeader(11)) return false;
    writer.writeString("*** Available fields:");
    writer.writeString("jointPosition");
    writer.writeString("jointVelocity");
    writer.writeString("jointAcceleration");
    writer.writeString("motorPosition");
    writer.writeString("motorVelocity");
    writer.writeString("motorAcceleration");
    writer.writeString("torque");
    writer.writeString("pidOutput");
    writer.writeString("controlMode");
    writer.writeString("interactionMode");
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
    if (key == "jointPosition") {
      will_set_jointPosition();
      if (!obj->nested_read_jointPosition(reader)) return false;
      did_set_jointPosition();
    } else if (key == "jointVelocity") {
      will_set_jointVelocity();
      if (!obj->nested_read_jointVelocity(reader)) return false;
      did_set_jointVelocity();
    } else if (key == "jointAcceleration") {
      will_set_jointAcceleration();
      if (!obj->nested_read_jointAcceleration(reader)) return false;
      did_set_jointAcceleration();
    } else if (key == "motorPosition") {
      will_set_motorPosition();
      if (!obj->nested_read_motorPosition(reader)) return false;
      did_set_motorPosition();
    } else if (key == "motorVelocity") {
      will_set_motorVelocity();
      if (!obj->nested_read_motorVelocity(reader)) return false;
      did_set_motorVelocity();
    } else if (key == "motorAcceleration") {
      will_set_motorAcceleration();
      if (!obj->nested_read_motorAcceleration(reader)) return false;
      did_set_motorAcceleration();
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
  yarp::os::idl::WireWriter writer(reader);
  if (writer.isNull()) return true;
  writer.writeListHeader(1);
  writer.writeVocab(VOCAB2('o','k'));
  return true;
}

yarp::os::ConstString jointData::toString() {
  yarp::os::Bottle b;
  b.read(*this);
  return b.toString();
}

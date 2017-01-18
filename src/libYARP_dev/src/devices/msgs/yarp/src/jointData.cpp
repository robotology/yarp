// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <jointData.h>

bool jointData::read_jointPosition(yarp::os::idl::WireReader& reader) {
  if (!reader.read(jointPosition)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::nested_read_jointPosition(yarp::os::idl::WireReader& reader) {
  if (!reader.readNested(jointPosition)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::read_jointPosition_isValid(yarp::os::idl::WireReader& reader) {
  if (!reader.readBool(jointPosition_isValid)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::nested_read_jointPosition_isValid(yarp::os::idl::WireReader& reader) {
  if (!reader.readBool(jointPosition_isValid)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::read_jointVelocity(yarp::os::idl::WireReader& reader) {
  if (!reader.read(jointVelocity)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::nested_read_jointVelocity(yarp::os::idl::WireReader& reader) {
  if (!reader.readNested(jointVelocity)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::read_jointVelocity_isValid(yarp::os::idl::WireReader& reader) {
  if (!reader.readBool(jointVelocity_isValid)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::nested_read_jointVelocity_isValid(yarp::os::idl::WireReader& reader) {
  if (!reader.readBool(jointVelocity_isValid)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::read_jointAcceleration(yarp::os::idl::WireReader& reader) {
  if (!reader.read(jointAcceleration)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::nested_read_jointAcceleration(yarp::os::idl::WireReader& reader) {
  if (!reader.readNested(jointAcceleration)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::read_jointAcceleration_isValid(yarp::os::idl::WireReader& reader) {
  if (!reader.readBool(jointAcceleration_isValid)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::nested_read_jointAcceleration_isValid(yarp::os::idl::WireReader& reader) {
  if (!reader.readBool(jointAcceleration_isValid)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::read_motorPosition(yarp::os::idl::WireReader& reader) {
  if (!reader.read(motorPosition)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::nested_read_motorPosition(yarp::os::idl::WireReader& reader) {
  if (!reader.readNested(motorPosition)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::read_motorPosition_isValid(yarp::os::idl::WireReader& reader) {
  if (!reader.readBool(motorPosition_isValid)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::nested_read_motorPosition_isValid(yarp::os::idl::WireReader& reader) {
  if (!reader.readBool(motorPosition_isValid)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::read_motorVelocity(yarp::os::idl::WireReader& reader) {
  if (!reader.read(motorVelocity)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::nested_read_motorVelocity(yarp::os::idl::WireReader& reader) {
  if (!reader.readNested(motorVelocity)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::read_motorVelocity_isValid(yarp::os::idl::WireReader& reader) {
  if (!reader.readBool(motorVelocity_isValid)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::nested_read_motorVelocity_isValid(yarp::os::idl::WireReader& reader) {
  if (!reader.readBool(motorVelocity_isValid)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::read_motorAcceleration(yarp::os::idl::WireReader& reader) {
  if (!reader.read(motorAcceleration)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::nested_read_motorAcceleration(yarp::os::idl::WireReader& reader) {
  if (!reader.readNested(motorAcceleration)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::read_motorAcceleration_isValid(yarp::os::idl::WireReader& reader) {
  if (!reader.readBool(motorAcceleration_isValid)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::nested_read_motorAcceleration_isValid(yarp::os::idl::WireReader& reader) {
  if (!reader.readBool(motorAcceleration_isValid)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::read_torque(yarp::os::idl::WireReader& reader) {
  if (!reader.read(torque)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::nested_read_torque(yarp::os::idl::WireReader& reader) {
  if (!reader.readNested(torque)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::read_torque_isValid(yarp::os::idl::WireReader& reader) {
  if (!reader.readBool(torque_isValid)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::nested_read_torque_isValid(yarp::os::idl::WireReader& reader) {
  if (!reader.readBool(torque_isValid)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::read_pidOutput(yarp::os::idl::WireReader& reader) {
  if (!reader.read(pidOutput)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::nested_read_pidOutput(yarp::os::idl::WireReader& reader) {
  if (!reader.readNested(pidOutput)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::read_pidOutput_isValid(yarp::os::idl::WireReader& reader) {
  if (!reader.readBool(pidOutput_isValid)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::nested_read_pidOutput_isValid(yarp::os::idl::WireReader& reader) {
  if (!reader.readBool(pidOutput_isValid)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::read_controlMode(yarp::os::idl::WireReader& reader) {
  if (!reader.read(controlMode)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::nested_read_controlMode(yarp::os::idl::WireReader& reader) {
  if (!reader.readNested(controlMode)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::read_controlMode_isValid(yarp::os::idl::WireReader& reader) {
  if (!reader.readBool(controlMode_isValid)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::nested_read_controlMode_isValid(yarp::os::idl::WireReader& reader) {
  if (!reader.readBool(controlMode_isValid)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::read_interactionMode(yarp::os::idl::WireReader& reader) {
  if (!reader.read(interactionMode)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::nested_read_interactionMode(yarp::os::idl::WireReader& reader) {
  if (!reader.readNested(interactionMode)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::read_interactionMode_isValid(yarp::os::idl::WireReader& reader) {
  if (!reader.readBool(interactionMode_isValid)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::nested_read_interactionMode_isValid(yarp::os::idl::WireReader& reader) {
  if (!reader.readBool(interactionMode_isValid)) {
    reader.fail();
    return false;
  }
  return true;
}
bool jointData::read(yarp::os::idl::WireReader& reader) {
  if (!read_jointPosition(reader)) return false;
  if (!read_jointPosition_isValid(reader)) return false;
  if (!read_jointVelocity(reader)) return false;
  if (!read_jointVelocity_isValid(reader)) return false;
  if (!read_jointAcceleration(reader)) return false;
  if (!read_jointAcceleration_isValid(reader)) return false;
  if (!read_motorPosition(reader)) return false;
  if (!read_motorPosition_isValid(reader)) return false;
  if (!read_motorVelocity(reader)) return false;
  if (!read_motorVelocity_isValid(reader)) return false;
  if (!read_motorAcceleration(reader)) return false;
  if (!read_motorAcceleration_isValid(reader)) return false;
  if (!read_torque(reader)) return false;
  if (!read_torque_isValid(reader)) return false;
  if (!read_pidOutput(reader)) return false;
  if (!read_pidOutput_isValid(reader)) return false;
  if (!read_controlMode(reader)) return false;
  if (!read_controlMode_isValid(reader)) return false;
  if (!read_interactionMode(reader)) return false;
  if (!read_interactionMode_isValid(reader)) return false;
  return !reader.isError();
}

bool jointData::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListHeader(20)) return false;
  return read(reader);
}

bool jointData::write_jointPosition(yarp::os::idl::WireWriter& writer) {
  if (!writer.write(jointPosition)) return false;
  return true;
}
bool jointData::nested_write_jointPosition(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeNested(jointPosition)) return false;
  return true;
}
bool jointData::write_jointPosition_isValid(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeBool(jointPosition_isValid)) return false;
  return true;
}
bool jointData::nested_write_jointPosition_isValid(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeBool(jointPosition_isValid)) return false;
  return true;
}
bool jointData::write_jointVelocity(yarp::os::idl::WireWriter& writer) {
  if (!writer.write(jointVelocity)) return false;
  return true;
}
bool jointData::nested_write_jointVelocity(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeNested(jointVelocity)) return false;
  return true;
}
bool jointData::write_jointVelocity_isValid(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeBool(jointVelocity_isValid)) return false;
  return true;
}
bool jointData::nested_write_jointVelocity_isValid(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeBool(jointVelocity_isValid)) return false;
  return true;
}
bool jointData::write_jointAcceleration(yarp::os::idl::WireWriter& writer) {
  if (!writer.write(jointAcceleration)) return false;
  return true;
}
bool jointData::nested_write_jointAcceleration(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeNested(jointAcceleration)) return false;
  return true;
}
bool jointData::write_jointAcceleration_isValid(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeBool(jointAcceleration_isValid)) return false;
  return true;
}
bool jointData::nested_write_jointAcceleration_isValid(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeBool(jointAcceleration_isValid)) return false;
  return true;
}
bool jointData::write_motorPosition(yarp::os::idl::WireWriter& writer) {
  if (!writer.write(motorPosition)) return false;
  return true;
}
bool jointData::nested_write_motorPosition(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeNested(motorPosition)) return false;
  return true;
}
bool jointData::write_motorPosition_isValid(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeBool(motorPosition_isValid)) return false;
  return true;
}
bool jointData::nested_write_motorPosition_isValid(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeBool(motorPosition_isValid)) return false;
  return true;
}
bool jointData::write_motorVelocity(yarp::os::idl::WireWriter& writer) {
  if (!writer.write(motorVelocity)) return false;
  return true;
}
bool jointData::nested_write_motorVelocity(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeNested(motorVelocity)) return false;
  return true;
}
bool jointData::write_motorVelocity_isValid(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeBool(motorVelocity_isValid)) return false;
  return true;
}
bool jointData::nested_write_motorVelocity_isValid(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeBool(motorVelocity_isValid)) return false;
  return true;
}
bool jointData::write_motorAcceleration(yarp::os::idl::WireWriter& writer) {
  if (!writer.write(motorAcceleration)) return false;
  return true;
}
bool jointData::nested_write_motorAcceleration(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeNested(motorAcceleration)) return false;
  return true;
}
bool jointData::write_motorAcceleration_isValid(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeBool(motorAcceleration_isValid)) return false;
  return true;
}
bool jointData::nested_write_motorAcceleration_isValid(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeBool(motorAcceleration_isValid)) return false;
  return true;
}
bool jointData::write_torque(yarp::os::idl::WireWriter& writer) {
  if (!writer.write(torque)) return false;
  return true;
}
bool jointData::nested_write_torque(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeNested(torque)) return false;
  return true;
}
bool jointData::write_torque_isValid(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeBool(torque_isValid)) return false;
  return true;
}
bool jointData::nested_write_torque_isValid(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeBool(torque_isValid)) return false;
  return true;
}
bool jointData::write_pidOutput(yarp::os::idl::WireWriter& writer) {
  if (!writer.write(pidOutput)) return false;
  return true;
}
bool jointData::nested_write_pidOutput(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeNested(pidOutput)) return false;
  return true;
}
bool jointData::write_pidOutput_isValid(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeBool(pidOutput_isValid)) return false;
  return true;
}
bool jointData::nested_write_pidOutput_isValid(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeBool(pidOutput_isValid)) return false;
  return true;
}
bool jointData::write_controlMode(yarp::os::idl::WireWriter& writer) {
  if (!writer.write(controlMode)) return false;
  return true;
}
bool jointData::nested_write_controlMode(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeNested(controlMode)) return false;
  return true;
}
bool jointData::write_controlMode_isValid(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeBool(controlMode_isValid)) return false;
  return true;
}
bool jointData::nested_write_controlMode_isValid(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeBool(controlMode_isValid)) return false;
  return true;
}
bool jointData::write_interactionMode(yarp::os::idl::WireWriter& writer) {
  if (!writer.write(interactionMode)) return false;
  return true;
}
bool jointData::nested_write_interactionMode(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeNested(interactionMode)) return false;
  return true;
}
bool jointData::write_interactionMode_isValid(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeBool(interactionMode_isValid)) return false;
  return true;
}
bool jointData::nested_write_interactionMode_isValid(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeBool(interactionMode_isValid)) return false;
  return true;
}
bool jointData::write(yarp::os::idl::WireWriter& writer) {
  if (!write_jointPosition(writer)) return false;
  if (!write_jointPosition_isValid(writer)) return false;
  if (!write_jointVelocity(writer)) return false;
  if (!write_jointVelocity_isValid(writer)) return false;
  if (!write_jointAcceleration(writer)) return false;
  if (!write_jointAcceleration_isValid(writer)) return false;
  if (!write_motorPosition(writer)) return false;
  if (!write_motorPosition_isValid(writer)) return false;
  if (!write_motorVelocity(writer)) return false;
  if (!write_motorVelocity_isValid(writer)) return false;
  if (!write_motorAcceleration(writer)) return false;
  if (!write_motorAcceleration_isValid(writer)) return false;
  if (!write_torque(writer)) return false;
  if (!write_torque_isValid(writer)) return false;
  if (!write_pidOutput(writer)) return false;
  if (!write_pidOutput_isValid(writer)) return false;
  if (!write_controlMode(writer)) return false;
  if (!write_controlMode_isValid(writer)) return false;
  if (!write_interactionMode(writer)) return false;
  if (!write_interactionMode_isValid(writer)) return false;
  return !writer.isError();
}

bool jointData::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(20)) return false;
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
  if (is_dirty_jointPosition_isValid) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("jointPosition_isValid")) return false;
    if (!obj->nested_write_jointPosition_isValid(writer)) return false;
  }
  if (is_dirty_jointVelocity) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("jointVelocity")) return false;
    if (!obj->nested_write_jointVelocity(writer)) return false;
  }
  if (is_dirty_jointVelocity_isValid) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("jointVelocity_isValid")) return false;
    if (!obj->nested_write_jointVelocity_isValid(writer)) return false;
  }
  if (is_dirty_jointAcceleration) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("jointAcceleration")) return false;
    if (!obj->nested_write_jointAcceleration(writer)) return false;
  }
  if (is_dirty_jointAcceleration_isValid) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("jointAcceleration_isValid")) return false;
    if (!obj->nested_write_jointAcceleration_isValid(writer)) return false;
  }
  if (is_dirty_motorPosition) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("motorPosition")) return false;
    if (!obj->nested_write_motorPosition(writer)) return false;
  }
  if (is_dirty_motorPosition_isValid) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("motorPosition_isValid")) return false;
    if (!obj->nested_write_motorPosition_isValid(writer)) return false;
  }
  if (is_dirty_motorVelocity) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("motorVelocity")) return false;
    if (!obj->nested_write_motorVelocity(writer)) return false;
  }
  if (is_dirty_motorVelocity_isValid) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("motorVelocity_isValid")) return false;
    if (!obj->nested_write_motorVelocity_isValid(writer)) return false;
  }
  if (is_dirty_motorAcceleration) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("motorAcceleration")) return false;
    if (!obj->nested_write_motorAcceleration(writer)) return false;
  }
  if (is_dirty_motorAcceleration_isValid) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("motorAcceleration_isValid")) return false;
    if (!obj->nested_write_motorAcceleration_isValid(writer)) return false;
  }
  if (is_dirty_torque) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("torque")) return false;
    if (!obj->nested_write_torque(writer)) return false;
  }
  if (is_dirty_torque_isValid) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("torque_isValid")) return false;
    if (!obj->nested_write_torque_isValid(writer)) return false;
  }
  if (is_dirty_pidOutput) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("pidOutput")) return false;
    if (!obj->nested_write_pidOutput(writer)) return false;
  }
  if (is_dirty_pidOutput_isValid) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("pidOutput_isValid")) return false;
    if (!obj->nested_write_pidOutput_isValid(writer)) return false;
  }
  if (is_dirty_controlMode) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("controlMode")) return false;
    if (!obj->nested_write_controlMode(writer)) return false;
  }
  if (is_dirty_controlMode_isValid) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("controlMode_isValid")) return false;
    if (!obj->nested_write_controlMode_isValid(writer)) return false;
  }
  if (is_dirty_interactionMode) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("interactionMode")) return false;
    if (!obj->nested_write_interactionMode(writer)) return false;
  }
  if (is_dirty_interactionMode_isValid) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("interactionMode_isValid")) return false;
    if (!obj->nested_write_interactionMode_isValid(writer)) return false;
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
        if (!writer.writeString("yarp::sig::VectorOf<double> jointPosition")) return false;
      }
      if (field=="jointPosition_isValid") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("bool jointPosition_isValid")) return false;
      }
      if (field=="jointVelocity") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("yarp::sig::VectorOf<double> jointVelocity")) return false;
      }
      if (field=="jointVelocity_isValid") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("bool jointVelocity_isValid")) return false;
      }
      if (field=="jointAcceleration") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("yarp::sig::VectorOf<double> jointAcceleration")) return false;
      }
      if (field=="jointAcceleration_isValid") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("bool jointAcceleration_isValid")) return false;
      }
      if (field=="motorPosition") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("yarp::sig::VectorOf<double> motorPosition")) return false;
      }
      if (field=="motorPosition_isValid") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("bool motorPosition_isValid")) return false;
      }
      if (field=="motorVelocity") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("yarp::sig::VectorOf<double> motorVelocity")) return false;
      }
      if (field=="motorVelocity_isValid") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("bool motorVelocity_isValid")) return false;
      }
      if (field=="motorAcceleration") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("yarp::sig::VectorOf<double> motorAcceleration")) return false;
      }
      if (field=="motorAcceleration_isValid") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("bool motorAcceleration_isValid")) return false;
      }
      if (field=="torque") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("yarp::sig::VectorOf<double> torque")) return false;
      }
      if (field=="torque_isValid") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("bool torque_isValid")) return false;
      }
      if (field=="pidOutput") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("yarp::sig::VectorOf<double> pidOutput")) return false;
      }
      if (field=="pidOutput_isValid") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("bool pidOutput_isValid")) return false;
      }
      if (field=="controlMode") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("yarp::sig::VectorOf<int> controlMode")) return false;
      }
      if (field=="controlMode_isValid") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("bool controlMode_isValid")) return false;
      }
      if (field=="interactionMode") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("yarp::sig::VectorOf<int> interactionMode")) return false;
      }
      if (field=="interactionMode_isValid") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("bool interactionMode_isValid")) return false;
      }
    }
    if (!writer.writeListHeader(21)) return false;
    writer.writeString("*** Available fields:");
    writer.writeString("jointPosition");
    writer.writeString("jointPosition_isValid");
    writer.writeString("jointVelocity");
    writer.writeString("jointVelocity_isValid");
    writer.writeString("jointAcceleration");
    writer.writeString("jointAcceleration_isValid");
    writer.writeString("motorPosition");
    writer.writeString("motorPosition_isValid");
    writer.writeString("motorVelocity");
    writer.writeString("motorVelocity_isValid");
    writer.writeString("motorAcceleration");
    writer.writeString("motorAcceleration_isValid");
    writer.writeString("torque");
    writer.writeString("torque_isValid");
    writer.writeString("pidOutput");
    writer.writeString("pidOutput_isValid");
    writer.writeString("controlMode");
    writer.writeString("controlMode_isValid");
    writer.writeString("interactionMode");
    writer.writeString("interactionMode_isValid");
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
    } else if (key == "jointPosition_isValid") {
      will_set_jointPosition_isValid();
      if (!obj->nested_read_jointPosition_isValid(reader)) return false;
      did_set_jointPosition_isValid();
    } else if (key == "jointVelocity") {
      will_set_jointVelocity();
      if (!obj->nested_read_jointVelocity(reader)) return false;
      did_set_jointVelocity();
    } else if (key == "jointVelocity_isValid") {
      will_set_jointVelocity_isValid();
      if (!obj->nested_read_jointVelocity_isValid(reader)) return false;
      did_set_jointVelocity_isValid();
    } else if (key == "jointAcceleration") {
      will_set_jointAcceleration();
      if (!obj->nested_read_jointAcceleration(reader)) return false;
      did_set_jointAcceleration();
    } else if (key == "jointAcceleration_isValid") {
      will_set_jointAcceleration_isValid();
      if (!obj->nested_read_jointAcceleration_isValid(reader)) return false;
      did_set_jointAcceleration_isValid();
    } else if (key == "motorPosition") {
      will_set_motorPosition();
      if (!obj->nested_read_motorPosition(reader)) return false;
      did_set_motorPosition();
    } else if (key == "motorPosition_isValid") {
      will_set_motorPosition_isValid();
      if (!obj->nested_read_motorPosition_isValid(reader)) return false;
      did_set_motorPosition_isValid();
    } else if (key == "motorVelocity") {
      will_set_motorVelocity();
      if (!obj->nested_read_motorVelocity(reader)) return false;
      did_set_motorVelocity();
    } else if (key == "motorVelocity_isValid") {
      will_set_motorVelocity_isValid();
      if (!obj->nested_read_motorVelocity_isValid(reader)) return false;
      did_set_motorVelocity_isValid();
    } else if (key == "motorAcceleration") {
      will_set_motorAcceleration();
      if (!obj->nested_read_motorAcceleration(reader)) return false;
      did_set_motorAcceleration();
    } else if (key == "motorAcceleration_isValid") {
      will_set_motorAcceleration_isValid();
      if (!obj->nested_read_motorAcceleration_isValid(reader)) return false;
      did_set_motorAcceleration_isValid();
    } else if (key == "torque") {
      will_set_torque();
      if (!obj->nested_read_torque(reader)) return false;
      did_set_torque();
    } else if (key == "torque_isValid") {
      will_set_torque_isValid();
      if (!obj->nested_read_torque_isValid(reader)) return false;
      did_set_torque_isValid();
    } else if (key == "pidOutput") {
      will_set_pidOutput();
      if (!obj->nested_read_pidOutput(reader)) return false;
      did_set_pidOutput();
    } else if (key == "pidOutput_isValid") {
      will_set_pidOutput_isValid();
      if (!obj->nested_read_pidOutput_isValid(reader)) return false;
      did_set_pidOutput_isValid();
    } else if (key == "controlMode") {
      will_set_controlMode();
      if (!obj->nested_read_controlMode(reader)) return false;
      did_set_controlMode();
    } else if (key == "controlMode_isValid") {
      will_set_controlMode_isValid();
      if (!obj->nested_read_controlMode_isValid(reader)) return false;
      did_set_controlMode_isValid();
    } else if (key == "interactionMode") {
      will_set_interactionMode();
      if (!obj->nested_read_interactionMode(reader)) return false;
      did_set_interactionMode();
    } else if (key == "interactionMode_isValid") {
      will_set_interactionMode_isValid();
      if (!obj->nested_read_interactionMode_isValid(reader)) return false;
      did_set_interactionMode_isValid();
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

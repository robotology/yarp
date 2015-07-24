// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <Box3D.h>

bool Box3D::read_corners(yarp::os::idl::WireReader& reader) {
  {
    corners.clear();
    uint32_t _size48;
    yarp::os::idl::WireState _etype51;
    reader.readListBegin(_etype51, _size48);
    corners.resize(_size48);
    uint32_t _i52;
    for (_i52 = 0; _i52 < _size48; ++_i52)
    {
      if (!reader.readNested(corners[_i52])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool Box3D::nested_read_corners(yarp::os::idl::WireReader& reader) {
  {
    corners.clear();
    uint32_t _size53;
    yarp::os::idl::WireState _etype56;
    reader.readListBegin(_etype56, _size53);
    corners.resize(_size53);
    uint32_t _i57;
    for (_i57 = 0; _i57 < _size53; ++_i57)
    {
      if (!reader.readNested(corners[_i57])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool Box3D::read_orientation(yarp::os::idl::WireReader& reader) {
  if (!reader.read(orientation)) {
    reader.fail();
    return false;
  }
  return true;
}
bool Box3D::nested_read_orientation(yarp::os::idl::WireReader& reader) {
  if (!reader.readNested(orientation)) {
    reader.fail();
    return false;
  }
  return true;
}
bool Box3D::read(yarp::os::idl::WireReader& reader) {
  if (!read_corners(reader)) return false;
  if (!read_orientation(reader)) return false;
  return !reader.isError();
}

bool Box3D::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListHeader(2)) return false;
  return read(reader);
}

bool Box3D::write_corners(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(corners.size()))) return false;
    std::vector<PointXYZ> ::iterator _iter58;
    for (_iter58 = corners.begin(); _iter58 != corners.end(); ++_iter58)
    {
      if (!writer.writeNested((*_iter58))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool Box3D::nested_write_corners(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(corners.size()))) return false;
    std::vector<PointXYZ> ::iterator _iter59;
    for (_iter59 = corners.begin(); _iter59 != corners.end(); ++_iter59)
    {
      if (!writer.writeNested((*_iter59))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool Box3D::write_orientation(yarp::os::idl::WireWriter& writer) {
  if (!writer.write(orientation)) return false;
  return true;
}
bool Box3D::nested_write_orientation(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeNested(orientation)) return false;
  return true;
}
bool Box3D::write(yarp::os::idl::WireWriter& writer) {
  if (!write_corners(writer)) return false;
  if (!write_orientation(writer)) return false;
  return !writer.isError();
}

bool Box3D::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(2)) return false;
  return write(writer);
}
bool Box3D::Editor::write(yarp::os::ConnectionWriter& connection) {
  if (!isValid()) return false;
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(dirty_count+1)) return false;
  if (!writer.writeString("patch")) return false;
  if (is_dirty_corners) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("corners")) return false;
    if (!obj->nested_write_corners(writer)) return false;
  }
  if (is_dirty_orientation) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("orientation")) return false;
    if (!obj->nested_write_orientation(writer)) return false;
  }
  return !writer.isError();
}
bool Box3D::Editor::read(yarp::os::ConnectionReader& connection) {
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
      if (field=="corners") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::vector<PointXYZ>  corners")) return false;
      }
      if (field=="orientation") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("yarp::sig::Matrix orientation")) return false;
      }
    }
    if (!writer.writeListHeader(3)) return false;
    writer.writeString("*** Available fields:");
    writer.writeString("corners");
    writer.writeString("orientation");
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
    if (key == "corners") {
      will_set_corners();
      if (!obj->nested_read_corners(reader)) return false;
      did_set_corners();
    } else if (key == "orientation") {
      will_set_orientation();
      if (!obj->nested_read_orientation(reader)) return false;
      did_set_orientation();
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

yarp::os::ConstString Box3D::toString() {
  yarp::os::Bottle b;
  b.read(*this);
  return b.toString();
}

// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <SurfaceMeshWithBoundingBox.h>

bool SurfaceMeshWithBoundingBox::read_mesh(yarp::os::idl::WireReader& reader) {
  if (!reader.read(mesh)) {
    reader.fail();
    return false;
  }
  return true;
}
bool SurfaceMeshWithBoundingBox::nested_read_mesh(yarp::os::idl::WireReader& reader) {
  if (!reader.readNested(mesh)) {
    reader.fail();
    return false;
  }
  return true;
}
bool SurfaceMeshWithBoundingBox::read_boundingBox(yarp::os::idl::WireReader& reader) {
  if (!reader.read(boundingBox)) {
    reader.fail();
    return false;
  }
  return true;
}
bool SurfaceMeshWithBoundingBox::nested_read_boundingBox(yarp::os::idl::WireReader& reader) {
  if (!reader.readNested(boundingBox)) {
    reader.fail();
    return false;
  }
  return true;
}
bool SurfaceMeshWithBoundingBox::read(yarp::os::idl::WireReader& reader) {
  if (!read_mesh(reader)) return false;
  if (!read_boundingBox(reader)) return false;
  return !reader.isError();
}

bool SurfaceMeshWithBoundingBox::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListHeader(6)) return false;
  return read(reader);
}

bool SurfaceMeshWithBoundingBox::write_mesh(yarp::os::idl::WireWriter& writer) {
  if (!writer.write(mesh)) return false;
  return true;
}
bool SurfaceMeshWithBoundingBox::nested_write_mesh(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeNested(mesh)) return false;
  return true;
}
bool SurfaceMeshWithBoundingBox::write_boundingBox(yarp::os::idl::WireWriter& writer) {
  if (!writer.write(boundingBox)) return false;
  return true;
}
bool SurfaceMeshWithBoundingBox::nested_write_boundingBox(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeNested(boundingBox)) return false;
  return true;
}
bool SurfaceMeshWithBoundingBox::write(yarp::os::idl::WireWriter& writer) {
  if (!write_mesh(writer)) return false;
  if (!write_boundingBox(writer)) return false;
  return !writer.isError();
}

bool SurfaceMeshWithBoundingBox::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(6)) return false;
  return write(writer);
}
bool SurfaceMeshWithBoundingBox::Editor::write(yarp::os::ConnectionWriter& connection) {
  if (!isValid()) return false;
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(dirty_count+1)) return false;
  if (!writer.writeString("patch")) return false;
  if (is_dirty_mesh) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("mesh")) return false;
    if (!obj->nested_write_mesh(writer)) return false;
  }
  if (is_dirty_boundingBox) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("boundingBox")) return false;
    if (!obj->nested_write_boundingBox(writer)) return false;
  }
  return !writer.isError();
}
bool SurfaceMeshWithBoundingBox::Editor::read(yarp::os::ConnectionReader& connection) {
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
      if (field=="mesh") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("SurfaceMesh mesh")) return false;
      }
      if (field=="boundingBox") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("Box3D boundingBox")) return false;
      }
    }
    if (!writer.writeListHeader(3)) return false;
    writer.writeString("*** Available fields:");
    writer.writeString("mesh");
    writer.writeString("boundingBox");
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
    if (key == "mesh") {
      will_set_mesh();
      if (!obj->nested_read_mesh(reader)) return false;
      did_set_mesh();
    } else if (key == "boundingBox") {
      will_set_boundingBox();
      if (!obj->nested_read_boundingBox(reader)) return false;
      did_set_boundingBox();
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

yarp::os::ConstString SurfaceMeshWithBoundingBox::toString() {
  yarp::os::Bottle b;
  b.read(*this);
  return b.toString();
}

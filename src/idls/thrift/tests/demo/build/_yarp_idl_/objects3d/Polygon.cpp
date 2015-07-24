// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <Polygon.h>

bool Polygon::read_vertices(yarp::os::idl::WireReader& reader) {
  {
    vertices.clear();
    uint32_t _size0;
    yarp::os::idl::WireState _etype3;
    reader.readListBegin(_etype3, _size0);
    vertices.resize(_size0);
    uint32_t _i4;
    for (_i4 = 0; _i4 < _size0; ++_i4)
    {
      if (!reader.readI32(vertices[_i4])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool Polygon::nested_read_vertices(yarp::os::idl::WireReader& reader) {
  {
    vertices.clear();
    uint32_t _size5;
    yarp::os::idl::WireState _etype8;
    reader.readListBegin(_etype8, _size5);
    vertices.resize(_size5);
    uint32_t _i9;
    for (_i9 = 0; _i9 < _size5; ++_i9)
    {
      if (!reader.readI32(vertices[_i9])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool Polygon::read(yarp::os::idl::WireReader& reader) {
  if (!read_vertices(reader)) return false;
  return !reader.isError();
}

bool Polygon::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListHeader(1)) return false;
  return read(reader);
}

bool Polygon::write_vertices(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_INT, static_cast<uint32_t>(vertices.size()))) return false;
    std::vector<int32_t> ::iterator _iter10;
    for (_iter10 = vertices.begin(); _iter10 != vertices.end(); ++_iter10)
    {
      if (!writer.writeI32((*_iter10))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool Polygon::nested_write_vertices(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_INT, static_cast<uint32_t>(vertices.size()))) return false;
    std::vector<int32_t> ::iterator _iter11;
    for (_iter11 = vertices.begin(); _iter11 != vertices.end(); ++_iter11)
    {
      if (!writer.writeI32((*_iter11))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool Polygon::write(yarp::os::idl::WireWriter& writer) {
  if (!write_vertices(writer)) return false;
  return !writer.isError();
}

bool Polygon::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(1)) return false;
  return write(writer);
}
bool Polygon::Editor::write(yarp::os::ConnectionWriter& connection) {
  if (!isValid()) return false;
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(dirty_count+1)) return false;
  if (!writer.writeString("patch")) return false;
  if (is_dirty_vertices) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("vertices")) return false;
    if (!obj->nested_write_vertices(writer)) return false;
  }
  return !writer.isError();
}
bool Polygon::Editor::read(yarp::os::ConnectionReader& connection) {
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
      if (field=="vertices") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::vector<int32_t>  vertices")) return false;
      }
    }
    if (!writer.writeListHeader(2)) return false;
    writer.writeString("*** Available fields:");
    writer.writeString("vertices");
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
    if (key == "vertices") {
      will_set_vertices();
      if (!obj->nested_read_vertices(reader)) return false;
      did_set_vertices();
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

yarp::os::ConstString Polygon::toString() {
  yarp::os::Bottle b;
  b.read(*this);
  return b.toString();
}

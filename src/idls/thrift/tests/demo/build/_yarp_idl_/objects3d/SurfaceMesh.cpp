// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <SurfaceMesh.h>

bool SurfaceMesh::read_meshName(yarp::os::idl::WireReader& reader) {
  if (!reader.readString(meshName)) {
    reader.fail();
    return false;
  }
  return true;
}
bool SurfaceMesh::nested_read_meshName(yarp::os::idl::WireReader& reader) {
  if (!reader.readString(meshName)) {
    reader.fail();
    return false;
  }
  return true;
}
bool SurfaceMesh::read_points(yarp::os::idl::WireReader& reader) {
  {
    points.clear();
    uint32_t _size12;
    yarp::os::idl::WireState _etype15;
    reader.readListBegin(_etype15, _size12);
    points.resize(_size12);
    uint32_t _i16;
    for (_i16 = 0; _i16 < _size12; ++_i16)
    {
      if (!reader.readNested(points[_i16])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool SurfaceMesh::nested_read_points(yarp::os::idl::WireReader& reader) {
  {
    points.clear();
    uint32_t _size17;
    yarp::os::idl::WireState _etype20;
    reader.readListBegin(_etype20, _size17);
    points.resize(_size17);
    uint32_t _i21;
    for (_i21 = 0; _i21 < _size17; ++_i21)
    {
      if (!reader.readNested(points[_i21])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool SurfaceMesh::read_rgbColour(yarp::os::idl::WireReader& reader) {
  {
    rgbColour.clear();
    uint32_t _size22;
    yarp::os::idl::WireState _etype25;
    reader.readListBegin(_etype25, _size22);
    rgbColour.resize(_size22);
    uint32_t _i26;
    for (_i26 = 0; _i26 < _size22; ++_i26)
    {
      if (!reader.readNested(rgbColour[_i26])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool SurfaceMesh::nested_read_rgbColour(yarp::os::idl::WireReader& reader) {
  {
    rgbColour.clear();
    uint32_t _size27;
    yarp::os::idl::WireState _etype30;
    reader.readListBegin(_etype30, _size27);
    rgbColour.resize(_size27);
    uint32_t _i31;
    for (_i31 = 0; _i31 < _size27; ++_i31)
    {
      if (!reader.readNested(rgbColour[_i31])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool SurfaceMesh::read_polygons(yarp::os::idl::WireReader& reader) {
  {
    polygons.clear();
    uint32_t _size32;
    yarp::os::idl::WireState _etype35;
    reader.readListBegin(_etype35, _size32);
    polygons.resize(_size32);
    uint32_t _i36;
    for (_i36 = 0; _i36 < _size32; ++_i36)
    {
      if (!reader.readNested(polygons[_i36])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool SurfaceMesh::nested_read_polygons(yarp::os::idl::WireReader& reader) {
  {
    polygons.clear();
    uint32_t _size37;
    yarp::os::idl::WireState _etype40;
    reader.readListBegin(_etype40, _size37);
    polygons.resize(_size37);
    uint32_t _i41;
    for (_i41 = 0; _i41 < _size37; ++_i41)
    {
      if (!reader.readNested(polygons[_i41])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool SurfaceMesh::read(yarp::os::idl::WireReader& reader) {
  if (!read_meshName(reader)) return false;
  if (!read_points(reader)) return false;
  if (!read_rgbColour(reader)) return false;
  if (!read_polygons(reader)) return false;
  return !reader.isError();
}

bool SurfaceMesh::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListHeader(4)) return false;
  return read(reader);
}

bool SurfaceMesh::write_meshName(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeString(meshName)) return false;
  return true;
}
bool SurfaceMesh::nested_write_meshName(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeString(meshName)) return false;
  return true;
}
bool SurfaceMesh::write_points(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(points.size()))) return false;
    std::vector<PointXYZ> ::iterator _iter42;
    for (_iter42 = points.begin(); _iter42 != points.end(); ++_iter42)
    {
      if (!writer.writeNested((*_iter42))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool SurfaceMesh::nested_write_points(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(points.size()))) return false;
    std::vector<PointXYZ> ::iterator _iter43;
    for (_iter43 = points.begin(); _iter43 != points.end(); ++_iter43)
    {
      if (!writer.writeNested((*_iter43))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool SurfaceMesh::write_rgbColour(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(rgbColour.size()))) return false;
    std::vector<RGBA> ::iterator _iter44;
    for (_iter44 = rgbColour.begin(); _iter44 != rgbColour.end(); ++_iter44)
    {
      if (!writer.writeNested((*_iter44))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool SurfaceMesh::nested_write_rgbColour(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(rgbColour.size()))) return false;
    std::vector<RGBA> ::iterator _iter45;
    for (_iter45 = rgbColour.begin(); _iter45 != rgbColour.end(); ++_iter45)
    {
      if (!writer.writeNested((*_iter45))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool SurfaceMesh::write_polygons(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(polygons.size()))) return false;
    std::vector<Polygon> ::iterator _iter46;
    for (_iter46 = polygons.begin(); _iter46 != polygons.end(); ++_iter46)
    {
      if (!writer.writeNested((*_iter46))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool SurfaceMesh::nested_write_polygons(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(polygons.size()))) return false;
    std::vector<Polygon> ::iterator _iter47;
    for (_iter47 = polygons.begin(); _iter47 != polygons.end(); ++_iter47)
    {
      if (!writer.writeNested((*_iter47))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool SurfaceMesh::write(yarp::os::idl::WireWriter& writer) {
  if (!write_meshName(writer)) return false;
  if (!write_points(writer)) return false;
  if (!write_rgbColour(writer)) return false;
  if (!write_polygons(writer)) return false;
  return !writer.isError();
}

bool SurfaceMesh::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(4)) return false;
  return write(writer);
}
bool SurfaceMesh::Editor::write(yarp::os::ConnectionWriter& connection) {
  if (!isValid()) return false;
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(dirty_count+1)) return false;
  if (!writer.writeString("patch")) return false;
  if (is_dirty_meshName) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("meshName")) return false;
    if (!obj->nested_write_meshName(writer)) return false;
  }
  if (is_dirty_points) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("points")) return false;
    if (!obj->nested_write_points(writer)) return false;
  }
  if (is_dirty_rgbColour) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("rgbColour")) return false;
    if (!obj->nested_write_rgbColour(writer)) return false;
  }
  if (is_dirty_polygons) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("polygons")) return false;
    if (!obj->nested_write_polygons(writer)) return false;
  }
  return !writer.isError();
}
bool SurfaceMesh::Editor::read(yarp::os::ConnectionReader& connection) {
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
      if (field=="meshName") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::string meshName")) return false;
      }
      if (field=="points") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::vector<PointXYZ>  points")) return false;
      }
      if (field=="rgbColour") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::vector<RGBA>  rgbColour")) return false;
      }
      if (field=="polygons") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::vector<Polygon>  polygons")) return false;
      }
    }
    if (!writer.writeListHeader(5)) return false;
    writer.writeString("*** Available fields:");
    writer.writeString("meshName");
    writer.writeString("points");
    writer.writeString("rgbColour");
    writer.writeString("polygons");
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
    if (key == "meshName") {
      will_set_meshName();
      if (!obj->nested_read_meshName(reader)) return false;
      did_set_meshName();
    } else if (key == "points") {
      will_set_points();
      if (!obj->nested_read_points(reader)) return false;
      did_set_points();
    } else if (key == "rgbColour") {
      will_set_rgbColour();
      if (!obj->nested_read_rgbColour(reader)) return false;
      did_set_rgbColour();
    } else if (key == "polygons") {
      will_set_polygons();
      if (!obj->nested_read_polygons(reader)) return false;
      did_set_polygons();
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

yarp::os::ConstString SurfaceMesh::toString() {
  yarp::os::Bottle b;
  b.read(*this);
  return b.toString();
}

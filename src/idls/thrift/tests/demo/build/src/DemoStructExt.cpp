// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <DemoStructExt.h>

bool DemoStructExt::read_x(yarp::os::idl::WireReader& reader) {
  if (!reader.readI32(x)) {
    x = 0;
  }
  return true;
}
bool DemoStructExt::nested_read_x(yarp::os::idl::WireReader& reader) {
  if (!reader.readI32(x)) {
    x = 0;
  }
  return true;
}
bool DemoStructExt::read_y(yarp::os::idl::WireReader& reader) {
  if (!reader.readI32(y)) {
    y = 0;
  }
  return true;
}
bool DemoStructExt::nested_read_y(yarp::os::idl::WireReader& reader) {
  if (!reader.readI32(y)) {
    y = 0;
  }
  return true;
}
bool DemoStructExt::read_int_list(yarp::os::idl::WireReader& reader) {
  {
    int_list.clear();
    uint32_t _size12;
    yarp::os::idl::WireState _etype15;
    reader.readListBegin(_etype15, _size12);
    int_list.resize(_size12);
    uint32_t _i16;
    for (_i16 = 0; _i16 < _size12; ++_i16)
    {
      if (!reader.readI32(int_list[_i16])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool DemoStructExt::nested_read_int_list(yarp::os::idl::WireReader& reader) {
  {
    int_list.clear();
    uint32_t _size17;
    yarp::os::idl::WireState _etype20;
    reader.readListBegin(_etype20, _size17);
    int_list.resize(_size17);
    uint32_t _i21;
    for (_i21 = 0; _i21 < _size17; ++_i21)
    {
      if (!reader.readI32(int_list[_i21])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool DemoStructExt::read_ds_list(yarp::os::idl::WireReader& reader) {
  {
    ds_list.clear();
    uint32_t _size22;
    yarp::os::idl::WireState _etype25;
    reader.readListBegin(_etype25, _size22);
    ds_list.resize(_size22);
    uint32_t _i26;
    for (_i26 = 0; _i26 < _size22; ++_i26)
    {
      if (!reader.readNested(ds_list[_i26])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool DemoStructExt::nested_read_ds_list(yarp::os::idl::WireReader& reader) {
  {
    ds_list.clear();
    uint32_t _size27;
    yarp::os::idl::WireState _etype30;
    reader.readListBegin(_etype30, _size27);
    ds_list.resize(_size27);
    uint32_t _i31;
    for (_i31 = 0; _i31 < _size27; ++_i31)
    {
      if (!reader.readNested(ds_list[_i31])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool DemoStructExt::read(yarp::os::idl::WireReader& reader) {
  if (!read_x(reader)) return false;
  if (!read_y(reader)) return false;
  if (!read_int_list(reader)) return false;
  if (!read_ds_list(reader)) return false;
  return !reader.isError();
}

bool DemoStructExt::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListHeader(4)) return false;
  return read(reader);
}

bool DemoStructExt::write_x(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeI32(x)) return false;
  return true;
}
bool DemoStructExt::nested_write_x(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeI32(x)) return false;
  return true;
}
bool DemoStructExt::write_y(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeI32(y)) return false;
  return true;
}
bool DemoStructExt::nested_write_y(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeI32(y)) return false;
  return true;
}
bool DemoStructExt::write_int_list(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_INT, static_cast<uint32_t>(int_list.size()))) return false;
    std::vector<int32_t> ::iterator _iter32;
    for (_iter32 = int_list.begin(); _iter32 != int_list.end(); ++_iter32)
    {
      if (!writer.writeI32((*_iter32))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool DemoStructExt::nested_write_int_list(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_INT, static_cast<uint32_t>(int_list.size()))) return false;
    std::vector<int32_t> ::iterator _iter33;
    for (_iter33 = int_list.begin(); _iter33 != int_list.end(); ++_iter33)
    {
      if (!writer.writeI32((*_iter33))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool DemoStructExt::write_ds_list(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(ds_list.size()))) return false;
    std::vector<DemoStruct> ::iterator _iter34;
    for (_iter34 = ds_list.begin(); _iter34 != ds_list.end(); ++_iter34)
    {
      if (!writer.writeNested((*_iter34))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool DemoStructExt::nested_write_ds_list(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(ds_list.size()))) return false;
    std::vector<DemoStruct> ::iterator _iter35;
    for (_iter35 = ds_list.begin(); _iter35 != ds_list.end(); ++_iter35)
    {
      if (!writer.writeNested((*_iter35))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool DemoStructExt::write(yarp::os::idl::WireWriter& writer) {
  if (!write_x(writer)) return false;
  if (!write_y(writer)) return false;
  if (!write_int_list(writer)) return false;
  if (!write_ds_list(writer)) return false;
  return !writer.isError();
}

bool DemoStructExt::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(4)) return false;
  return write(writer);
}
bool DemoStructExt::Editor::write(yarp::os::ConnectionWriter& connection) {
  if (!isValid()) return false;
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(dirty_count+1)) return false;
  if (!writer.writeString("patch")) return false;
  if (is_dirty_x) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("x")) return false;
    if (!obj->nested_write_x(writer)) return false;
  }
  if (is_dirty_y) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("y")) return false;
    if (!obj->nested_write_y(writer)) return false;
  }
  if (is_dirty_int_list) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("int_list")) return false;
    if (!obj->nested_write_int_list(writer)) return false;
  }
  if (is_dirty_ds_list) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("ds_list")) return false;
    if (!obj->nested_write_ds_list(writer)) return false;
  }
  return !writer.isError();
}
bool DemoStructExt::Editor::read(yarp::os::ConnectionReader& connection) {
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
      if (field=="x") {
        if (!writer.writeListHeader(2)) return false;
        if (!writer.writeString("int32_t x")) return false;
        if (!writer.writeString("this is the x part")) return false;
      }
      if (field=="y") {
        if (!writer.writeListHeader(2)) return false;
        if (!writer.writeString("int32_t y")) return false;
        if (!writer.writeString("this is the y part")) return false;
      }
      if (field=="int_list") {
        if (!writer.writeListHeader(2)) return false;
        if (!writer.writeString("std::vector<int32_t>  int_list")) return false;
        if (!writer.writeString("this is a list of ints")) return false;
      }
      if (field=="ds_list") {
        if (!writer.writeListHeader(2)) return false;
        if (!writer.writeString("std::vector<DemoStruct>  ds_list")) return false;
        if (!writer.writeString("this is a list of structs")) return false;
      }
    }
    if (!writer.writeListHeader(5)) return false;
    writer.writeString("*** Available fields:");
    writer.writeString("x");
    writer.writeString("y");
    writer.writeString("int_list");
    writer.writeString("ds_list");
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
    if (key == "x") {
      will_set_x();
      if (!obj->nested_read_x(reader)) return false;
      did_set_x();
    } else if (key == "y") {
      will_set_y();
      if (!obj->nested_read_y(reader)) return false;
      did_set_y();
    } else if (key == "int_list") {
      will_set_int_list();
      if (!obj->nested_read_int_list(reader)) return false;
      did_set_int_list();
    } else if (key == "ds_list") {
      will_set_ds_list();
      if (!obj->nested_read_ds_list(reader)) return false;
      did_set_ds_list();
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

yarp::os::ConstString DemoStructExt::toString() {
  yarp::os::Bottle b;
  b.read(*this);
  return b.toString();
}

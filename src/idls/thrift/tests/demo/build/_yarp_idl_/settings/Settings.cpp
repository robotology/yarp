// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <Settings.h>

bool Settings::read_id(yarp::os::idl::WireReader& reader) {
  if (!reader.readI32(id)) {
    reader.fail();
    return false;
  }
  return true;
}
bool Settings::nested_read_id(yarp::os::idl::WireReader& reader) {
  if (!reader.readI32(id)) {
    reader.fail();
    return false;
  }
  return true;
}
bool Settings::read_name(yarp::os::idl::WireReader& reader) {
  if (!reader.readString(name)) {
    reader.fail();
    return false;
  }
  return true;
}
bool Settings::nested_read_name(yarp::os::idl::WireReader& reader) {
  if (!reader.readString(name)) {
    reader.fail();
    return false;
  }
  return true;
}
bool Settings::read_pt(yarp::os::idl::WireReader& reader) {
  if (!reader.read(pt)) {
    reader.fail();
    return false;
  }
  return true;
}
bool Settings::nested_read_pt(yarp::os::idl::WireReader& reader) {
  if (!reader.readNested(pt)) {
    reader.fail();
    return false;
  }
  return true;
}
bool Settings::read_ints(yarp::os::idl::WireReader& reader) {
  {
    ints.clear();
    uint32_t _size0;
    yarp::os::idl::WireState _etype3;
    reader.readListBegin(_etype3, _size0);
    ints.resize(_size0);
    uint32_t _i4;
    for (_i4 = 0; _i4 < _size0; ++_i4)
    {
      if (!reader.readI32(ints[_i4])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool Settings::nested_read_ints(yarp::os::idl::WireReader& reader) {
  {
    ints.clear();
    uint32_t _size5;
    yarp::os::idl::WireState _etype8;
    reader.readListBegin(_etype8, _size5);
    ints.resize(_size5);
    uint32_t _i9;
    for (_i9 = 0; _i9 < _size5; ++_i9)
    {
      if (!reader.readI32(ints[_i9])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool Settings::read(yarp::os::idl::WireReader& reader) {
  if (!read_id(reader)) return false;
  if (!read_name(reader)) return false;
  if (!read_pt(reader)) return false;
  if (!read_ints(reader)) return false;
  return !reader.isError();
}

bool Settings::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListHeader(5)) return false;
  return read(reader);
}

bool Settings::write_id(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeI32(id)) return false;
  return true;
}
bool Settings::nested_write_id(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeI32(id)) return false;
  return true;
}
bool Settings::write_name(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeString(name)) return false;
  return true;
}
bool Settings::nested_write_name(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeString(name)) return false;
  return true;
}
bool Settings::write_pt(yarp::os::idl::WireWriter& writer) {
  if (!writer.write(pt)) return false;
  return true;
}
bool Settings::nested_write_pt(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeNested(pt)) return false;
  return true;
}
bool Settings::write_ints(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_INT, static_cast<uint32_t>(ints.size()))) return false;
    std::vector<int32_t> ::iterator _iter10;
    for (_iter10 = ints.begin(); _iter10 != ints.end(); ++_iter10)
    {
      if (!writer.writeI32((*_iter10))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool Settings::nested_write_ints(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_INT, static_cast<uint32_t>(ints.size()))) return false;
    std::vector<int32_t> ::iterator _iter11;
    for (_iter11 = ints.begin(); _iter11 != ints.end(); ++_iter11)
    {
      if (!writer.writeI32((*_iter11))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool Settings::write(yarp::os::idl::WireWriter& writer) {
  if (!write_id(writer)) return false;
  if (!write_name(writer)) return false;
  if (!write_pt(writer)) return false;
  if (!write_ints(writer)) return false;
  return !writer.isError();
}

bool Settings::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(5)) return false;
  return write(writer);
}
bool Settings::Editor::write(yarp::os::ConnectionWriter& connection) {
  if (!isValid()) return false;
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(dirty_count+1)) return false;
  if (!writer.writeString("patch")) return false;
  if (is_dirty_id) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("id")) return false;
    if (!obj->nested_write_id(writer)) return false;
  }
  if (is_dirty_name) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("name")) return false;
    if (!obj->nested_write_name(writer)) return false;
  }
  if (is_dirty_pt) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("pt")) return false;
    if (!obj->nested_write_pt(writer)) return false;
  }
  if (is_dirty_ints) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("ints")) return false;
    if (!obj->nested_write_ints(writer)) return false;
  }
  return !writer.isError();
}
bool Settings::Editor::read(yarp::os::ConnectionReader& connection) {
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
      if (field=="id") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("int32_t id")) return false;
      }
      if (field=="name") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::string name")) return false;
      }
      if (field=="pt") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("Point pt")) return false;
      }
      if (field=="ints") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::vector<int32_t>  ints")) return false;
      }
    }
    if (!writer.writeListHeader(5)) return false;
    writer.writeString("*** Available fields:");
    writer.writeString("id");
    writer.writeString("name");
    writer.writeString("pt");
    writer.writeString("ints");
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
    if (key == "id") {
      will_set_id();
      if (!obj->nested_read_id(reader)) return false;
      did_set_id();
    } else if (key == "name") {
      will_set_name();
      if (!obj->nested_read_name(reader)) return false;
      did_set_name();
    } else if (key == "pt") {
      will_set_pt();
      if (!obj->nested_read_pt(reader)) return false;
      did_set_pt();
    } else if (key == "ints") {
      will_set_ints();
      if (!obj->nested_read_ints(reader)) return false;
      did_set_ints();
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

yarp::os::ConstString Settings::toString() {
  yarp::os::Bottle b;
  b.read(*this);
  return b.toString();
}

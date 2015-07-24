// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <DemoStructList.h>

bool DemoStructList::read_lst(yarp::os::idl::WireReader& reader) {
  {
    lst.clear();
    uint32_t _size0;
    yarp::os::idl::WireState _etype3;
    reader.readListBegin(_etype3, _size0);
    lst.resize(_size0);
    uint32_t _i4;
    for (_i4 = 0; _i4 < _size0; ++_i4)
    {
      if (!reader.readNested(lst[_i4])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool DemoStructList::nested_read_lst(yarp::os::idl::WireReader& reader) {
  {
    lst.clear();
    uint32_t _size5;
    yarp::os::idl::WireState _etype8;
    reader.readListBegin(_etype8, _size5);
    lst.resize(_size5);
    uint32_t _i9;
    for (_i9 = 0; _i9 < _size5; ++_i9)
    {
      if (!reader.readNested(lst[_i9])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool DemoStructList::read(yarp::os::idl::WireReader& reader) {
  if (!read_lst(reader)) return false;
  return !reader.isError();
}

bool DemoStructList::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListHeader(1)) return false;
  return read(reader);
}

bool DemoStructList::write_lst(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(lst.size()))) return false;
    std::vector<DemoStruct> ::iterator _iter10;
    for (_iter10 = lst.begin(); _iter10 != lst.end(); ++_iter10)
    {
      if (!writer.writeNested((*_iter10))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool DemoStructList::nested_write_lst(yarp::os::idl::WireWriter& writer) {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(lst.size()))) return false;
    std::vector<DemoStruct> ::iterator _iter11;
    for (_iter11 = lst.begin(); _iter11 != lst.end(); ++_iter11)
    {
      if (!writer.writeNested((*_iter11))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool DemoStructList::write(yarp::os::idl::WireWriter& writer) {
  if (!write_lst(writer)) return false;
  return !writer.isError();
}

bool DemoStructList::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(1)) return false;
  return write(writer);
}
bool DemoStructList::Editor::write(yarp::os::ConnectionWriter& connection) {
  if (!isValid()) return false;
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(dirty_count+1)) return false;
  if (!writer.writeString("patch")) return false;
  if (is_dirty_lst) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("lst")) return false;
    if (!obj->nested_write_lst(writer)) return false;
  }
  return !writer.isError();
}
bool DemoStructList::Editor::read(yarp::os::ConnectionReader& connection) {
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
      if (field=="lst") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::vector<DemoStruct>  lst")) return false;
      }
    }
    if (!writer.writeListHeader(2)) return false;
    writer.writeString("*** Available fields:");
    writer.writeString("lst");
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
    if (key == "lst") {
      will_set_lst();
      if (!obj->nested_read_lst(reader)) return false;
      did_set_lst();
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

yarp::os::ConstString DemoStructList::toString() {
  yarp::os::Bottle b;
  b.read(*this);
  return b.toString();
}

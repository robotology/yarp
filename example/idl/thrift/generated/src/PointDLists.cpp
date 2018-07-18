/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <firstInterface/PointDLists.h>

namespace yarp { namespace test {
bool PointDLists::read_name(yarp::os::idl::WireReader& reader) {
  if (!reader.readString(name)) {
    name = "pointLists";
  }
  return true;
}
bool PointDLists::nested_read_name(yarp::os::idl::WireReader& reader) {
  if (!reader.readString(name)) {
    name = "pointLists";
  }
  return true;
}
bool PointDLists::read_firstList(yarp::os::idl::WireReader& reader) {
  {
    firstList.clear();
    uint32_t _size0;
    yarp::os::idl::WireState _etype3;
    reader.readListBegin(_etype3, _size0);
    firstList.resize(_size0);
    uint32_t _i4;
    for (_i4 = 0; _i4 < _size0; ++_i4)
    {
      if (!reader.readNested(firstList[_i4])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool PointDLists::nested_read_firstList(yarp::os::idl::WireReader& reader) {
  {
    firstList.clear();
    uint32_t _size5;
    yarp::os::idl::WireState _etype8;
    reader.readListBegin(_etype8, _size5);
    firstList.resize(_size5);
    uint32_t _i9;
    for (_i9 = 0; _i9 < _size5; ++_i9)
    {
      if (!reader.readNested(firstList[_i9])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool PointDLists::read_secondList(yarp::os::idl::WireReader& reader) {
  {
    secondList.clear();
    uint32_t _size10;
    yarp::os::idl::WireState _etype13;
    reader.readListBegin(_etype13, _size10);
    secondList.resize(_size10);
    uint32_t _i14;
    for (_i14 = 0; _i14 < _size10; ++_i14)
    {
      if (!reader.readNested(secondList[_i14])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool PointDLists::nested_read_secondList(yarp::os::idl::WireReader& reader) {
  {
    secondList.clear();
    uint32_t _size15;
    yarp::os::idl::WireState _etype18;
    reader.readListBegin(_etype18, _size15);
    secondList.resize(_size15);
    uint32_t _i19;
    for (_i19 = 0; _i19 < _size15; ++_i19)
    {
      if (!reader.readNested(secondList[_i19])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool PointDLists::read(yarp::os::idl::WireReader& reader) {
  if (!read_name(reader)) return false;
  if (!read_firstList(reader)) return false;
  if (!read_secondList(reader)) return false;
  return !reader.isError();
}

bool PointDLists::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListHeader(3)) return false;
  return read(reader);
}

bool PointDLists::write_name(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeString(name)) return false;
  return true;
}
bool PointDLists::nested_write_name(const yarp::os::idl::WireWriter& writer) const {
  if (!writer.writeString(name)) return false;
  return true;
}
bool PointDLists::write_firstList(const yarp::os::idl::WireWriter& writer) const {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(firstList.size()))) return false;
    std::vector<PointD> ::const_iterator _iter20;
    for (_iter20 = firstList.begin(); _iter20 != firstList.end(); ++_iter20)
    {
      if (!writer.writeNested((*_iter20))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool PointDLists::nested_write_firstList(const yarp::os::idl::WireWriter& writer) const {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(firstList.size()))) return false;
    std::vector<PointD> ::const_iterator _iter21;
    for (_iter21 = firstList.begin(); _iter21 != firstList.end(); ++_iter21)
    {
      if (!writer.writeNested((*_iter21))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool PointDLists::write_secondList(const yarp::os::idl::WireWriter& writer) const {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(secondList.size()))) return false;
    std::vector<PointD> ::const_iterator _iter22;
    for (_iter22 = secondList.begin(); _iter22 != secondList.end(); ++_iter22)
    {
      if (!writer.writeNested((*_iter22))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool PointDLists::nested_write_secondList(const yarp::os::idl::WireWriter& writer) const {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(secondList.size()))) return false;
    std::vector<PointD> ::const_iterator _iter23;
    for (_iter23 = secondList.begin(); _iter23 != secondList.end(); ++_iter23)
    {
      if (!writer.writeNested((*_iter23))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool PointDLists::write(const yarp::os::idl::WireWriter& writer) const {
  if (!write_name(writer)) return false;
  if (!write_firstList(writer)) return false;
  if (!write_secondList(writer)) return false;
  return !writer.isError();
}

bool PointDLists::write(yarp::os::ConnectionWriter& connection) const {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(3)) return false;
  return write(writer);
}
bool PointDLists::Editor::write(yarp::os::ConnectionWriter& connection) const {
  if (!isValid()) return false;
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(dirty_count+1)) return false;
  if (!writer.writeString("patch")) return false;
  if (is_dirty_name) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("name")) return false;
    if (!obj->nested_write_name(writer)) return false;
  }
  if (is_dirty_firstList) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("firstList")) return false;
    if (!obj->nested_write_firstList(writer)) return false;
  }
  if (is_dirty_secondList) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("secondList")) return false;
    if (!obj->nested_write_secondList(writer)) return false;
  }
  return !writer.isError();
}
bool PointDLists::Editor::read(yarp::os::ConnectionReader& connection) {
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
  std::string tag;
  if (!reader.readString(tag)) return false;
  if (tag=="help") {
    yarp::os::idl::WireWriter writer(reader);
    if (writer.isNull()) return true;
    if (!writer.writeListHeader(2)) return false;
    if (!writer.writeTag("many",1, 0)) return false;
    if (reader.getLength()>0) {
      std::string field;
      if (!reader.readString(field)) return false;
      if (field=="name") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::string name")) return false;
      }
      if (field=="firstList") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::vector<PointD>  firstList")) return false;
      }
      if (field=="secondList") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::vector<PointD>  secondList")) return false;
      }
    }
    if (!writer.writeListHeader(4)) return false;
    writer.writeString("*** Available fields:");
    writer.writeString("name");
    writer.writeString("firstList");
    writer.writeString("secondList");
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
    std::string act;
    std::string key;
    if (have_act) {
      act = tag;
    } else {
      if (!reader.readString(act)) return false;
    }
    if (!reader.readString(key)) return false;
    // inefficient code follows, bug paulfitz to improve it
    if (key == "name") {
      will_set_name();
      if (!obj->nested_read_name(reader)) return false;
      did_set_name();
    } else if (key == "firstList") {
      will_set_firstList();
      if (!obj->nested_read_firstList(reader)) return false;
      did_set_firstList();
    } else if (key == "secondList") {
      will_set_secondList();
      if (!obj->nested_read_secondList(reader)) return false;
      did_set_secondList();
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

std::string PointDLists::toString() const {
  yarp::os::Bottle b;
  b.read(*this);
  return b.toString();
}
}} // namespace

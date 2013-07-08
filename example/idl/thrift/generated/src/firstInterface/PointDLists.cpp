// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <firstInterface/PointDLists.h>

namespace yarp { namespace test {
bool PointDLists::read(yarp::os::idl::WireReader& reader) {
  if (!reader.readString(name)) {
    name = "pointLists";
  }
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
  {
    secondList.clear();
    uint32_t _size5;
    yarp::os::idl::WireState _etype8;
    reader.readListBegin(_etype8, _size5);
    secondList.resize(_size5);
    uint32_t _i9;
    for (_i9 = 0; _i9 < _size5; ++_i9)
    {
      if (!reader.readNested(secondList[_i9])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return !reader.isError();
}

bool PointDLists::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListHeader(3)) return false;
  return read(reader);
}

bool PointDLists::write(yarp::os::idl::WireWriter& writer) {
  if (!writer.writeString(name)) return false;
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(firstList.size()))) return false;
    std::vector<PointD> ::iterator _iter10;
    for (_iter10 = firstList.begin(); _iter10 != firstList.end(); ++_iter10)
    {
      if (!writer.writeNested((*_iter10))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(secondList.size()))) return false;
    std::vector<PointD> ::iterator _iter11;
    for (_iter11 = secondList.begin(); _iter11 != secondList.end(); ++_iter11)
    {
      if (!writer.writeNested((*_iter11))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return !writer.isError();
}

bool PointDLists::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(3)) return false;
  return write(writer);
}
}} // namespace

// This is an automatically generated file.
// Generated from this Demo.msg definition:
//   int32 x
//   int8 a_signed_byte
//   uint8 an_unsigned_byte
//   int16 a_signed_byte2
//   uint16 an_unsigned_byte2
//   int32 a_signed_byte4
//   uint32 an_unsigned_byte4
//   int64 a_signed_byte8
//   uint64 an_unsigned_byte8
//   string str
//   bool a_bool
//   int8[] byte_list
//   string[] string_list
//   int8[3] fixed_byte_list
//   string[3] fixed_string_list
//   int32 fixed = 3
//   SharedData[] data
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARPMSG_TYPE_Demo
#define YARPMSG_TYPE_Demo

#include <string>
#include <vector>
#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <SharedData.h>

class Demo : public yarp::os::idl::WirePortable {
public:
  yarp::os::NetInt32 x;
  char a_signed_byte;
  unsigned char an_unsigned_byte;
  yarp::os::NetInt16 a_signed_byte2;
  yarp::os::NetUint16 an_unsigned_byte2;
  yarp::os::NetInt32 a_signed_byte4;
  yarp::os::NetUint32 an_unsigned_byte4;
  yarp::os::NetInt64 a_signed_byte8;
  yarp::os::NetUint64 an_unsigned_byte8;
  std::string str;
  bool a_bool;
  std::vector<char> byte_list;
  std::vector<std::string> string_list;
  std::vector<char> fixed_byte_list;
  std::vector<std::string> fixed_string_list;
  static const yarp::os::NetInt32 fixed = 3;
  std::vector<SharedData> data;

  Demo() {
    fixed_byte_list.resize(3,0);
    fixed_string_list.resize(3);
  }

  bool readBare(yarp::os::ConnectionReader& connection) {
    // *** x ***
    x = connection.expectInt();

    // *** a_signed_byte ***
    if (!connection.expectBlock((char*)&a_signed_byte,1)) return false;

    // *** an_unsigned_byte ***
    if (!connection.expectBlock((char*)&an_unsigned_byte,1)) return false;

    // *** a_signed_byte2 ***
    if (!connection.expectBlock((char*)&a_signed_byte2,2)) return false;

    // *** an_unsigned_byte2 ***
    if (!connection.expectBlock((char*)&an_unsigned_byte2,2)) return false;

    // *** a_signed_byte4 ***
    a_signed_byte4 = connection.expectInt();

    // *** an_unsigned_byte4 ***
    an_unsigned_byte4 = connection.expectInt();

    // *** a_signed_byte8 ***
    if (!connection.expectBlock((char*)&a_signed_byte8,8)) return false;

    // *** an_unsigned_byte8 ***
    if (!connection.expectBlock((char*)&an_unsigned_byte8,8)) return false;

    // *** str ***
    int len = connection.expectInt();
    str.resize(len);
    if (!connection.expectBlock((char*)str.c_str(),len)) return false;

    // *** a_bool ***
    if (!connection.expectBlock((char*)&a_bool,1)) return false;

    // *** byte_list ***
    len = connection.expectInt();
    byte_list.resize(len);
    if (!connection.expectBlock((char*)&byte_list[0],sizeof(char)*len)) return false;

    // *** string_list ***
    len = connection.expectInt();
    string_list.resize(len);
    for (int i=0; i<len; i++) {
      int len2 = connection.expectInt();
      string_list[i].resize(len2);
      if (!connection.expectBlock((char*)string_list[i].c_str(),len2)) return false;
    }

    // *** fixed_byte_list ***
    len = 3;
    fixed_byte_list.resize(len);
    if (!connection.expectBlock((char*)&fixed_byte_list[0],sizeof(char)*len)) return false;

    // *** fixed_string_list ***
    len = 3;
    fixed_string_list.resize(len);
    for (int i=0; i<len; i++) {
      int len2 = connection.expectInt();
      fixed_string_list[i].resize(len2);
      if (!connection.expectBlock((char*)fixed_string_list[i].c_str(),len2)) return false;
    }

    // *** data ***
    len = connection.expectInt();
    data.resize(len);
    for (int i=0; i<len; i++) {
      if (!data[i].read(connection)) return false;
    }
    return !connection.isError();
  }

  bool readBottle(yarp::os::ConnectionReader& connection) {
    connection.convertTextMode();
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListHeader(17)) return false;

    // *** x ***
    x = reader.expectInt();

    // *** a_signed_byte ***
    a_signed_byte = reader.expectInt();

    // *** an_unsigned_byte ***
    an_unsigned_byte = reader.expectInt();

    // *** a_signed_byte2 ***
    a_signed_byte2 = reader.expectInt();

    // *** an_unsigned_byte2 ***
    an_unsigned_byte2 = reader.expectInt();

    // *** a_signed_byte4 ***
    a_signed_byte4 = reader.expectInt();

    // *** an_unsigned_byte4 ***
    an_unsigned_byte4 = reader.expectInt();

    // *** a_signed_byte8 ***
    a_signed_byte8 = reader.expectInt();

    // *** an_unsigned_byte8 ***
    an_unsigned_byte8 = reader.expectInt();

    // *** str ***
    if (!reader.readString(str)) return false;

    // *** a_bool ***
    a_bool = reader.expectInt();

    // *** byte_list ***
    if (connection.expectInt()!=(BOTTLE_TAG_LIST|BOTTLE_TAG_INT)) return false;
    int len = connection.expectInt();
    byte_list.resize(len);
    for (size_t i=0; i<len; i++) {
      byte_list[i] = (char)connection.expectInt();
    }

    // *** string_list ***
    if (connection.expectInt()!=(BOTTLE_TAG_LIST|BOTTLE_TAG_STRING)) return false;
    len = connection.expectInt();
    string_list.resize(len);
    for (int i=0; i<len; i++) {
      int len2 = connection.expectInt();
      string_list[i].resize(len2);
      if (!connection.expectBlock((char*)string_list[i].c_str(),len2)) return false;
    }

    // *** fixed_byte_list ***
    if (connection.expectInt()!=(BOTTLE_TAG_LIST|BOTTLE_TAG_INT)) return false;
    len = connection.expectInt();
    fixed_byte_list.resize(len);
    for (size_t i=0; i<len; i++) {
      fixed_byte_list[i] = (char)connection.expectInt();
    }

    // *** fixed_string_list ***
    if (connection.expectInt()!=(BOTTLE_TAG_LIST|BOTTLE_TAG_STRING)) return false;
    len = connection.expectInt();
    fixed_string_list.resize(len);
    for (int i=0; i<len; i++) {
      int len2 = connection.expectInt();
      fixed_string_list[i].resize(len2);
      if (!connection.expectBlock((char*)fixed_string_list[i].c_str(),len2)) return false;
    }

    // *** data ***
    if (connection.expectInt()!=BOTTLE_TAG_LIST) return false;
    len = connection.expectInt();
    data.resize(len);
    for (int i=0; i<len; i++) {
      if (!data[i].read(connection)) return false;
    }
    return !connection.isError();
  }

  bool read(yarp::os::ConnectionReader& connection) {
    if (connection.isBareMode()) return readBare(connection);
    return readBottle(connection);
  }

  bool writeBare(yarp::os::ConnectionWriter& connection) {
    // *** x ***
    connection.appendInt(x);

    // *** a_signed_byte ***
    connection.appendBlock((char*)&a_signed_byte,1);

    // *** an_unsigned_byte ***
    connection.appendBlock((char*)&an_unsigned_byte,1);

    // *** a_signed_byte2 ***
    connection.appendBlock((char*)&a_signed_byte2,2);

    // *** an_unsigned_byte2 ***
    connection.appendBlock((char*)&an_unsigned_byte2,2);

    // *** a_signed_byte4 ***
    connection.appendInt(a_signed_byte4);

    // *** an_unsigned_byte4 ***
    connection.appendInt(an_unsigned_byte4);

    // *** a_signed_byte8 ***
    connection.appendBlock((char*)&a_signed_byte8,8);

    // *** an_unsigned_byte8 ***
    connection.appendBlock((char*)&an_unsigned_byte8,8);

    // *** str ***
    connection.appendInt(str.length());
    connection.appendExternalBlock((char*)str.c_str(),str.length());

    // *** a_bool ***
    connection.appendBlock((char*)&a_bool,1);

    // *** byte_list ***
    connection.appendInt(byte_list.size());
    connection.appendExternalBlock((char*)&byte_list[0],sizeof(char)*byte_list.size());

    // *** string_list ***
    connection.appendInt(string_list.size());
    for (size_t i=0; i<string_list.size(); i++) {
      connection.appendInt(string_list[i].length());
      connection.appendExternalBlock((char*)string_list[i].c_str(),string_list[i].length());
    }

    // *** fixed_byte_list ***
    connection.appendExternalBlock((char*)&fixed_byte_list[0],sizeof(char)*fixed_byte_list.size());

    // *** fixed_string_list ***
    for (size_t i=0; i<fixed_string_list.size(); i++) {
      connection.appendInt(fixed_string_list[i].length());
      connection.appendExternalBlock((char*)fixed_string_list[i].c_str(),fixed_string_list[i].length());
    }

    // *** data ***
    connection.appendInt(data.size());
    for (size_t i=0; i<data.size(); i++) {
      if (!data[i].write(connection)) return false;
    }
    return !connection.isError();
  }

  bool writeBottle(yarp::os::ConnectionWriter& connection) {
    connection.appendInt(BOTTLE_TAG_LIST);
    connection.appendInt(17);

    // *** x ***
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt((int)x);

    // *** a_signed_byte ***
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt((int)a_signed_byte);

    // *** an_unsigned_byte ***
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt((int)an_unsigned_byte);

    // *** a_signed_byte2 ***
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt((int)a_signed_byte2);

    // *** an_unsigned_byte2 ***
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt((int)an_unsigned_byte2);

    // *** a_signed_byte4 ***
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt((int)a_signed_byte4);

    // *** an_unsigned_byte4 ***
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt((int)an_unsigned_byte4);

    // *** a_signed_byte8 ***
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt((int)a_signed_byte8);

    // *** an_unsigned_byte8 ***
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt((int)an_unsigned_byte8);

    // *** str ***
    connection.appendInt(BOTTLE_TAG_STRING);
    connection.appendInt(str.length()+1);
    connection.appendExternalBlock((char*)str.c_str(),str.length()+1);

    // *** a_bool ***
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt((int)a_bool);

    // *** byte_list ***
    connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_INT);
    connection.appendInt(byte_list.size());
    for (size_t i=0; i<byte_list.size(); i++) {
      connection.appendInt((int)byte_list[i]);
    }

    // *** string_list ***
    connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_STRING);
    connection.appendInt(string_list.size());
    for (size_t i=0; i<string_list.size(); i++) {
      connection.appendInt(string_list[i].length()+1);
      connection.appendExternalBlock((char*)string_list[i].c_str(),string_list[i].length()+1);
    }

    // *** fixed_byte_list ***
    connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_INT);
    connection.appendInt(fixed_byte_list.size());
    for (size_t i=0; i<fixed_byte_list.size(); i++) {
      connection.appendInt((int)fixed_byte_list[i]);
    }

    // *** fixed_string_list ***
    connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_STRING);
    connection.appendInt(fixed_string_list.size());
    for (size_t i=0; i<fixed_string_list.size(); i++) {
      connection.appendInt(fixed_string_list[i].length()+1);
      connection.appendExternalBlock((char*)fixed_string_list[i].c_str(),fixed_string_list[i].length()+1);
    }

    // *** data ***
    connection.appendInt(BOTTLE_TAG_LIST);
    connection.appendInt(data.size());
    for (size_t i=0; i<data.size(); i++) {
      if (!data[i].write(connection)) return false;
    }
    connection.convertTextMode();
    return !connection.isError();
  }

  bool write(yarp::os::ConnectionWriter& connection) {
    if (connection.isBareMode()) return writeBare(connection);
    return writeBottle(connection);
  }

  // This class will serialize ROS style or YARP style depending on protocol.
  // If you need to force a serialization style, use one of these classes:
  typedef yarp::os::idl::BareStyle<Demo> rosStyle;
  typedef yarp::os::idl::BottleStyle<Demo> bottleStyle;

  // Give source text for class, ROS will need this
  yarp::os::ConstString getTypeText() {
    return "int32 x\n\
int8 a_signed_byte\n\
uint8 an_unsigned_byte\n\
int16 a_signed_byte2\n\
uint16 an_unsigned_byte2\n\
int32 a_signed_byte4\n\
uint32 an_unsigned_byte4\n\
int64 a_signed_byte8\n\
uint64 an_unsigned_byte8\n\
string str\n\
bool a_bool\n\
int8[] byte_list\n\
string[] string_list\n\
int8[3] fixed_byte_list\n\
string[3] fixed_string_list\n\
int32 fixed = 3\n\
SharedData[] data\n================================================================================\n\
MSG: SharedData\n\
string text\n\
float64[] content";
  }

  // Name the class, ROS will need this
  yarp::os::Type getType() {
    yarp::os::Type typ = yarp::os::Type::byName("Demo","Demo");
    typ.addProperty("md5sum",yarp::os::Value("efc05e7a025ac5817b9b6016984bfe30"));
    typ.addProperty("message_definition",yarp::os::Value(getTypeText()));
    return typ;
  }
};

#endif

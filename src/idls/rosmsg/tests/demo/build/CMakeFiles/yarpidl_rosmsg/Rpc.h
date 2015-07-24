// This is an automatically generated file.
// Generated from this Rpc.msg definition:
//   #comment
//   int8 PIPPO=1
//   int8 PLUTO=2
//   #comment
//   int8 pluppo
//   string msg
//   ---
//   #comment
//   uint32 CODE=54321
//   #comment
//   string val
//   uint32 an_int
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARPMSG_TYPE_Rpc
#define YARPMSG_TYPE_Rpc

#include <string>
#include <vector>
#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <RpcReply.h>

class Rpc : public yarp::os::idl::WirePortable {
public:
  static const char PIPPO = 1;
  static const char PLUTO = 2;
  char pluppo;
  std::string msg;

  Rpc() {
  }

  bool readBare(yarp::os::ConnectionReader& connection) {
    // *** pluppo ***
    if (!connection.expectBlock((char*)&pluppo,1)) return false;

    // *** msg ***
    int len = connection.expectInt();
    msg.resize(len);
    if (!connection.expectBlock((char*)msg.c_str(),len)) return false;
    return !connection.isError();
  }

  bool readBottle(yarp::os::ConnectionReader& connection) {
    connection.convertTextMode();
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListHeader(4)) return false;

    // *** pluppo ***
    pluppo = reader.expectInt();

    // *** msg ***
    if (!reader.readString(msg)) return false;
    return !connection.isError();
  }

  bool read(yarp::os::ConnectionReader& connection) {
    if (connection.isBareMode()) return readBare(connection);
    return readBottle(connection);
  }

  bool writeBare(yarp::os::ConnectionWriter& connection) {
    // *** pluppo ***
    connection.appendBlock((char*)&pluppo,1);

    // *** msg ***
    connection.appendInt(msg.length());
    connection.appendExternalBlock((char*)msg.c_str(),msg.length());
    return !connection.isError();
  }

  bool writeBottle(yarp::os::ConnectionWriter& connection) {
    connection.appendInt(BOTTLE_TAG_LIST);
    connection.appendInt(4);

    // *** pluppo ***
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt((int)pluppo);

    // *** msg ***
    connection.appendInt(BOTTLE_TAG_STRING);
    connection.appendInt(msg.length()+1);
    connection.appendExternalBlock((char*)msg.c_str(),msg.length()+1);
    connection.convertTextMode();
    return !connection.isError();
  }

  bool write(yarp::os::ConnectionWriter& connection) {
    if (connection.isBareMode()) return writeBare(connection);
    return writeBottle(connection);
  }

  // This class will serialize ROS style or YARP style depending on protocol.
  // If you need to force a serialization style, use one of these classes:
  typedef yarp::os::idl::BareStyle<Rpc> rosStyle;
  typedef yarp::os::idl::BottleStyle<Rpc> bottleStyle;

  // Give source text for class, ROS will need this
  yarp::os::ConstString getTypeText() {
    return "#comment\n\
int8 PIPPO=1\n\
int8 PLUTO=2\n\
#comment\n\
int8 pluppo\n\
string msg\n\
---\n\
#comment\n\
uint32 CODE=54321\n\
#comment\n\
string val\n\
uint32 an_int";
  }

  // Name the class, ROS will need this
  yarp::os::Type getType() {
    yarp::os::Type typ = yarp::os::Type::byName("Rpc","Rpc");
    typ.addProperty("md5sum",yarp::os::Value("4a4bad01e2813cbb43a1cb139fa07847"));
    typ.addProperty("message_definition",yarp::os::Value(getTypeText()));
    return typ;
  }
};

#endif

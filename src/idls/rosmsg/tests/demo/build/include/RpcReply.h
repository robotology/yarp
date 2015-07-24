// This is an automatically generated file.
// Generated from this RpcReply.msg definition:
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARPMSG_TYPE_RpcReply
#define YARPMSG_TYPE_RpcReply

#include <string>
#include <vector>
#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>

class RpcReply : public yarp::os::idl::WirePortable {
public:
  static const yarp::os::NetUint32 CODE = 54321;
  std::string val;
  yarp::os::NetUint32 an_int;

  RpcReply() {
  }

  bool readBare(yarp::os::ConnectionReader& connection) {
    // *** val ***
    int len = connection.expectInt();
    val.resize(len);
    if (!connection.expectBlock((char*)val.c_str(),len)) return false;

    // *** an_int ***
    an_int = connection.expectInt();
    return !connection.isError();
  }

  bool readBottle(yarp::os::ConnectionReader& connection) {
    connection.convertTextMode();
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListHeader(3)) return false;

    // *** val ***
    if (!reader.readString(val)) return false;

    // *** an_int ***
    an_int = reader.expectInt();
    return !connection.isError();
  }

  bool read(yarp::os::ConnectionReader& connection) {
    if (connection.isBareMode()) return readBare(connection);
    return readBottle(connection);
  }

  bool writeBare(yarp::os::ConnectionWriter& connection) {
    // *** val ***
    connection.appendInt(val.length());
    connection.appendExternalBlock((char*)val.c_str(),val.length());

    // *** an_int ***
    connection.appendInt(an_int);
    return !connection.isError();
  }

  bool writeBottle(yarp::os::ConnectionWriter& connection) {
    connection.appendInt(BOTTLE_TAG_LIST);
    connection.appendInt(3);

    // *** val ***
    connection.appendInt(BOTTLE_TAG_STRING);
    connection.appendInt(val.length()+1);
    connection.appendExternalBlock((char*)val.c_str(),val.length()+1);

    // *** an_int ***
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt((int)an_int);
    connection.convertTextMode();
    return !connection.isError();
  }

  bool write(yarp::os::ConnectionWriter& connection) {
    if (connection.isBareMode()) return writeBare(connection);
    return writeBottle(connection);
  }

  // This class will serialize ROS style or YARP style depending on protocol.
  // If you need to force a serialization style, use one of these classes:
  typedef yarp::os::idl::BareStyle<RpcReply> rosStyle;
  typedef yarp::os::idl::BottleStyle<RpcReply> bottleStyle;

  // Give source text for class, ROS will need this
  yarp::os::ConstString getTypeText() {
    return "";
  }

  // Name the class, ROS will need this
  yarp::os::Type getType() {
    yarp::os::Type typ = yarp::os::Type::byName("RpcReply","RpcReply");
    typ.addProperty("md5sum",yarp::os::Value("c9562f24893749e1e9fb4e66d514814b"));
    typ.addProperty("message_definition",yarp::os::Value(getTypeText()));
    return typ;
  }
};

#endif

// This is an automatically generated file.
// Generated from this Tennis.msg definition:
//   int32 x
//   int32 y
//   ---
//   int32 response
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARPMSG_TYPE_Tennis
#define YARPMSG_TYPE_Tennis

#include <string>
#include <vector>
#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <TennisReply.h>

class Tennis : public yarp::os::idl::WirePortable {
public:
  yarp::os::NetInt32 x;
  yarp::os::NetInt32 y;

  Tennis() {
  }

  bool readBare(yarp::os::ConnectionReader& connection) {
    // *** x ***
    x = connection.expectInt();

    // *** y ***
    y = connection.expectInt();
    return !connection.isError();
  }

  bool readBottle(yarp::os::ConnectionReader& connection) {
    connection.convertTextMode();
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListHeader(2)) return false;

    // *** x ***
    x = reader.expectInt();

    // *** y ***
    y = reader.expectInt();
    return !connection.isError();
  }

  bool read(yarp::os::ConnectionReader& connection) {
    if (connection.isBareMode()) return readBare(connection);
    return readBottle(connection);
  }

  bool writeBare(yarp::os::ConnectionWriter& connection) {
    // *** x ***
    connection.appendInt(x);

    // *** y ***
    connection.appendInt(y);
    return !connection.isError();
  }

  bool writeBottle(yarp::os::ConnectionWriter& connection) {
    connection.appendInt(BOTTLE_TAG_LIST);
    connection.appendInt(2);

    // *** x ***
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt((int)x);

    // *** y ***
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt((int)y);
    connection.convertTextMode();
    return !connection.isError();
  }

  bool write(yarp::os::ConnectionWriter& connection) {
    if (connection.isBareMode()) return writeBare(connection);
    return writeBottle(connection);
  }

  // This class will serialize ROS style or YARP style depending on protocol.
  // If you need to force a serialization style, use one of these classes:
  typedef yarp::os::idl::BareStyle<Tennis> rosStyle;
  typedef yarp::os::idl::BottleStyle<Tennis> bottleStyle;

  // Give source text for class, ROS will need this
  yarp::os::ConstString getTypeText() {
    return "int32 x\n\
int32 y\n\
---\n\
int32 response";
  }

  // Name the class, ROS will need this
  yarp::os::Type getType() {
    yarp::os::Type typ = yarp::os::Type::byName("Tennis","Tennis");
    typ.addProperty("md5sum",yarp::os::Value("bd7b43fd41d4c47bf5c703cc7d016709"));
    typ.addProperty("message_definition",yarp::os::Value(getTypeText()));
    return typ;
  }
};

#endif

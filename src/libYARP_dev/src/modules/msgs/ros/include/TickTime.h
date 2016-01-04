// This is an automatically generated file.
// Generated from this TickTime.msg definition:
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARPMSG_TYPE_TickTime
#define YARPMSG_TYPE_TickTime

#include <string>
#include <vector>
#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>

class TickTime : public yarp::os::idl::WirePortable {
public:
  yarp::os::NetUint32 sec;
  yarp::os::NetUint32 nsec;

  TickTime() {
  }

  bool readBare(yarp::os::ConnectionReader& connection) {
    // *** sec ***
    sec = connection.expectInt();

    // *** nsec ***
    nsec = connection.expectInt();
    return !connection.isError();
  }

  bool readBottle(yarp::os::ConnectionReader& connection) {
    connection.convertTextMode();
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListHeader(2)) return false;

    // *** sec ***
    sec = reader.expectInt();

    // *** nsec ***
    nsec = reader.expectInt();
    return !connection.isError();
  }

  using yarp::os::idl::WirePortable::read;
  bool read(yarp::os::ConnectionReader& connection) {
    if (connection.isBareMode()) return readBare(connection);
    return readBottle(connection);
  }

  bool writeBare(yarp::os::ConnectionWriter& connection) {
    // *** sec ***
    connection.appendInt(sec);

    // *** nsec ***
    connection.appendInt(nsec);
    return !connection.isError();
  }

  bool writeBottle(yarp::os::ConnectionWriter& connection) {
    connection.appendInt(BOTTLE_TAG_LIST);
    connection.appendInt(2);

    // *** sec ***
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt((int)sec);

    // *** nsec ***
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt((int)nsec);
    connection.convertTextMode();
    return !connection.isError();
  }

  using yarp::os::idl::WirePortable::write;
  bool write(yarp::os::ConnectionWriter& connection) {
    if (connection.isBareMode()) return writeBare(connection);
    return writeBottle(connection);
  }

  // This class will serialize ROS style or YARP style depending on protocol.
  // If you need to force a serialization style, use one of these classes:
  typedef yarp::os::idl::BareStyle<TickTime> rosStyle;
  typedef yarp::os::idl::BottleStyle<TickTime> bottleStyle;

  // Give source text for class, ROS will need this
  yarp::os::ConstString getTypeText() {
    return "";
  }

  // Name the class, ROS will need this
  yarp::os::Type getType() {
    yarp::os::Type typ = yarp::os::Type::byName("TickTime","TickTime");
    typ.addProperty("md5sum",yarp::os::Value("4f8dc7710c22b42c7b09295dcda33fa0"));
    typ.addProperty("message_definition",yarp::os::Value(getTypeText()));
    return typ;
  }
};

#endif

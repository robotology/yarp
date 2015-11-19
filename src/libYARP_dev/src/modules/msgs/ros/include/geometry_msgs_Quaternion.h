// This is an automatically generated file.
// Generated from this geometry_msgs_Quaternion.msg definition:
//   # This represents an orientation in free space in quaternion form.
//
//   float64 x
//   float64 y
//   float64 z
//   float64 w
//
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARPMSG_TYPE_geometry_msgs_Quaternion
#define YARPMSG_TYPE_geometry_msgs_Quaternion

#include <string>
#include <vector>
#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include "TickTime.h"
#include "Header.h"

class geometry_msgs_Quaternion : public yarp::os::idl::WirePortable {
public:
  yarp::os::NetFloat64 x;
  yarp::os::NetFloat64 y;
  yarp::os::NetFloat64 z;
  yarp::os::NetFloat64 w;

  geometry_msgs_Quaternion() {
  }

  bool readBare(yarp::os::ConnectionReader& connection) {
    // *** x ***
    x = connection.expectDouble();

    // *** y ***
    y = connection.expectDouble();

    // *** z ***
    z = connection.expectDouble();

    // *** w ***
    w = connection.expectDouble();
    return !connection.isError();
  }

  bool readBottle(yarp::os::ConnectionReader& connection) {
    connection.convertTextMode();
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListHeader(4)) return false;

    // *** x ***
    x = reader.expectDouble();

    // *** y ***
    y = reader.expectDouble();

    // *** z ***
    z = reader.expectDouble();

    // *** w ***
    w = reader.expectDouble();
    return !connection.isError();
  }

  bool read(yarp::os::ConnectionReader& connection) {
    if (connection.isBareMode()) return readBare(connection);
    return readBottle(connection);
  }

  bool writeBare(yarp::os::ConnectionWriter& connection) {
    // *** x ***
    connection.appendDouble(x);

    // *** y ***
    connection.appendDouble(y);

    // *** z ***
    connection.appendDouble(z);

    // *** w ***
    connection.appendDouble(w);
    return !connection.isError();
  }

  bool writeBottle(yarp::os::ConnectionWriter& connection) {
    connection.appendInt(BOTTLE_TAG_LIST);
    connection.appendInt(4);

    // *** x ***
    connection.appendInt(BOTTLE_TAG_DOUBLE);
    connection.appendDouble((double)x);

    // *** y ***
    connection.appendInt(BOTTLE_TAG_DOUBLE);
    connection.appendDouble((double)y);

    // *** z ***
    connection.appendInt(BOTTLE_TAG_DOUBLE);
    connection.appendDouble((double)z);

    // *** w ***
    connection.appendInt(BOTTLE_TAG_DOUBLE);
    connection.appendDouble((double)w);
    connection.convertTextMode();
    return !connection.isError();
  }

  bool write(yarp::os::ConnectionWriter& connection) {
    if (connection.isBareMode()) return writeBare(connection);
    return writeBottle(connection);
  }

  // This class will serialize ROS style or YARP style depending on protocol.
  // If you need to force a serialization style, use one of these classes:
  typedef yarp::os::idl::BareStyle<geometry_msgs_Quaternion> rosStyle;
  typedef yarp::os::idl::BottleStyle<geometry_msgs_Quaternion> bottleStyle;

  // Give source text for class, ROS will need this
  yarp::os::ConstString getTypeText() {
    return "# This represents an orientation in free space in quaternion form.\n\
\n\
float64 x\n\
float64 y\n\
float64 z\n\
float64 w\n\
";
  }

  // Name the class, ROS will need this
  yarp::os::Type getType() {
    yarp::os::Type typ = yarp::os::Type::byName("geometry_msgs/Quaternion","geometry_msgs/Quaternion");
    typ.addProperty("md5sum",yarp::os::Value("a779879fadf0160734f906b8c19c7004"));
    typ.addProperty("message_definition",yarp::os::Value(getTypeText()));
    return typ;
  }
};

#endif

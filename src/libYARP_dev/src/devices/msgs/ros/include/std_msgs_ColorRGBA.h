// This is an automatically generated file.
// Generated from this std_msgs_ColorRGBA.msg definition:
//   float32 r
//   float32 g
//   float32 b
//   float32 a
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARPMSG_TYPE_std_msgs_ColorRGBA
#define YARPMSG_TYPE_std_msgs_ColorRGBA

#include <string>
#include <vector>
#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include "TickTime.h"
#include "std_msgs_Header.h"
#include "geometry_msgs_Point.h"
#include "geometry_msgs_Quaternion.h"
#include "geometry_msgs_Pose.h"
#include "geometry_msgs_Vector3.h"

class std_msgs_ColorRGBA : public yarp::os::idl::WirePortable {
public:
  yarp::os::NetFloat32 r;
  yarp::os::NetFloat32 g;
  yarp::os::NetFloat32 b;
  yarp::os::NetFloat32 a;

  std_msgs_ColorRGBA() {
  }

  bool readBare(yarp::os::ConnectionReader& connection) {
    // *** r ***
    if (!connection.expectBlock((char*)&r,4)) return false;

    // *** g ***
    if (!connection.expectBlock((char*)&g,4)) return false;

    // *** b ***
    if (!connection.expectBlock((char*)&b,4)) return false;

    // *** a ***
    if (!connection.expectBlock((char*)&a,4)) return false;
    return !connection.isError();
  }

  bool readBottle(yarp::os::ConnectionReader& connection) {
    connection.convertTextMode();
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListHeader(4)) return false;

    // *** r ***
    r = reader.expectDouble();

    // *** g ***
    g = reader.expectDouble();

    // *** b ***
    b = reader.expectDouble();

    // *** a ***
    a = reader.expectDouble();
    return !connection.isError();
  }

  using yarp::os::idl::WirePortable::read;
  bool read(yarp::os::ConnectionReader& connection) {
    if (connection.isBareMode()) return readBare(connection);
    return readBottle(connection);
  }

  bool writeBare(yarp::os::ConnectionWriter& connection) {
    // *** r ***
    connection.appendBlock((char*)&r,4);

    // *** g ***
    connection.appendBlock((char*)&g,4);

    // *** b ***
    connection.appendBlock((char*)&b,4);

    // *** a ***
    connection.appendBlock((char*)&a,4);
    return !connection.isError();
  }

  bool writeBottle(yarp::os::ConnectionWriter& connection) {
    connection.appendInt(BOTTLE_TAG_LIST);
    connection.appendInt(4);

    // *** r ***
    connection.appendInt(BOTTLE_TAG_DOUBLE);
    connection.appendDouble((double)r);

    // *** g ***
    connection.appendInt(BOTTLE_TAG_DOUBLE);
    connection.appendDouble((double)g);

    // *** b ***
    connection.appendInt(BOTTLE_TAG_DOUBLE);
    connection.appendDouble((double)b);

    // *** a ***
    connection.appendInt(BOTTLE_TAG_DOUBLE);
    connection.appendDouble((double)a);
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
  typedef yarp::os::idl::BareStyle<std_msgs_ColorRGBA> rosStyle;
  typedef yarp::os::idl::BottleStyle<std_msgs_ColorRGBA> bottleStyle;

  // Give source text for class, ROS will need this
  yarp::os::ConstString getTypeText() {
    return "float32 r\n\
float32 g\n\
float32 b\n\
float32 a";
  }

  // Name the class, ROS will need this
  yarp::os::Type getType() {
    yarp::os::Type typ = yarp::os::Type::byName("std_msgs/ColorRGBA","std_msgs/ColorRGBA");
    typ.addProperty("md5sum",yarp::os::Value("a29a96539573343b1310c73607334b00"));
    typ.addProperty("message_definition",yarp::os::Value(getTypeText()));
    return typ;
  }
};

#endif

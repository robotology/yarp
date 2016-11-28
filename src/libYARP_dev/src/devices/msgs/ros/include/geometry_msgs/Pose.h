// This is an automatically generated file.
// Generated from this geometry_msgs_Pose.msg definition:
//   geometry_msgs/Point position
//   geometry_msgs/Quaternion orientation
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARPMSG_TYPE_geometry_msgs_Pose
#define YARPMSG_TYPE_geometry_msgs_Pose

#include <string>
#include <vector>
#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include "TickTime.h"
#include "std_msgs_Header.h"
#include "geometry_msgs_Point.h"
#include "geometry_msgs_Quaternion.h"

class geometry_msgs_Pose : public yarp::os::idl::WirePortable {
public:
  geometry_msgs_Point position;
  geometry_msgs_Quaternion orientation;

  geometry_msgs_Pose() {
  }

  bool readBare(yarp::os::ConnectionReader& connection) {
    // *** position ***
    if (!position.read(connection)) return false;

    // *** orientation ***
    if (!orientation.read(connection)) return false;
    return !connection.isError();
  }

  bool readBottle(yarp::os::ConnectionReader& connection) {
    connection.convertTextMode();
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListHeader(2)) return false;

    // *** position ***
    if (!position.read(connection)) return false;

    // *** orientation ***
    if (!orientation.read(connection)) return false;
    return !connection.isError();
  }

  using yarp::os::idl::WirePortable::read;
  bool read(yarp::os::ConnectionReader& connection) {
    if (connection.isBareMode()) return readBare(connection);
    return readBottle(connection);
  }

  bool writeBare(yarp::os::ConnectionWriter& connection) {
    // *** position ***
    if (!position.write(connection)) return false;

    // *** orientation ***
    if (!orientation.write(connection)) return false;
    return !connection.isError();
  }

  bool writeBottle(yarp::os::ConnectionWriter& connection) {
    connection.appendInt(BOTTLE_TAG_LIST);
    connection.appendInt(2);

    // *** position ***
    if (!position.write(connection)) return false;

    // *** orientation ***
    if (!orientation.write(connection)) return false;
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
  typedef yarp::os::idl::BareStyle<geometry_msgs_Pose> rosStyle;
  typedef yarp::os::idl::BottleStyle<geometry_msgs_Pose> bottleStyle;

  // Give source text for class, ROS will need this
  yarp::os::ConstString getTypeText() {
    return "geometry_msgs/Point position\n\
geometry_msgs/Quaternion orientation\n================================================================================\n\
MSG: geometry_msgs/Point\n\
float64 x\n\
float64 y\n\
float64 z\n================================================================================\n\
MSG: geometry_msgs/Quaternion\n\
float64 x\n\
float64 y\n\
float64 z\n\
float64 w";
  }

  // Name the class, ROS will need this
  yarp::os::Type getType() {
    yarp::os::Type typ = yarp::os::Type::byName("geometry_msgs/Pose","geometry_msgs/Pose");
    typ.addProperty("md5sum",yarp::os::Value("e45d45a5a1ce597b249e23fb30fc871f"));
    typ.addProperty("message_definition",yarp::os::Value(getTypeText()));
    return typ;
  }
};

#endif

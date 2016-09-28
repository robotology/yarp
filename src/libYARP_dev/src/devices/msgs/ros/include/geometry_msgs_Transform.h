// This is an automatically generated file.
// Generated from this geometry_msgs_Transform.msg definition:
//   # This represents the transform between two coordinate frames in free space.
//   
//   Vector3 translation
//   Quaternion rotation
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARPMSG_TYPE_geometry_msgs_Transform
#define YARPMSG_TYPE_geometry_msgs_Transform

#include <string>
#include <vector>
#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include "TickTime.h"
#include "std_msgs_Header.h"
#include "geometry_msgs_Vector3.h"
#include "geometry_msgs_Quaternion.h"

class geometry_msgs_Transform : public yarp::os::idl::WirePortable {
public:
  geometry_msgs_Vector3 translation;
  geometry_msgs_Quaternion rotation;

  geometry_msgs_Transform() {
  }

  bool readBare(yarp::os::ConnectionReader& connection) {
    // *** translation ***
    if (!translation.read(connection)) return false;

    // *** rotation ***
    if (!rotation.read(connection)) return false;
    return !connection.isError();
  }

  bool readBottle(yarp::os::ConnectionReader& connection) {
    connection.convertTextMode();
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListHeader(2)) return false;

    // *** translation ***
    if (!translation.read(connection)) return false;

    // *** rotation ***
    if (!rotation.read(connection)) return false;
    return !connection.isError();
  }

  using yarp::os::idl::WirePortable::read;
  bool read(yarp::os::ConnectionReader& connection) {
    if (connection.isBareMode()) return readBare(connection);
    return readBottle(connection);
  }

  bool writeBare(yarp::os::ConnectionWriter& connection) {
    // *** translation ***
    if (!translation.write(connection)) return false;

    // *** rotation ***
    if (!rotation.write(connection)) return false;
    return !connection.isError();
  }

  bool writeBottle(yarp::os::ConnectionWriter& connection) {
    connection.appendInt(BOTTLE_TAG_LIST);
    connection.appendInt(2);

    // *** translation ***
    if (!translation.write(connection)) return false;

    // *** rotation ***
    if (!rotation.write(connection)) return false;
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
  typedef yarp::os::idl::BareStyle<geometry_msgs_Transform> rosStyle;
  typedef yarp::os::idl::BottleStyle<geometry_msgs_Transform> bottleStyle;

  // Give source text for class, ROS will need this
  yarp::os::ConstString getTypeText() {
    return "# This represents the transform between two coordinate frames in free space.\n\
\n\
Vector3 translation\n\
Quaternion rotation\n================================================================================\n\
MSG: geometry_msgs/Vector3\n\
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
    yarp::os::Type typ = yarp::os::Type::byName("geometry_msgs/Transform","geometry_msgs/Transform");
    typ.addProperty("md5sum",yarp::os::Value("ac9eff44abf714214112b05d54a3cf9b"));
    typ.addProperty("message_definition",yarp::os::Value(getTypeText()));
    return typ;
  }
};

#endif

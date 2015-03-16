// This is an automatically generated file.
// Generated from this geometry_msgs_Wrench.msg definition:
//   # This represents force in free space, separated into
//   # its linear and angular parts.
//   Vector3  force
//   Vector3  torque
//   
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARPMSG_TYPE_geometry_msgs_Wrench
#define YARPMSG_TYPE_geometry_msgs_Wrench

#include <string>
#include <vector>
#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include "TickTime.h"
#include "Header.h"
#include "geometry_msgs_Vector3.h"

class geometry_msgs_Wrench : public yarp::os::idl::WirePortable {
public:
  geometry_msgs_Vector3 force;
  geometry_msgs_Vector3 torque;

  geometry_msgs_Wrench() {
  }

  bool readBare(yarp::os::ConnectionReader& connection) {
    // *** force ***
    if (!force.read(connection)) return false;

    // *** torque ***
    if (!torque.read(connection)) return false;
    return !connection.isError();
  }

  bool readBottle(yarp::os::ConnectionReader& connection) {
    connection.convertTextMode();
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListHeader(2)) return false;

    // *** force ***
    if (!force.read(connection)) return false;

    // *** torque ***
    if (!torque.read(connection)) return false;
    return !connection.isError();
  }

  bool read(yarp::os::ConnectionReader& connection) {
    if (connection.isBareMode()) return readBare(connection);
    return readBottle(connection);
  }

  bool writeBare(yarp::os::ConnectionWriter& connection) {
    // *** force ***
    if (!force.write(connection)) return false;

    // *** torque ***
    if (!torque.write(connection)) return false;
    return !connection.isError();
  }

  bool writeBottle(yarp::os::ConnectionWriter& connection) {
    connection.appendInt(BOTTLE_TAG_LIST);
    connection.appendInt(2);

    // *** force ***
    if (!force.write(connection)) return false;

    // *** torque ***
    if (!torque.write(connection)) return false;
    connection.convertTextMode();
    return !connection.isError();
  }

  bool write(yarp::os::ConnectionWriter& connection) {
    if (connection.isBareMode()) return writeBare(connection);
    return writeBottle(connection);
  }

  // This class will serialize ROS style or YARP style depending on protocol.
  // If you need to force a serialization style, use one of these classes:
  typedef yarp::os::idl::BareStyle<geometry_msgs_Wrench> rosStyle;
  typedef yarp::os::idl::BottleStyle<geometry_msgs_Wrench> bottleStyle;

  // Give source text for class, ROS will need this
  yarp::os::ConstString getTypeText() {
    return "# This represents force in free space, separated into\n\
# its linear and angular parts.\n\
Vector3  force\n\
Vector3  torque\n\
\n================================================================================\n\
MSG: geometry_msgs/Vector3\n\
# This represents a vector in free space. \n\
\n\
float64 x\n\
float64 y\n\
float64 z";
  }

  // Name the class, ROS will need this
  yarp::os::Type getType() {
    yarp::os::Type typ = yarp::os::Type::byName("geometry_msgs/Wrench","geometry_msgs/Wrench");
    typ.addProperty("md5sum",yarp::os::Value("4f539cf138b23283b520fd271b567936"));
    typ.addProperty("message_definition",yarp::os::Value(getTypeText()));
    return typ;
  }
};

#endif

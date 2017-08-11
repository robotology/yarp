// This is an automatically generated file.
// Generated from this geometry_msgs_TransformStamped.msg definition:
//   # This expresses a transform from coordinate frame header.frame_id
//   # to the coordinate frame child_frame_id
//   #
//   # This message is mostly used by the 
//   # tf package. 
//   # See it's documentation for more information.
//   
//   Header header
//   string child_frame_id # the frame id of the child frame
//   Transform transform
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARPMSG_TYPE_geometry_msgs_TransformStamped
#define YARPMSG_TYPE_geometry_msgs_TransformStamped

#include <string>
#include <vector>
#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include "TickTime.h"
#include "std_msgs_Header.h"
#include "geometry_msgs_Vector3.h"
#include "geometry_msgs_Quaternion.h"
#include "geometry_msgs_Transform.h"

class geometry_msgs_TransformStamped : public yarp::os::idl::WirePortable {
public:
  std_msgs_Header header;
  std::string child_frame_id;
  geometry_msgs_Transform transform;

  geometry_msgs_TransformStamped() {
  }

  void clear() {
    // *** header ***
    header.clear();

    // *** child_frame_id ***
    child_frame_id = "";

    // *** transform ***
    transform.clear();
  }

  bool readBare(yarp::os::ConnectionReader& connection) override {
    // *** header ***
    if (!header.read(connection)) return false;

    // *** child_frame_id ***
    int len = connection.expectInt();
    child_frame_id.resize(len);
    if (!connection.expectBlock((char*)child_frame_id.c_str(),len)) return false;

    // *** transform ***
    if (!transform.read(connection)) return false;
    return !connection.isError();
  }

  bool readBottle(yarp::os::ConnectionReader& connection) override {
    connection.convertTextMode();
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListHeader(3)) return false;

    // *** header ***
    if (!header.read(connection)) return false;

    // *** child_frame_id ***
    if (!reader.readString(child_frame_id)) return false;

    // *** transform ***
    if (!transform.read(connection)) return false;
    return !connection.isError();
  }

  using yarp::os::idl::WirePortable::read;
  bool read(yarp::os::ConnectionReader& connection) override {
    if (connection.isBareMode()) return readBare(connection);
    return readBottle(connection);
  }

  bool writeBare(yarp::os::ConnectionWriter& connection) override {
    // *** header ***
    if (!header.write(connection)) return false;

    // *** child_frame_id ***
    connection.appendInt(child_frame_id.length());
    connection.appendExternalBlock((char*)child_frame_id.c_str(),child_frame_id.length());

    // *** transform ***
    if (!transform.write(connection)) return false;
    return !connection.isError();
  }

  bool writeBottle(yarp::os::ConnectionWriter& connection) override {
    connection.appendInt(BOTTLE_TAG_LIST);
    connection.appendInt(3);

    // *** header ***
    if (!header.write(connection)) return false;

    // *** child_frame_id ***
    connection.appendInt(BOTTLE_TAG_STRING);
    connection.appendInt(child_frame_id.length());
    connection.appendExternalBlock((char*)child_frame_id.c_str(),child_frame_id.length());

    // *** transform ***
    if (!transform.write(connection)) return false;
    connection.convertTextMode();
    return !connection.isError();
  }

  using yarp::os::idl::WirePortable::write;
  bool write(yarp::os::ConnectionWriter& connection) override {
    if (connection.isBareMode()) return writeBare(connection);
    return writeBottle(connection);
  }

  // This class will serialize ROS style or YARP style depending on protocol.
  // If you need to force a serialization style, use one of these classes:
  typedef yarp::os::idl::BareStyle<geometry_msgs_TransformStamped> rosStyle;
  typedef yarp::os::idl::BottleStyle<geometry_msgs_TransformStamped> bottleStyle;

  // Give source text for class, ROS will need this
  yarp::os::ConstString getTypeText() {
    return "# This expresses a transform from coordinate frame header.frame_id\n\
# to the coordinate frame child_frame_id\n\
#\n\
# This message is mostly used by the \n\
# tf package. \n\
# See it's documentation for more information.\n\
\n\
Header header\n\
string child_frame_id # the frame id of the child frame\n\
Transform transform\n================================================================================\n\
MSG: std_msgs/Header\n\
[std_msgs/Header]:\n\
# Standard metadata for higher-level stamped data types.\n\
# This is generally used to communicate timestamped data\n\
# in a particular coordinate frame.\n\
#\n\
# sequence ID: consecutively increasing ID\n\
uint32 seq\n\
#Two-integer timestamp that is expressed as:\n\
# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')\n\
# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')\n\
# time-handling sugar is provided by the client library\n\
time stamp\n\
#Frame this data is associated with\n\
# 0: no frame\n\
# 1: global frame\n\
string frame_id\n================================================================================\n\
MSG: geometry_msgs/Transform\n\
# This represents the transform between two coordinate frames in free space.\n\
\n\
Vector3 translation\n\
Quaternion rotation\n================================================================================\n\
MSG: geometry_msgs/Vector3\n\
# This represents a vector in free space.\n\
\n\
float64 x\n\
float64 y\n\
float64 z\n================================================================================\n\
MSG: geometry_msgs/Quaternion\n\
# This represents an orientation in free space in quaternion form.\n\
\n\
float64 x\n\
float64 y\n\
float64 z\n\
float64 w";
  }

  // Name the class, ROS will need this
  yarp::os::Type getType() override {
    yarp::os::Type typ = yarp::os::Type::byName("geometry_msgs/TransformStamped","geometry_msgs/TransformStamped");
    typ.addProperty("md5sum",yarp::os::Value("b5764a33bfeb3588febc2682852579b0"));
    typ.addProperty("message_definition",yarp::os::Value(getTypeText()));
    return typ;
  }
};

#endif

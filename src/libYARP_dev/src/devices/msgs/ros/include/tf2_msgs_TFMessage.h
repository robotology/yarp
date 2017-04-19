// This is an automatically generated file.
// Generated from this tf2_msgs_TFMessage.msg definition:
//   geometry_msgs/TransformStamped[] transforms
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARPMSG_TYPE_tf2_msgs_TFMessage
#define YARPMSG_TYPE_tf2_msgs_TFMessage

#include <string>
#include <vector>
#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include "TickTime.h"
#include "std_msgs_Header.h"
#include "geometry_msgs_Vector3.h"
#include "geometry_msgs_Quaternion.h"
#include "geometry_msgs_Transform.h"
#include "geometry_msgs_TransformStamped.h"

class tf2_msgs_TFMessage : public yarp::os::idl::WirePortable {
public:
  std::vector<geometry_msgs_TransformStamped> transforms;

  tf2_msgs_TFMessage() {
  }

  bool readBare(yarp::os::ConnectionReader& connection) {
    // *** transforms ***
    int len = connection.expectInt();
    transforms.resize(len);
    for (int i=0; i<len; i++) {
      if (!transforms[i].read(connection)) return false;
    }
    return !connection.isError();
  }

  bool readBottle(yarp::os::ConnectionReader& connection) {
    connection.convertTextMode();
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListHeader(1)) return false;

    // *** transforms ***
    if (connection.expectInt()!=BOTTLE_TAG_LIST) return false;
    int len = connection.expectInt();
    transforms.resize(len);
    for (int i=0; i<len; i++) {
      if (!transforms[i].read(connection)) return false;
    }
    return !connection.isError();
  }

  using yarp::os::idl::WirePortable::read;
  bool read(yarp::os::ConnectionReader& connection) {
    if (connection.isBareMode()) return readBare(connection);
    return readBottle(connection);
  }

  bool writeBare(yarp::os::ConnectionWriter& connection) {
    // *** transforms ***
    connection.appendInt(transforms.size());
    for (size_t i=0; i<transforms.size(); i++) {
      if (!transforms[i].write(connection)) return false;
    }
    return !connection.isError();
  }

  bool writeBottle(yarp::os::ConnectionWriter& connection) {
    connection.appendInt(BOTTLE_TAG_LIST);
    connection.appendInt(1);

    // *** transforms ***
    connection.appendInt(BOTTLE_TAG_LIST);
    connection.appendInt(transforms.size());
    for (size_t i=0; i<transforms.size(); i++) {
      if (!transforms[i].write(connection)) return false;
    }
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
  typedef yarp::os::idl::BareStyle<tf2_msgs_TFMessage> rosStyle;
  typedef yarp::os::idl::BottleStyle<tf2_msgs_TFMessage> bottleStyle;

  // Give source text for class, ROS will need this
  yarp::os::ConstString getTypeText() {
    return "geometry_msgs/TransformStamped[] transforms\n================================================================================\n\
MSG: geometry_msgs/TransformStamped\n\
# This expresses a transform from coordinate frame header.frame_id\n\
# to the coordinate frame child_frame_id\n\
#\n\
# This message is mostly used by the \n\
# <a href=\"http://wiki.ros.org/tf\">tf</a> package. \n\
# See its documentation for more information.\n\
\n\
Header header\n\
string child_frame_id # the frame id of the child frame\n\
Transform transform\n\
\n================================================================================\n\
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
Quaternion rotation\n\
\n================================================================================\n\
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
  yarp::os::Type getType() {
    yarp::os::Type typ = yarp::os::Type::byName("tf2_msgs/TFMessage","tf2_msgs/TFMessage");
    typ.addProperty("md5sum",yarp::os::Value("94810edda583a504dfda3829e70d7eec"));
    typ.addProperty("message_definition",yarp::os::Value(getTypeText()));
    return typ;
  }
};

#endif

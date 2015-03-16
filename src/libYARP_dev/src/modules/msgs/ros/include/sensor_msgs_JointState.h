// This is an automatically generated file.
// Generated from this sensor_msgs_JointState.msg definition:
//   # This is a message that holds data to describe the state of a set of torque controlled joints.
//   #
//   # The state of each joint (revolute or prismatic) is defined by:
//   #  * the position of the joint (rad or m),
//   #  * the velocity of the joint (rad/s or m/s) and
//   #  * the effort that is applied in the joint (Nm or N).
//   #
//   # Each joint is uniquely identified by its name
//   # The header specifies the time at which the joint states were recorded. All the joint states
//   # in one message have to be recorded at the same time.
//   #
//   # This message consists of a multiple arrays, one for each part of the joint state.
//   # The goal is to make each of the fields optional. When e.g. your joints have no
//   # effort associated with them, you can leave the effort array empty.
//   #
//   # All arrays in this message should have the same size, or be empty.
//   # This is the only way to uniquely associate the joint name with the correct
//   # states.
//
//
//   Header header
//
//   string[] name
//   float64[] position
//   float64[] velocity
//   float64[] effort
//
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARPMSG_TYPE_sensor_msgs_JointState
#define YARPMSG_TYPE_sensor_msgs_JointState

#include <string>
#include <vector>
#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include "TickTime.h"
#include "Header.h"

class sensor_msgs_JointState : public yarp::os::idl::WirePortable {
public:
  Header header;
  std::vector<std::string> name;
  std::vector<yarp::os::NetFloat64> position;
  std::vector<yarp::os::NetFloat64> velocity;
  std::vector<yarp::os::NetFloat64> effort;

  sensor_msgs_JointState() {
  }

  bool readBare(yarp::os::ConnectionReader& connection) {
    // *** header ***
    if (!header.read(connection)) return false;

    // *** name ***
    int len = connection.expectInt();
    name.resize(len);
    for (int i=0; i<len; i++) {
      int len2 = connection.expectInt();
      name[i].resize(len2);
      if (!connection.expectBlock((char*)name[i].c_str(),len2)) return false;
    }

    // *** position ***
    len = connection.expectInt();
    position.resize(len);
    if (!connection.expectBlock((char*)&position[0],sizeof(yarp::os::NetFloat64)*len)) return false;

    // *** velocity ***
    len = connection.expectInt();
    velocity.resize(len);
    if (!connection.expectBlock((char*)&velocity[0],sizeof(yarp::os::NetFloat64)*len)) return false;

    // *** effort ***
    len = connection.expectInt();
    effort.resize(len);
    if (!connection.expectBlock((char*)&effort[0],sizeof(yarp::os::NetFloat64)*len)) return false;
    return !connection.isError();
  }

  bool readBottle(yarp::os::ConnectionReader& connection) {
    connection.convertTextMode();
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListHeader(5)) return false;

    // *** header ***
    if (!header.read(connection)) return false;

    // *** name ***
    if (connection.expectInt()!=(BOTTLE_TAG_LIST|BOTTLE_TAG_STRING)) return false;
    int len = connection.expectInt();
    name.resize(len);
    for (int i=0; i<len; i++) {
      int len2 = connection.expectInt();
      name[i].resize(len2);
      if (!connection.expectBlock((char*)name[i].c_str(),len2)) return false;
    }

    // *** position ***
    if (connection.expectInt()!=(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE)) return false;
    len = connection.expectInt();
    position.resize(len);
    for (int i=0; i<len; i++) {
      position[i] = (yarp::os::NetFloat64)connection.expectDouble();
    }

    // *** velocity ***
    if (connection.expectInt()!=(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE)) return false;
    len = connection.expectInt();
    velocity.resize(len);
    for (int i=0; i<len; i++) {
      velocity[i] = (yarp::os::NetFloat64)connection.expectDouble();
    }

    // *** effort ***
    if (connection.expectInt()!=(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE)) return false;
    len = connection.expectInt();
    effort.resize(len);
    for (int i=0; i<len; i++) {
      effort[i] = (yarp::os::NetFloat64)connection.expectDouble();
    }
    return !connection.isError();
  }

  bool read(yarp::os::ConnectionReader& connection) {
    if (connection.isBareMode()) return readBare(connection);
    return readBottle(connection);
  }

  bool writeBare(yarp::os::ConnectionWriter& connection) {
    // *** header ***
    if (!header.write(connection)) return false;

    // *** name ***
    connection.appendInt(name.size());
    for (size_t i=0; i<name.size(); i++) {
      connection.appendInt(name[i].length());
      connection.appendExternalBlock((char*)name[i].c_str(),name[i].length());
    }

    // *** position ***
    connection.appendInt(position.size());
    connection.appendExternalBlock((char*)&position[0],sizeof(yarp::os::NetFloat64)*position.size());

    // *** velocity ***
    connection.appendInt(velocity.size());
    connection.appendExternalBlock((char*)&velocity[0],sizeof(yarp::os::NetFloat64)*velocity.size());

    // *** effort ***
    connection.appendInt(effort.size());
    connection.appendExternalBlock((char*)&effort[0],sizeof(yarp::os::NetFloat64)*effort.size());
    return !connection.isError();
  }

  bool writeBottle(yarp::os::ConnectionWriter& connection) {
    connection.appendInt(BOTTLE_TAG_LIST);
    connection.appendInt(5);

    // *** header ***
    if (!header.write(connection)) return false;

    // *** name ***
    connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_STRING);
    connection.appendInt(name.size());
    for (size_t i=0; i<name.size(); i++) {
      connection.appendInt(name[i].length()+1);
      connection.appendExternalBlock((char*)name[i].c_str(),name[i].length()+1);
    }

    // *** position ***
    connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE);
    connection.appendInt(position.size());
    for (size_t i=0; i<position.size(); i++) {
      connection.appendDouble((double)position[i]);
    }

    // *** velocity ***
    connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE);
    connection.appendInt(velocity.size());
    for (size_t i=0; i<velocity.size(); i++) {
      connection.appendDouble((double)velocity[i]);
    }

    // *** effort ***
    connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE);
    connection.appendInt(effort.size());
    for (size_t i=0; i<effort.size(); i++) {
      connection.appendDouble((double)effort[i]);
    }
    connection.convertTextMode();
    return !connection.isError();
  }

  bool write(yarp::os::ConnectionWriter& connection) {
    if (connection.isBareMode()) return writeBare(connection);
    return writeBottle(connection);
  }

  // This class will serialize ROS style or YARP style depending on protocol.
  // If you need to force a serialization style, use one of these classes:
  typedef yarp::os::idl::BareStyle<sensor_msgs_JointState> rosStyle;
  typedef yarp::os::idl::BottleStyle<sensor_msgs_JointState> bottleStyle;

  // Give source text for class, ROS will need this
  yarp::os::ConstString getTypeText() {
    return "# This is a message that holds data to describe the state of a set of torque controlled joints. \n\
#\n\
# The state of each joint (revolute or prismatic) is defined by:\n\
#  * the position of the joint (rad or m),\n\
#  * the velocity of the joint (rad/s or m/s) and \n\
#  * the effort that is applied in the joint (Nm or N).\n\
#\n\
# Each joint is uniquely identified by its name\n\
# The header specifies the time at which the joint states were recorded. All the joint states\n\
# in one message have to be recorded at the same time.\n\
#\n\
# This message consists of a multiple arrays, one for each part of the joint state. \n\
# The goal is to make each of the fields optional. When e.g. your joints have no\n\
# effort associated with them, you can leave the effort array empty. \n\
#\n\
# All arrays in this message should have the same size, or be empty.\n\
# This is the only way to uniquely associate the joint name with the correct\n\
# states.\n\
\n\
\n\
Header header\n\
\n\
string[] name\n\
float64[] position\n\
float64[] velocity\n\
float64[] effort\n\
\n================================================================================\n\
MSG: std_msgs/Header\n\
# Standard metadata for higher-level stamped data types.\n\
# This is generally used to communicate timestamped data \n\
# in a particular coordinate frame.\n\
# \n\
# sequence ID: consecutively increasing ID \n\
uint32 seq\n\
#Two-integer timestamp that is expressed as:\n\
# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')\n\
# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')\n\
# time-handling sugar is provided by the client library\n\
time stamp\n\
#Frame this data is associated with\n\
# 0: no frame\n\
# 1: global frame\n\
string frame_id\n\
";
  }

  // Name the class, ROS will need this
  yarp::os::Type getType() {
    yarp::os::Type typ = yarp::os::Type::byName("sensor_msgs/JointState","sensor_msgs/JointState");
    typ.addProperty("md5sum",yarp::os::Value("3066dcd76a6cfaef579bd0f34173e9fd"));
    typ.addProperty("message_definition",yarp::os::Value(getTypeText()));
    return typ;
  }
};

#endif

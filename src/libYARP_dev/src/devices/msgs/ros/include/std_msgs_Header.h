// This is an automatically generated file.
// Generated from this std_msgs_Header.msg definition:
//   [std_msgs/Header]:
//   # Standard metadata for higher-level stamped data types.
//   # This is generally used to communicate timestamped data
//   # in a particular coordinate frame.
//   #
//   # sequence ID: consecutively increasing ID
//   uint32 seq
//   #Two-integer timestamp that is expressed as:
//   # * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')
//   # * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')
//   # time-handling sugar is provided by the client library
//   time stamp
//   #Frame this data is associated with
//   # 0: no frame
//   # 1: global frame
//   string frame_id
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARPMSG_TYPE_std_msgs_Header
#define YARPMSG_TYPE_std_msgs_Header

#include <string>
#include <vector>
#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include "TickTime.h"

class std_msgs_Header : public yarp::os::idl::WirePortable {
public:
  yarp::os::NetUint32 seq;
  TickTime stamp;
  std::string frame_id;

  std_msgs_Header() {
  }

  bool readBare(yarp::os::ConnectionReader& connection) {
    // *** seq ***
    seq = connection.expectInt();

    // *** stamp ***
    if (!stamp.read(connection)) return false;

    // *** frame_id ***
    int len = connection.expectInt();
    frame_id.resize(len);
    if (!connection.expectBlock((char*)frame_id.c_str(),len)) return false;
    return !connection.isError();
  }

  bool readBottle(yarp::os::ConnectionReader& connection) {
    connection.convertTextMode();
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListHeader(3)) return false;

    // *** seq ***
    seq = reader.expectInt();

    // *** stamp ***
    if (!stamp.read(connection)) return false;

    // *** frame_id ***
    if (!reader.readString(frame_id)) return false;
    return !connection.isError();
  }

  using yarp::os::idl::WirePortable::read;
  bool read(yarp::os::ConnectionReader& connection) {
    if (connection.isBareMode()) return readBare(connection);
    return readBottle(connection);
  }

  bool writeBare(yarp::os::ConnectionWriter& connection) {
    // *** seq ***
    connection.appendInt(seq);

    // *** stamp ***
    if (!stamp.write(connection)) return false;

    // *** frame_id ***
    connection.appendInt(frame_id.length());
    connection.appendExternalBlock((char*)frame_id.c_str(),frame_id.length());
    return !connection.isError();
  }

  bool writeBottle(yarp::os::ConnectionWriter& connection) {
    connection.appendInt(BOTTLE_TAG_LIST);
    connection.appendInt(3);

    // *** seq ***
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt((int)seq);

    // *** stamp ***
    if (!stamp.write(connection)) return false;

    // *** frame_id ***
    connection.appendInt(BOTTLE_TAG_STRING);
    connection.appendInt(frame_id.length());
    connection.appendExternalBlock((char*)frame_id.c_str(),frame_id.length());
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
  typedef yarp::os::idl::BareStyle<std_msgs_Header> rosStyle;
  typedef yarp::os::idl::BottleStyle<std_msgs_Header> bottleStyle;

  // Give source text for class, ROS will need this
  yarp::os::ConstString getTypeText() {
    return "[std_msgs/Header]:\n\
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
string frame_id";
  }

  // Name the class, ROS will need this
  yarp::os::Type getType() {
    yarp::os::Type typ = yarp::os::Type::byName("std_msgs/Header","std_msgs/Header");
    typ.addProperty("md5sum",yarp::os::Value("2176decaecbce78abc3b96ef049fabed"));
    typ.addProperty("message_definition",yarp::os::Value(getTypeText()));
    return typ;
  }
};

#endif

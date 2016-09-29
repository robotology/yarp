// This is an automatically generated file.
// Generated from this sensor_msgs_Image.msg definition:
//   # This message contains an uncompressed image
//   # (0, 0) is at top-left corner of image
//   #
//   
//   Header header        # Header timestamp should be acquisition time of image
//                        # Header frame_id should be optical frame of camera
//                        # origin of frame should be optical center of cameara
//                        # +x should point to the right in the image
//                        # +y should point down in the image
//                        # +z should point into to plane of the image
//                        # If the frame_id here and the frame_id of the CameraInfo
//                        # message associated with the image conflict
//                        # the behavior is undefined
//   
//   uint32 height         # image height, that is, number of rows
//   uint32 width          # image width, that is, number of columns
//   
//   # The legal values for encoding are in file src/image_encodings.cpp
//   # If you want to standardize a new string format, join
//   # ros-users@lists.sourceforge.net and send an email proposing a new encoding.
//   
//   string encoding       # Encoding of pixels -- channel meaning, ordering, size
//                         # taken from the list of strings in include/sensor_msgs/image_encodings.h
//   
//   uint8 is_bigendian    # is this data bigendian?
//   uint32 step           # Full row length in bytes
//   uint8[] data          # actual matrix data, size is (step * rows)
//   
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARPMSG_TYPE_sensor_msgs_Image
#define YARPMSG_TYPE_sensor_msgs_Image

#include <string>
#include <vector>
#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include "TickTime.h"
#include "std_msgs_Header.h"

class sensor_msgs_Image : public yarp::os::idl::WirePortable {
public:
  std_msgs_Header header;
  yarp::os::NetUint32 height;
  yarp::os::NetUint32 width;
  std::string encoding;
  unsigned char is_bigendian;
  yarp::os::NetUint32 step;
  std::vector<unsigned char> data;

  sensor_msgs_Image() {
  }

  bool readBare(yarp::os::ConnectionReader& connection) {
    // *** header ***
    if (!header.read(connection)) return false;

    // *** height ***
    height = connection.expectInt();

    // *** width ***
    width = connection.expectInt();

    // *** encoding ***
    int len = connection.expectInt();
    encoding.resize(len);
    if (!connection.expectBlock((char*)encoding.c_str(),len)) return false;

    // *** is_bigendian ***
    if (!connection.expectBlock((char*)&is_bigendian,1)) return false;

    // *** step ***
    step = connection.expectInt();

    // *** data ***
    len = connection.expectInt();
    data.resize(len);
    if (!connection.expectBlock((char*)&data[0],sizeof(unsigned char)*len)) return false;
    return !connection.isError();
  }

  bool readBottle(yarp::os::ConnectionReader& connection) {
    connection.convertTextMode();
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListHeader(7)) return false;

    // *** header ***
    if (!header.read(connection)) return false;

    // *** height ***
    height = reader.expectInt();

    // *** width ***
    width = reader.expectInt();

    // *** encoding ***
    if (!reader.readString(encoding)) return false;

    // *** is_bigendian ***
    is_bigendian = reader.expectInt();

    // *** step ***
    step = reader.expectInt();

    // *** data ***
    if (connection.expectInt()!=(BOTTLE_TAG_LIST|BOTTLE_TAG_INT)) return false;
    int len = connection.expectInt();
    data.resize(len);
    for (int i=0; i<len; i++) {
      data[i] = (unsigned char)connection.expectInt();
    }
    return !connection.isError();
  }

  using yarp::os::idl::WirePortable::read;
  bool read(yarp::os::ConnectionReader& connection) {
    if (connection.isBareMode()) return readBare(connection);
    return readBottle(connection);
  }

  bool writeBare(yarp::os::ConnectionWriter& connection) {
    // *** header ***
    if (!header.write(connection)) return false;

    // *** height ***
    connection.appendInt(height);

    // *** width ***
    connection.appendInt(width);

    // *** encoding ***
    connection.appendInt(encoding.length());
    connection.appendExternalBlock((char*)encoding.c_str(),encoding.length());

    // *** is_bigendian ***
    connection.appendBlock((char*)&is_bigendian,1);

    // *** step ***
    connection.appendInt(step);

    // *** data ***
    connection.appendInt(data.size());
    connection.appendExternalBlock((char*)&data[0],sizeof(unsigned char)*data.size());
    return !connection.isError();
  }

  bool writeBottle(yarp::os::ConnectionWriter& connection) {
    connection.appendInt(BOTTLE_TAG_LIST);
    connection.appendInt(7);

    // *** header ***
    if (!header.write(connection)) return false;

    // *** height ***
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt((int)height);

    // *** width ***
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt((int)width);

    // *** encoding ***
    connection.appendInt(BOTTLE_TAG_STRING);
    connection.appendInt(encoding.length());
    connection.appendExternalBlock((char*)encoding.c_str(),encoding.length());

    // *** is_bigendian ***
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt((int)is_bigendian);

    // *** step ***
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt((int)step);

    // *** data ***
    connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_INT);
    connection.appendInt(data.size());
    for (size_t i=0; i<data.size(); i++) {
      connection.appendInt((int)data[i]);
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
  typedef yarp::os::idl::BareStyle<sensor_msgs_Image> rosStyle;
  typedef yarp::os::idl::BottleStyle<sensor_msgs_Image> bottleStyle;

  // Give source text for class, ROS will need this
  yarp::os::ConstString getTypeText() {
    return "# This message contains an uncompressed image\n\
# (0, 0) is at top-left corner of image\n\
#\n\
\n\
Header header        # Header timestamp should be acquisition time of image\n\
                     # Header frame_id should be optical frame of camera\n\
                     # origin of frame should be optical center of cameara\n\
                     # +x should point to the right in the image\n\
                     # +y should point down in the image\n\
                     # +z should point into to plane of the image\n\
                     # If the frame_id here and the frame_id of the CameraInfo\n\
                     # message associated with the image conflict\n\
                     # the behavior is undefined\n\
\n\
uint32 height         # image height, that is, number of rows\n\
uint32 width          # image width, that is, number of columns\n\
\n\
# The legal values for encoding are in file src/image_encodings.cpp\n\
# If you want to standardize a new string format, join\n\
# ros-users@lists.sourceforge.net and send an email proposing a new encoding.\n\
\n\
string encoding       # Encoding of pixels -- channel meaning, ordering, size\n\
                      # taken from the list of strings in include/sensor_msgs/image_encodings.h\n\
\n\
uint8 is_bigendian    # is this data bigendian?\n\
uint32 step           # Full row length in bytes\n\
uint8[] data          # actual matrix data, size is (step * rows)\n\
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
    yarp::os::Type typ = yarp::os::Type::byName("sensor_msgs/Image","sensor_msgs/Image");
    typ.addProperty("md5sum",yarp::os::Value("060021388200f6f0f447d0fcd9c64743"));
    typ.addProperty("message_definition",yarp::os::Value(getTypeText()));
    return typ;
  }
};

#endif

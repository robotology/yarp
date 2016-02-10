// This is an automatically generated file.
// Generated from this sensor_msgs_LaserScan.msg definition:
//   # Single scan from a planar laser range-finder
//   #
//   # If you have another ranging device with different behavior (e.g. a sonar
//   # array), please find or create a different message, since applications
//   # will make fairly laser-specific assumptions about this data
//   
//   Header header            # timestamp in the header is the acquisition time of 
//                            # the first ray in the scan.
//                            #
//                            # in frame frame_id, angles are measured around 
//                            # the positive Z axis (counterclockwise, if Z is up)
//                            # with zero angle being forward along the x axis
//                            
//   float32 angle_min        # start angle of the scan [rad]
//   float32 angle_max        # end angle of the scan [rad]
//   float32 angle_increment  # angular distance between measurements [rad]
//   
//   float32 time_increment   # time between measurements [seconds] - if your scanner
//                            # is moving, this will be used in interpolating position
//                            # of 3d points
//   float32 scan_time        # time between scans [seconds]
//   
//   float32 range_min        # minimum range value [m]
//   float32 range_max        # maximum range value [m]
//   
//   float32[] ranges         # range data [m] (Note: values < range_min or > range_max should be discarded)
//   float32[] intensities    # intensity data [device-specific units].  If your
//                            # device does not provide intensities, please leave
//                            # the array empty.// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARPMSG_TYPE_sensor_msgs_LaserScan
#define YARPMSG_TYPE_sensor_msgs_LaserScan

#include <string>
#include <vector>
#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include "TickTime.h"
#include "std_msgs_Header.h"

class sensor_msgs_LaserScan : public yarp::os::idl::WirePortable {
public:
  std_msgs_Header header;
  yarp::os::NetFloat32 angle_min;
  yarp::os::NetFloat32 angle_max;
  yarp::os::NetFloat32 angle_increment;
  yarp::os::NetFloat32 time_increment;
  yarp::os::NetFloat32 scan_time;
  yarp::os::NetFloat32 range_min;
  yarp::os::NetFloat32 range_max;
  std::vector<yarp::os::NetFloat32> ranges;
  std::vector<yarp::os::NetFloat32> intensities;

  sensor_msgs_LaserScan() {
  }

  bool readBare(yarp::os::ConnectionReader& connection) {
    // *** header ***
    if (!header.read(connection)) return false;

    // *** angle_min ***
    if (!connection.expectBlock((char*)&angle_min,4)) return false;

    // *** angle_max ***
    if (!connection.expectBlock((char*)&angle_max,4)) return false;

    // *** angle_increment ***
    if (!connection.expectBlock((char*)&angle_increment,4)) return false;

    // *** time_increment ***
    if (!connection.expectBlock((char*)&time_increment,4)) return false;

    // *** scan_time ***
    if (!connection.expectBlock((char*)&scan_time,4)) return false;

    // *** range_min ***
    if (!connection.expectBlock((char*)&range_min,4)) return false;

    // *** range_max ***
    if (!connection.expectBlock((char*)&range_max,4)) return false;

    // *** ranges ***
    int len = connection.expectInt();
    ranges.resize(len);
    if (!connection.expectBlock((char*)&ranges[0],sizeof(yarp::os::NetFloat32)*len)) return false;

    // *** intensities ***
    len = connection.expectInt();
    intensities.resize(len);
    if (!connection.expectBlock((char*)&intensities[0],sizeof(yarp::os::NetFloat32)*len)) return false;
    return !connection.isError();
  }

  bool readBottle(yarp::os::ConnectionReader& connection) {
    connection.convertTextMode();
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListHeader(10)) return false;

    // *** header ***
    if (!header.read(connection)) return false;

    // *** angle_min ***
    angle_min = reader.expectDouble();

    // *** angle_max ***
    angle_max = reader.expectDouble();

    // *** angle_increment ***
    angle_increment = reader.expectDouble();

    // *** time_increment ***
    time_increment = reader.expectDouble();

    // *** scan_time ***
    scan_time = reader.expectDouble();

    // *** range_min ***
    range_min = reader.expectDouble();

    // *** range_max ***
    range_max = reader.expectDouble();

    // *** ranges ***
    if (connection.expectInt()!=(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE)) return false;
    int len = connection.expectInt();
    ranges.resize(len);
    for (int i=0; i<len; i++) {
      ranges[i] = (yarp::os::NetFloat32)connection.expectDouble();
    }

    // *** intensities ***
    if (connection.expectInt()!=(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE)) return false;
    len = connection.expectInt();
    intensities.resize(len);
    for (int i=0; i<len; i++) {
      intensities[i] = (yarp::os::NetFloat32)connection.expectDouble();
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

    // *** angle_min ***
    connection.appendBlock((char*)&angle_min,4);

    // *** angle_max ***
    connection.appendBlock((char*)&angle_max,4);

    // *** angle_increment ***
    connection.appendBlock((char*)&angle_increment,4);

    // *** time_increment ***
    connection.appendBlock((char*)&time_increment,4);

    // *** scan_time ***
    connection.appendBlock((char*)&scan_time,4);

    // *** range_min ***
    connection.appendBlock((char*)&range_min,4);

    // *** range_max ***
    connection.appendBlock((char*)&range_max,4);

    // *** ranges ***
    connection.appendInt(ranges.size());
    connection.appendExternalBlock((char*)&ranges[0],sizeof(yarp::os::NetFloat32)*ranges.size());

    // *** intensities ***
    connection.appendInt(intensities.size());
    connection.appendExternalBlock((char*)&intensities[0],sizeof(yarp::os::NetFloat32)*intensities.size());
    return !connection.isError();
  }

  bool writeBottle(yarp::os::ConnectionWriter& connection) {
    connection.appendInt(BOTTLE_TAG_LIST);
    connection.appendInt(10);

    // *** header ***
    if (!header.write(connection)) return false;

    // *** angle_min ***
    connection.appendInt(BOTTLE_TAG_DOUBLE);
    connection.appendDouble((double)angle_min);

    // *** angle_max ***
    connection.appendInt(BOTTLE_TAG_DOUBLE);
    connection.appendDouble((double)angle_max);

    // *** angle_increment ***
    connection.appendInt(BOTTLE_TAG_DOUBLE);
    connection.appendDouble((double)angle_increment);

    // *** time_increment ***
    connection.appendInt(BOTTLE_TAG_DOUBLE);
    connection.appendDouble((double)time_increment);

    // *** scan_time ***
    connection.appendInt(BOTTLE_TAG_DOUBLE);
    connection.appendDouble((double)scan_time);

    // *** range_min ***
    connection.appendInt(BOTTLE_TAG_DOUBLE);
    connection.appendDouble((double)range_min);

    // *** range_max ***
    connection.appendInt(BOTTLE_TAG_DOUBLE);
    connection.appendDouble((double)range_max);

    // *** ranges ***
    connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE);
    connection.appendInt(ranges.size());
    for (size_t i=0; i<ranges.size(); i++) {
      connection.appendDouble((double)ranges[i]);
    }

    // *** intensities ***
    connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE);
    connection.appendInt(intensities.size());
    for (size_t i=0; i<intensities.size(); i++) {
      connection.appendDouble((double)intensities[i]);
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
  typedef yarp::os::idl::BareStyle<sensor_msgs_LaserScan> rosStyle;
  typedef yarp::os::idl::BottleStyle<sensor_msgs_LaserScan> bottleStyle;

  // Give source text for class, ROS will need this
  yarp::os::ConstString getTypeText() {
    return "# Single scan from a planar laser range-finder\n\
#\n\
# If you have another ranging device with different behavior (e.g. a sonar\n\
# array), please find or create a different message, since applications\n\
# will make fairly laser-specific assumptions about this data\n\
\n\
Header header            # timestamp in the header is the acquisition time of \n\
                         # the first ray in the scan.\n\
                         #\n\
                         # in frame frame_id, angles are measured around \n\
                         # the positive Z axis (counterclockwise, if Z is up)\n\
                         # with zero angle being forward along the x axis\n\
                         \n\
float32 angle_min        # start angle of the scan [rad]\n\
float32 angle_max        # end angle of the scan [rad]\n\
float32 angle_increment  # angular distance between measurements [rad]\n\
\n\
float32 time_increment   # time between measurements [seconds] - if your scanner\n\
                         # is moving, this will be used in interpolating position\n\
                         # of 3d points\n\
float32 scan_time        # time between scans [seconds]\n\
\n\
float32 range_min        # minimum range value [m]\n\
float32 range_max        # maximum range value [m]\n\
\n\
float32[] ranges         # range data [m] (Note: values < range_min or > range_max should be discarded)\n\
float32[] intensities    # intensity data [device-specific units].  If your\n\
                         # device does not provide intensities, please leave\n\
                         # the array empty.\n================================================================================\n\
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
string frame_id";
  }

  // Name the class, ROS will need this
  yarp::os::Type getType() {
    yarp::os::Type typ = yarp::os::Type::byName("sensor_msgs/LaserScan","sensor_msgs/LaserScan");
    typ.addProperty("md5sum",yarp::os::Value("90c7ef2dc6895d81024acba2ac42f369"));
    typ.addProperty("message_definition",yarp::os::Value(getTypeText()));
    return typ;
  }
};

#endif

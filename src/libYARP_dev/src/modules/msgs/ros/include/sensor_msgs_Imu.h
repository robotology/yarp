// This is an automatically generated file.
// Generated from this sensor_msgs_Imu.msg definition:
//   # This is a message to hold data from an IMU (Inertial Measurement Unit)
//   #
//   # Accelerations should be in m/s^2 (not in g's), and rotational velocity should be in rad/sec
//   #
//   # If the covariance of the measurement is known, it should be filled in (if all you know is the 
//   # variance of each measurement, e.g. from the datasheet, just put those along the diagonal)
//   # A covariance matrix of all zeros will be interpreted as "covariance unknown", and to use the
//   # data a covariance will have to be assumed or gotten from some other source
//   #
//   # If you have no estimate for one of the data elements (e.g. your IMU doesn't produce an orientation 
//   # estimate), please set element 0 of the associated covariance matrix to -1
//   # If you are interpreting this message, please check for a value of -1 in the first element of each 
//   # covariance matrix, and disregard the associated estimate.
//   
//   Header header
//   
//   geometry_msgs/Quaternion orientation
//   float64[9] orientation_covariance # Row major about x, y, z axes
//   
//   geometry_msgs/Vector3 angular_velocity
//   float64[9] angular_velocity_covariance # Row major about x, y, z axes
//   
//   geometry_msgs/Vector3 linear_acceleration
//   float64[9] linear_acceleration_covariance # Row major x, y z 
//   
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARPMSG_TYPE_sensor_msgs_Imu
#define YARPMSG_TYPE_sensor_msgs_Imu

#include <string>
#include <vector>
#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include "TickTime.h"
#include "Header.h"
#include "geometry_msgs_Quaternion.h"
#include "geometry_msgs_Vector3.h"

class sensor_msgs_Imu : public yarp::os::idl::WirePortable {
public:
  Header header;
  geometry_msgs_Quaternion orientation;
  std::vector<yarp::os::NetFloat64> orientation_covariance;
  geometry_msgs_Vector3 angular_velocity;
  std::vector<yarp::os::NetFloat64> angular_velocity_covariance;
  geometry_msgs_Vector3 linear_acceleration;
  std::vector<yarp::os::NetFloat64> linear_acceleration_covariance;

  sensor_msgs_Imu() {
    orientation_covariance.resize(9,0.0);
    angular_velocity_covariance.resize(9,0.0);
    linear_acceleration_covariance.resize(9,0.0);
  }

  bool readBare(yarp::os::ConnectionReader& connection) {
    // *** header ***
    if (!header.read(connection)) return false;

    // *** orientation ***
    if (!orientation.read(connection)) return false;

    // *** orientation_covariance ***
    int len = 9;
    orientation_covariance.resize(len);
    if (!connection.expectBlock((char*)&orientation_covariance[0],sizeof(yarp::os::NetFloat64)*len)) return false;

    // *** angular_velocity ***
    if (!angular_velocity.read(connection)) return false;

    // *** angular_velocity_covariance ***
    len = 9;
    angular_velocity_covariance.resize(len);
    if (!connection.expectBlock((char*)&angular_velocity_covariance[0],sizeof(yarp::os::NetFloat64)*len)) return false;

    // *** linear_acceleration ***
    if (!linear_acceleration.read(connection)) return false;

    // *** linear_acceleration_covariance ***
    len = 9;
    linear_acceleration_covariance.resize(len);
    if (!connection.expectBlock((char*)&linear_acceleration_covariance[0],sizeof(yarp::os::NetFloat64)*len)) return false;
    return !connection.isError();
  }

  bool readBottle(yarp::os::ConnectionReader& connection) {
    connection.convertTextMode();
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListHeader(7)) return false;

    // *** header ***
    if (!header.read(connection)) return false;

    // *** orientation ***
    if (!orientation.read(connection)) return false;

    // *** orientation_covariance ***
    if (connection.expectInt()!=(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE)) return false;
    int len = connection.expectInt();
    orientation_covariance.resize(len);
    for (int i=0; i<len; i++) {
      orientation_covariance[i] = (yarp::os::NetFloat64)connection.expectDouble();
    }

    // *** angular_velocity ***
    if (!angular_velocity.read(connection)) return false;

    // *** angular_velocity_covariance ***
    if (connection.expectInt()!=(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE)) return false;
    len = connection.expectInt();
    angular_velocity_covariance.resize(len);
    for (int i=0; i<len; i++) {
      angular_velocity_covariance[i] = (yarp::os::NetFloat64)connection.expectDouble();
    }

    // *** linear_acceleration ***
    if (!linear_acceleration.read(connection)) return false;

    // *** linear_acceleration_covariance ***
    if (connection.expectInt()!=(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE)) return false;
    len = connection.expectInt();
    linear_acceleration_covariance.resize(len);
    for (int i=0; i<len; i++) {
      linear_acceleration_covariance[i] = (yarp::os::NetFloat64)connection.expectDouble();
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

    // *** orientation ***
    if (!orientation.write(connection)) return false;

    // *** orientation_covariance ***
    connection.appendExternalBlock((char*)&orientation_covariance[0],sizeof(yarp::os::NetFloat64)*orientation_covariance.size());

    // *** angular_velocity ***
    if (!angular_velocity.write(connection)) return false;

    // *** angular_velocity_covariance ***
    connection.appendExternalBlock((char*)&angular_velocity_covariance[0],sizeof(yarp::os::NetFloat64)*angular_velocity_covariance.size());

    // *** linear_acceleration ***
    if (!linear_acceleration.write(connection)) return false;

    // *** linear_acceleration_covariance ***
    connection.appendExternalBlock((char*)&linear_acceleration_covariance[0],sizeof(yarp::os::NetFloat64)*linear_acceleration_covariance.size());
    return !connection.isError();
  }

  bool writeBottle(yarp::os::ConnectionWriter& connection) {
    connection.appendInt(BOTTLE_TAG_LIST);
    connection.appendInt(7);

    // *** header ***
    if (!header.write(connection)) return false;

    // *** orientation ***
    if (!orientation.write(connection)) return false;

    // *** orientation_covariance ***
    connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE);
    connection.appendInt(orientation_covariance.size());
    for (size_t i=0; i<orientation_covariance.size(); i++) {
      connection.appendDouble((double)orientation_covariance[i]);
    }

    // *** angular_velocity ***
    if (!angular_velocity.write(connection)) return false;

    // *** angular_velocity_covariance ***
    connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE);
    connection.appendInt(angular_velocity_covariance.size());
    for (size_t i=0; i<angular_velocity_covariance.size(); i++) {
      connection.appendDouble((double)angular_velocity_covariance[i]);
    }

    // *** linear_acceleration ***
    if (!linear_acceleration.write(connection)) return false;

    // *** linear_acceleration_covariance ***
    connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE);
    connection.appendInt(linear_acceleration_covariance.size());
    for (size_t i=0; i<linear_acceleration_covariance.size(); i++) {
      connection.appendDouble((double)linear_acceleration_covariance[i]);
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
  typedef yarp::os::idl::BareStyle<sensor_msgs_Imu> rosStyle;
  typedef yarp::os::idl::BottleStyle<sensor_msgs_Imu> bottleStyle;

  // Give source text for class, ROS will need this
  yarp::os::ConstString getTypeText() {
    return "# This is a message to hold data from an IMU (Inertial Measurement Unit)\n\
#\n\
# Accelerations should be in m/s^2 (not in g's), and rotational velocity should be in rad/sec\n\
#\n\
# If the covariance of the measurement is known, it should be filled in (if all you know is the \n\
# variance of each measurement, e.g. from the datasheet, just put those along the diagonal)\n\
# A covariance matrix of all zeros will be interpreted as \"covariance unknown\", and to use the\n\
# data a covariance will have to be assumed or gotten from some other source\n\
#\n\
# If you have no estimate for one of the data elements (e.g. your IMU doesn't produce an orientation \n\
# estimate), please set element 0 of the associated covariance matrix to -1\n\
# If you are interpreting this message, please check for a value of -1 in the first element of each \n\
# covariance matrix, and disregard the associated estimate.\n\
\n\
Header header\n\
\n\
geometry_msgs/Quaternion orientation\n\
float64[9] orientation_covariance # Row major about x, y, z axes\n\
\n\
geometry_msgs/Vector3 angular_velocity\n\
float64[9] angular_velocity_covariance # Row major about x, y, z axes\n\
\n\
geometry_msgs/Vector3 linear_acceleration\n\
float64[9] linear_acceleration_covariance # Row major x, y z \n\
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
\n================================================================================\n\
MSG: geometry_msgs/Quaternion\n\
# This represents an orientation in free space in quaternion form.\n\
\n\
float64 x\n\
float64 y\n\
float64 z\n\
float64 w\n\
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
    yarp::os::Type typ = yarp::os::Type::byName("sensor_msgs/Imu","sensor_msgs/Imu");
    typ.addProperty("md5sum",yarp::os::Value("6a62c6daae103f4ff57a132d6f95cec2"));
    typ.addProperty("message_definition",yarp::os::Value(getTypeText()));
    return typ;
  }
};

#endif

// This is an automatically generated file.
// Generated from this sensor_msgs_CameraInfo.msg definition:
//   std_msgs/Header header
//   uint32 height
//   uint32 width
//   string distortion_model
//   float64[] D
//   float64[9] K
//   float64[9] R
//   float64[12] P
//   uint32 binning_x
//   uint32 binning_y
//   sensor_msgs/RegionOfInterest roi
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARPMSG_TYPE_sensor_msgs_CameraInfo
#define YARPMSG_TYPE_sensor_msgs_CameraInfo

#include <string>
#include <vector>
#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include "TickTime.h"
#include "std_msgs_Header.h"
#include "sensor_msgs_RegionOfInterest.h"

class sensor_msgs_CameraInfo : public yarp::os::idl::WirePortable {
public:
  std_msgs_Header header;
  yarp::os::NetUint32 height;
  yarp::os::NetUint32 width;
  std::string distortion_model;
  std::vector<yarp::os::NetFloat64> D;
  std::vector<yarp::os::NetFloat64> K;
  std::vector<yarp::os::NetFloat64> R;
  std::vector<yarp::os::NetFloat64> P;
  yarp::os::NetUint32 binning_x;
  yarp::os::NetUint32 binning_y;
  sensor_msgs_RegionOfInterest roi;

  sensor_msgs_CameraInfo() {
    K.resize(9,0.0);
    R.resize(9,0.0);
    P.resize(12,0.0);
  }

  bool readBare(yarp::os::ConnectionReader& connection) {
    // *** header ***
    if (!header.read(connection)) return false;

    // *** height ***
    height = connection.expectInt();

    // *** width ***
    width = connection.expectInt();

    // *** distortion_model ***
    int len = connection.expectInt();
    distortion_model.resize(len);
    if (!connection.expectBlock((char*)distortion_model.c_str(),len)) return false;

    // *** D ***
    len = connection.expectInt();
    D.resize(len);
    if (!connection.expectBlock((char*)&D[0],sizeof(yarp::os::NetFloat64)*len)) return false;

    // *** K ***
    len = 9;
    K.resize(len);
    if (!connection.expectBlock((char*)&K[0],sizeof(yarp::os::NetFloat64)*len)) return false;

    // *** R ***
    len = 9;
    R.resize(len);
    if (!connection.expectBlock((char*)&R[0],sizeof(yarp::os::NetFloat64)*len)) return false;

    // *** P ***
    len = 12;
    P.resize(len);
    if (!connection.expectBlock((char*)&P[0],sizeof(yarp::os::NetFloat64)*len)) return false;

    // *** binning_x ***
    binning_x = connection.expectInt();

    // *** binning_y ***
    binning_y = connection.expectInt();

    // *** roi ***
    if (!roi.read(connection)) return false;
    return !connection.isError();
  }

  bool readBottle(yarp::os::ConnectionReader& connection) {
    connection.convertTextMode();
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListHeader(11)) return false;

    // *** header ***
    if (!header.read(connection)) return false;

    // *** height ***
    height = reader.expectInt();

    // *** width ***
    width = reader.expectInt();

    // *** distortion_model ***
    if (!reader.readString(distortion_model)) return false;

    // *** D ***
    if (connection.expectInt()!=(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE)) return false;
    int len = connection.expectInt();
    D.resize(len);
    for (int i=0; i<len; i++) {
      D[i] = (yarp::os::NetFloat64)connection.expectDouble();
    }

    // *** K ***
    if (connection.expectInt()!=(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE)) return false;
    len = connection.expectInt();
    K.resize(len);
    for (int i=0; i<len; i++) {
      K[i] = (yarp::os::NetFloat64)connection.expectDouble();
    }

    // *** R ***
    if (connection.expectInt()!=(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE)) return false;
    len = connection.expectInt();
    R.resize(len);
    for (int i=0; i<len; i++) {
      R[i] = (yarp::os::NetFloat64)connection.expectDouble();
    }

    // *** P ***
    if (connection.expectInt()!=(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE)) return false;
    len = connection.expectInt();
    P.resize(len);
    for (int i=0; i<len; i++) {
      P[i] = (yarp::os::NetFloat64)connection.expectDouble();
    }

    // *** binning_x ***
    binning_x = reader.expectInt();

    // *** binning_y ***
    binning_y = reader.expectInt();

    // *** roi ***
    if (!roi.read(connection)) return false;
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

    // *** distortion_model ***
    connection.appendInt(distortion_model.length());
    connection.appendExternalBlock((char*)distortion_model.c_str(),distortion_model.length());

    // *** D ***
    connection.appendInt(D.size());
    connection.appendExternalBlock((char*)&D[0],sizeof(yarp::os::NetFloat64)*D.size());

    // *** K ***
    connection.appendExternalBlock((char*)&K[0],sizeof(yarp::os::NetFloat64)*K.size());

    // *** R ***
    connection.appendExternalBlock((char*)&R[0],sizeof(yarp::os::NetFloat64)*R.size());

    // *** P ***
    connection.appendExternalBlock((char*)&P[0],sizeof(yarp::os::NetFloat64)*P.size());

    // *** binning_x ***
    connection.appendInt(binning_x);

    // *** binning_y ***
    connection.appendInt(binning_y);

    // *** roi ***
    if (!roi.write(connection)) return false;
    return !connection.isError();
  }

  bool writeBottle(yarp::os::ConnectionWriter& connection) {
    connection.appendInt(BOTTLE_TAG_LIST);
    connection.appendInt(11);

    // *** header ***
    if (!header.write(connection)) return false;

    // *** height ***
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt((int)height);

    // *** width ***
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt((int)width);

    // *** distortion_model ***
    connection.appendInt(BOTTLE_TAG_STRING);
    connection.appendInt(distortion_model.length());
    connection.appendExternalBlock((char*)distortion_model.c_str(),distortion_model.length());

    // *** D ***
    connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE);
    connection.appendInt(D.size());
    for (size_t i=0; i<D.size(); i++) {
      connection.appendDouble((double)D[i]);
    }

    // *** K ***
    connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE);
    connection.appendInt(K.size());
    for (size_t i=0; i<K.size(); i++) {
      connection.appendDouble((double)K[i]);
    }

    // *** R ***
    connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE);
    connection.appendInt(R.size());
    for (size_t i=0; i<R.size(); i++) {
      connection.appendDouble((double)R[i]);
    }

    // *** P ***
    connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE);
    connection.appendInt(P.size());
    for (size_t i=0; i<P.size(); i++) {
      connection.appendDouble((double)P[i]);
    }

    // *** binning_x ***
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt((int)binning_x);

    // *** binning_y ***
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt((int)binning_y);

    // *** roi ***
    if (!roi.write(connection)) return false;
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
  typedef yarp::os::idl::BareStyle<sensor_msgs_CameraInfo> rosStyle;
  typedef yarp::os::idl::BottleStyle<sensor_msgs_CameraInfo> bottleStyle;

  // Give source text for class, ROS will need this
  yarp::os::ConstString getTypeText() {
    return "std_msgs/Header header\n\
uint32 height\n\
uint32 width\n\
string distortion_model\n\
float64[] D\n\
float64[9] K\n\
float64[9] R\n\
float64[12] P\n\
uint32 binning_x\n\
uint32 binning_y\n\
sensor_msgs/RegionOfInterest roi\n================================================================================\n\
MSG: std_msgs/Header\n\
uint32 seq\n\
time stamp\n\
string frame_id\n================================================================================\n\
MSG: sensor_msgs/RegionOfInterest\n\
uint32 x_offset\n\
uint32 y_offset\n\
uint32 height\n\
uint32 width\n\
bool do_rectify";
  }

  // Name the class, ROS will need this
  yarp::os::Type getType() {
    yarp::os::Type typ = yarp::os::Type::byName("sensor_msgs/CameraInfo","sensor_msgs/CameraInfo");
    typ.addProperty("md5sum",yarp::os::Value("c9a58c1b0b154e0e6da7578cb991d214"));
    typ.addProperty("message_definition",yarp::os::Value(getTypeText()));
    return typ;
  }
};

#endif

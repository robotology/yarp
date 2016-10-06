// This is an automatically generated file.
// Generated from this visualization_msgs_Marker.msg definition:
//   uint8 ARROW=0
//   uint8 CUBE=1
//   uint8 SPHERE=2
//   uint8 CYLINDER=3
//   uint8 LINE_STRIP=4
//   uint8 LINE_LIST=5
//   uint8 CUBE_LIST=6
//   uint8 SPHERE_LIST=7
//   uint8 POINTS=8
//   uint8 TEXT_VIEW_FACING=9
//   uint8 MESH_RESOURCE=10
//   uint8 TRIANGLE_LIST=11
//   uint8 ADD=0
//   uint8 MODIFY=0
//   uint8 DELETE=2
//   uint8 DELETEALL=3
//   std_msgs/Header header
//   string ns
//   int32 id
//   int32 type
//   int32 action
//   geometry_msgs/Pose pose
//   geometry_msgs/Vector3 scale
//   std_msgs/ColorRGBA color
//   duration lifetime
//   bool frame_locked
//   geometry_msgs/Point[] points
//   std_msgs/ColorRGBA[] colors
//   string text
//   string mesh_resource
//   bool mesh_use_embedded_materials
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARPMSG_TYPE_visualization_msgs_Marker
#define YARPMSG_TYPE_visualization_msgs_Marker

#include <string>
#include <vector>
#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include "TickTime.h"
#include "std_msgs_Header.h"
#include "geometry_msgs_Point.h"
#include "geometry_msgs_Quaternion.h"
#include "geometry_msgs_Pose.h"
#include "geometry_msgs_Vector3.h"
#include "std_msgs_ColorRGBA.h"
#include "TickDuration.h"

class visualization_msgs_Marker : public yarp::os::idl::WirePortable {
public:
  static const unsigned char ARROW = 0;
  static const unsigned char CUBE = 1;
  static const unsigned char SPHERE = 2;
  static const unsigned char CYLINDER = 3;
  static const unsigned char LINE_STRIP = 4;
  static const unsigned char LINE_LIST = 5;
  static const unsigned char CUBE_LIST = 6;
  static const unsigned char SPHERE_LIST = 7;
  static const unsigned char POINTS = 8;
  static const unsigned char TEXT_VIEW_FACING = 9;
  static const unsigned char MESH_RESOURCE = 10;
  static const unsigned char TRIANGLE_LIST = 11;
  static const unsigned char ADD = 0;
  static const unsigned char MODIFY = 0;
  static const unsigned char DELETE = 2;
  static const unsigned char DELETEALL = 3;
  std_msgs_Header header;
  std::string ns;
  yarp::os::NetInt32 id;
  yarp::os::NetInt32 type;
  yarp::os::NetInt32 action;
  geometry_msgs_Pose pose;
  geometry_msgs_Vector3 scale;
  std_msgs_ColorRGBA color;
  TickDuration lifetime;
  bool frame_locked;
  std::vector<geometry_msgs_Point> points;
  std::vector<std_msgs_ColorRGBA> colors;
  std::string text;
  std::string mesh_resource;
  bool mesh_use_embedded_materials;

  visualization_msgs_Marker() {
  }

  bool readBare(yarp::os::ConnectionReader& connection) {
    // *** header ***
    if (!header.read(connection)) return false;

    // *** ns ***
    int len = connection.expectInt();
    ns.resize(len);
    if (!connection.expectBlock((char*)ns.c_str(),len)) return false;

    // *** id ***
    id = connection.expectInt();

    // *** type ***
    type = connection.expectInt();

    // *** action ***
    action = connection.expectInt();

    // *** pose ***
    if (!pose.read(connection)) return false;

    // *** scale ***
    if (!scale.read(connection)) return false;

    // *** color ***
    if (!color.read(connection)) return false;

    // *** lifetime ***
    if (!lifetime.read(connection)) return false;

    // *** frame_locked ***
    if (!connection.expectBlock((char*)&frame_locked,1)) return false;

    // *** points ***
    len = connection.expectInt();
    points.resize(len);
    for (int i=0; i<len; i++) {
      if (!points[i].read(connection)) return false;
    }

    // *** colors ***
    len = connection.expectInt();
    colors.resize(len);
    for (int i=0; i<len; i++) {
      if (!colors[i].read(connection)) return false;
    }

    // *** text ***
    len = connection.expectInt();
    text.resize(len);
    if (!connection.expectBlock((char*)text.c_str(),len)) return false;

    // *** mesh_resource ***
    len = connection.expectInt();
    mesh_resource.resize(len);
    if (!connection.expectBlock((char*)mesh_resource.c_str(),len)) return false;

    // *** mesh_use_embedded_materials ***
    if (!connection.expectBlock((char*)&mesh_use_embedded_materials,1)) return false;
    return !connection.isError();
  }

  bool readBottle(yarp::os::ConnectionReader& connection) {
    connection.convertTextMode();
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListHeader(31)) return false;

    // *** header ***
    if (!header.read(connection)) return false;

    // *** ns ***
    if (!reader.readString(ns)) return false;

    // *** id ***
    id = reader.expectInt();

    // *** type ***
    type = reader.expectInt();

    // *** action ***
    action = reader.expectInt();

    // *** pose ***
    if (!pose.read(connection)) return false;

    // *** scale ***
    if (!scale.read(connection)) return false;

    // *** color ***
    if (!color.read(connection)) return false;

    // *** lifetime ***
    if (!lifetime.read(connection)) return false;

    // *** frame_locked ***
    frame_locked = reader.expectInt();

    // *** points ***
    if (connection.expectInt()!=BOTTLE_TAG_LIST) return false;
    int len = connection.expectInt();
    points.resize(len);
    for (int i=0; i<len; i++) {
      if (!points[i].read(connection)) return false;
    }

    // *** colors ***
    if (connection.expectInt()!=BOTTLE_TAG_LIST) return false;
    len = connection.expectInt();
    colors.resize(len);
    for (int i=0; i<len; i++) {
      if (!colors[i].read(connection)) return false;
    }

    // *** text ***
    if (!reader.readString(text)) return false;

    // *** mesh_resource ***
    if (!reader.readString(mesh_resource)) return false;

    // *** mesh_use_embedded_materials ***
    mesh_use_embedded_materials = reader.expectInt();
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

    // *** ns ***
    connection.appendInt(ns.length());
    connection.appendExternalBlock((char*)ns.c_str(),ns.length());

    // *** id ***
    connection.appendInt(id);

    // *** type ***
    connection.appendInt(type);

    // *** action ***
    connection.appendInt(action);

    // *** pose ***
    if (!pose.write(connection)) return false;

    // *** scale ***
    if (!scale.write(connection)) return false;

    // *** color ***
    if (!color.write(connection)) return false;

    // *** lifetime ***
    if (!lifetime.write(connection)) return false;

    // *** frame_locked ***
    connection.appendBlock((char*)&frame_locked,1);

    // *** points ***
    connection.appendInt(points.size());
    for (size_t i=0; i<points.size(); i++) {
      if (!points[i].write(connection)) return false;
    }

    // *** colors ***
    connection.appendInt(colors.size());
    for (size_t i=0; i<colors.size(); i++) {
      if (!colors[i].write(connection)) return false;
    }

    // *** text ***
    connection.appendInt(text.length());
    connection.appendExternalBlock((char*)text.c_str(),text.length());

    // *** mesh_resource ***
    connection.appendInt(mesh_resource.length());
    connection.appendExternalBlock((char*)mesh_resource.c_str(),mesh_resource.length());

    // *** mesh_use_embedded_materials ***
    connection.appendBlock((char*)&mesh_use_embedded_materials,1);
    return !connection.isError();
  }

  bool writeBottle(yarp::os::ConnectionWriter& connection) {
    connection.appendInt(BOTTLE_TAG_LIST);
    connection.appendInt(31);

    // *** header ***
    if (!header.write(connection)) return false;

    // *** ns ***
    connection.appendInt(BOTTLE_TAG_STRING);
    connection.appendInt(ns.length());
    connection.appendExternalBlock((char*)ns.c_str(),ns.length());

    // *** id ***
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt((int)id);

    // *** type ***
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt((int)type);

    // *** action ***
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt((int)action);

    // *** pose ***
    if (!pose.write(connection)) return false;

    // *** scale ***
    if (!scale.write(connection)) return false;

    // *** color ***
    if (!color.write(connection)) return false;

    // *** lifetime ***
    if (!lifetime.write(connection)) return false;

    // *** frame_locked ***
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt((int)frame_locked);

    // *** points ***
    connection.appendInt(BOTTLE_TAG_LIST);
    connection.appendInt(points.size());
    for (size_t i=0; i<points.size(); i++) {
      if (!points[i].write(connection)) return false;
    }

    // *** colors ***
    connection.appendInt(BOTTLE_TAG_LIST);
    connection.appendInt(colors.size());
    for (size_t i=0; i<colors.size(); i++) {
      if (!colors[i].write(connection)) return false;
    }

    // *** text ***
    connection.appendInt(BOTTLE_TAG_STRING);
    connection.appendInt(text.length());
    connection.appendExternalBlock((char*)text.c_str(),text.length());

    // *** mesh_resource ***
    connection.appendInt(BOTTLE_TAG_STRING);
    connection.appendInt(mesh_resource.length());
    connection.appendExternalBlock((char*)mesh_resource.c_str(),mesh_resource.length());

    // *** mesh_use_embedded_materials ***
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt((int)mesh_use_embedded_materials);
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
  typedef yarp::os::idl::BareStyle<visualization_msgs_Marker> rosStyle;
  typedef yarp::os::idl::BottleStyle<visualization_msgs_Marker> bottleStyle;

  // Give source text for class, ROS will need this
  yarp::os::ConstString getTypeText() {
    return "uint8 ARROW=0\n\
uint8 CUBE=1\n\
uint8 SPHERE=2\n\
uint8 CYLINDER=3\n\
uint8 LINE_STRIP=4\n\
uint8 LINE_LIST=5\n\
uint8 CUBE_LIST=6\n\
uint8 SPHERE_LIST=7\n\
uint8 POINTS=8\n\
uint8 TEXT_VIEW_FACING=9\n\
uint8 MESH_RESOURCE=10\n\
uint8 TRIANGLE_LIST=11\n\
uint8 ADD=0\n\
uint8 MODIFY=0\n\
uint8 DELETE=2\n\
uint8 DELETEALL=3\n\
std_msgs/Header header\n\
string ns\n\
int32 id\n\
int32 type\n\
int32 action\n\
geometry_msgs/Pose pose\n\
geometry_msgs/Vector3 scale\n\
std_msgs/ColorRGBA color\n\
duration lifetime\n\
bool frame_locked\n\
geometry_msgs/Point[] points\n\
std_msgs/ColorRGBA[] colors\n\
string text\n\
string mesh_resource\n\
bool mesh_use_embedded_materials\n================================================================================\n\
MSG: std_msgs/Header\n\
uint32 seq\n\
time stamp\n\
string frame_id\n================================================================================\n\
MSG: geometry_msgs/Pose\n\
geometry_msgs/Point position\n\
geometry_msgs/Quaternion orientation\n================================================================================\n\
MSG: geometry_msgs/Point\n\
float64 x\n\
float64 y\n\
float64 z\n================================================================================\n\
MSG: geometry_msgs/Quaternion\n\
float64 x\n\
float64 y\n\
float64 z\n\
float64 w\n================================================================================\n\
MSG: geometry_msgs/Vector3\n\
float64 x\n\
float64 y\n\
float64 z\n================================================================================\n\
MSG: std_msgs/ColorRGBA\n\
float32 r\n\
float32 g\n\
float32 b\n\
float32 a";
  }

  // Name the class, ROS will need this
  yarp::os::Type getType() {
    yarp::os::Type typ = yarp::os::Type::byName("visualization_msgs/Marker","visualization_msgs/Marker");
    typ.addProperty("md5sum",yarp::os::Value("4048c9de2a16f4ae8e0538085ebf1b97"));
    typ.addProperty("message_definition",yarp::os::Value(getTypeText()));
    return typ;
  }
};

#endif

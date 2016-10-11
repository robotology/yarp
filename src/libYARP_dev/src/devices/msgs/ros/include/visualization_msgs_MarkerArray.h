// This is an automatically generated file.
// Generated from this visualization_msgs_MarkerArray.msg definition:
//   visualization_msgs/Marker[] markers
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARPMSG_TYPE_visualization_msgs_MarkerArray
#define YARPMSG_TYPE_visualization_msgs_MarkerArray

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
#include "visualization_msgs_Marker.h"

class visualization_msgs_MarkerArray : public yarp::os::idl::WirePortable {
public:
  std::vector<visualization_msgs_Marker> markers;

  visualization_msgs_MarkerArray() {
  }

  bool readBare(yarp::os::ConnectionReader& connection) {
    // *** markers ***
    int len = connection.expectInt();
    markers.resize(len);
    for (int i=0; i<len; i++) {
      if (!markers[i].read(connection)) return false;
    }
    return !connection.isError();
  }

  bool readBottle(yarp::os::ConnectionReader& connection) {
    connection.convertTextMode();
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListHeader(1)) return false;

    // *** markers ***
    if (connection.expectInt()!=BOTTLE_TAG_LIST) return false;
    int len = connection.expectInt();
    markers.resize(len);
    for (int i=0; i<len; i++) {
      if (!markers[i].read(connection)) return false;
    }
    return !connection.isError();
  }

  using yarp::os::idl::WirePortable::read;
  bool read(yarp::os::ConnectionReader& connection) {
    if (connection.isBareMode()) return readBare(connection);
    return readBottle(connection);
  }

  bool writeBare(yarp::os::ConnectionWriter& connection) {
    // *** markers ***
    connection.appendInt(markers.size());
    for (size_t i=0; i<markers.size(); i++) {
      if (!markers[i].write(connection)) return false;
    }
    return !connection.isError();
  }

  bool writeBottle(yarp::os::ConnectionWriter& connection) {
    connection.appendInt(BOTTLE_TAG_LIST);
    connection.appendInt(1);

    // *** markers ***
    connection.appendInt(BOTTLE_TAG_LIST);
    connection.appendInt(markers.size());
    for (size_t i=0; i<markers.size(); i++) {
      if (!markers[i].write(connection)) return false;
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
  typedef yarp::os::idl::BareStyle<visualization_msgs_MarkerArray> rosStyle;
  typedef yarp::os::idl::BottleStyle<visualization_msgs_MarkerArray> bottleStyle;

  // Give source text for class, ROS will need this
  yarp::os::ConstString getTypeText() {
    return "visualization_msgs/Marker[] markers\n================================================================================\n\
MSG: visualization_msgs/Marker\n\
uint8 ARROW=0\n\
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
    yarp::os::Type typ = yarp::os::Type::byName("visualization_msgs/MarkerArray","visualization_msgs/MarkerArray");
    typ.addProperty("md5sum",yarp::os::Value("d155b9ce5188fbaf89745847fd5882d7"));
    typ.addProperty("message_definition",yarp::os::Value(getTypeText()));
    return typ;
  }
};

#endif

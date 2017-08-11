// This is an automatically generated file.
// Generated from this nav_msgs_MapMetaData.msg definition:
//   # This hold basic information about the characterists of the OccupancyGrid
//   
//   # The time at which the map was loaded
//   time map_load_time
//   # The map resolution [m/cell]
//   float32 resolution
//   # Map width [cells]
//   uint32 width
//   # Map height [cells]
//   uint32 height
//   # The origin of the map [m, m, rad].  This is the real-world pose of the
//   # cell (0,0) in the map.
//   geometry_msgs/Pose origin
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARPMSG_TYPE_nav_msgs_MapMetaData
#define YARPMSG_TYPE_nav_msgs_MapMetaData

#include <string>
#include <vector>
#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include "TickTime.h"
#include "geometry_msgs_Point.h"
#include "geometry_msgs_Quaternion.h"
#include "geometry_msgs_Pose.h"

class nav_msgs_MapMetaData : public yarp::os::idl::WirePortable {
public:
  TickTime map_load_time;
  yarp::os::NetFloat32 resolution;
  yarp::os::NetUint32 width;
  yarp::os::NetUint32 height;
  geometry_msgs_Pose origin;

  nav_msgs_MapMetaData() {
  }

  void clear() {
    // *** map_load_time ***
    map_load_time.clear();

    // *** resolution ***
    resolution = 0.0;

    // *** width ***
    width = 0;

    // *** height ***
    height = 0;

    // *** origin ***
    origin.clear();
  }

  bool readBare(yarp::os::ConnectionReader& connection) override {
    // *** map_load_time ***
    if (!map_load_time.read(connection)) return false;

    // *** resolution ***
    if (!connection.expectBlock((char*)&resolution,4)) return false;

    // *** width ***
    width = connection.expectInt();

    // *** height ***
    height = connection.expectInt();

    // *** origin ***
    if (!origin.read(connection)) return false;
    return !connection.isError();
  }

  bool readBottle(yarp::os::ConnectionReader& connection) override {
    connection.convertTextMode();
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListHeader(5)) return false;

    // *** map_load_time ***
    if (!map_load_time.read(connection)) return false;

    // *** resolution ***
    resolution = reader.expectDouble();

    // *** width ***
    width = reader.expectInt();

    // *** height ***
    height = reader.expectInt();

    // *** origin ***
    if (!origin.read(connection)) return false;
    return !connection.isError();
  }

  using yarp::os::idl::WirePortable::read;
  bool read(yarp::os::ConnectionReader& connection) override {
    if (connection.isBareMode()) return readBare(connection);
    return readBottle(connection);
  }

  bool writeBare(yarp::os::ConnectionWriter& connection) override {
    // *** map_load_time ***
    if (!map_load_time.write(connection)) return false;

    // *** resolution ***
    connection.appendBlock((char*)&resolution,4);

    // *** width ***
    connection.appendInt(width);

    // *** height ***
    connection.appendInt(height);

    // *** origin ***
    if (!origin.write(connection)) return false;
    return !connection.isError();
  }

  bool writeBottle(yarp::os::ConnectionWriter& connection) override {
    connection.appendInt(BOTTLE_TAG_LIST);
    connection.appendInt(5);

    // *** map_load_time ***
    if (!map_load_time.write(connection)) return false;

    // *** resolution ***
    connection.appendInt(BOTTLE_TAG_DOUBLE);
    connection.appendDouble((double)resolution);

    // *** width ***
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt((int)width);

    // *** height ***
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt((int)height);

    // *** origin ***
    if (!origin.write(connection)) return false;
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
  typedef yarp::os::idl::BareStyle<nav_msgs_MapMetaData> rosStyle;
  typedef yarp::os::idl::BottleStyle<nav_msgs_MapMetaData> bottleStyle;

  // Give source text for class, ROS will need this
  yarp::os::ConstString getTypeText() {
    return "# This hold basic information about the characterists of the OccupancyGrid\n\
\n\
# The time at which the map was loaded\n\
time map_load_time\n\
# The map resolution [m/cell]\n\
float32 resolution\n\
# Map width [cells]\n\
uint32 width\n\
# Map height [cells]\n\
uint32 height\n\
# The origin of the map [m, m, rad].  This is the real-world pose of the\n\
# cell (0,0) in the map.\n\
geometry_msgs/Pose origin\n================================================================================\n\
MSG: geometry_msgs/Pose\n\
# A representation of pose in free space, composed of postion and orientation. \n\
geometry_msgs/Point position\n\
geometry_msgs/Quaternion orientation\n================================================================================\n\
MSG: geometry_msgs/Point\n\
# This contains the position of a point in free space\n\
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
    yarp::os::Type typ = yarp::os::Type::byName("nav_msgs/MapMetaData","nav_msgs/MapMetaData");
    typ.addProperty("md5sum",yarp::os::Value("10cfc8a2818024d3248802c00c95f11b"));
    typ.addProperty("message_definition",yarp::os::Value(getTypeText()));
    return typ;
  }
};

#endif

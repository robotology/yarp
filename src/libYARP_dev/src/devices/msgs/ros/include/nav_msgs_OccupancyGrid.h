// This is an automatically generated file.
// Generated from this nav_msgs_OccupancyGrid.msg definition:
//   # This represents a 2-D grid map, in which each cell represents the probability of
//   # occupancy.
//   
//   Header header 
//   
//   #MetaData for the map
//   MapMetaData info
//   
//   # The map data, in row-major order, starting with (0,0).  Occupancy
//   # probabilities are in the range [0,100].  Unknown is -1.
//   int8[] data
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARPMSG_TYPE_nav_msgs_OccupancyGrid
#define YARPMSG_TYPE_nav_msgs_OccupancyGrid

#include <string>
#include <vector>
#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include "TickTime.h"
#include "std_msgs_Header.h"
#include "geometry_msgs_Point.h"
#include "geometry_msgs_Quaternion.h"
#include "geometry_msgs_Pose.h"
#include "nav_msgs_MapMetaData.h"

class nav_msgs_OccupancyGrid : public yarp::os::idl::WirePortable {
public:
  std_msgs_Header header;
  nav_msgs_MapMetaData info;
  std::vector<char> data;

  nav_msgs_OccupancyGrid() {
  }

  void clear() {
    // *** header ***
    header.clear();

    // *** info ***
    info.clear();

    // *** data ***
    data.clear();
  }

  bool readBare(yarp::os::ConnectionReader& connection) YARP_OVERRIDE {
    // *** header ***
    if (!header.read(connection)) return false;

    // *** info ***
    if (!info.read(connection)) return false;

    // *** data ***
    int len = connection.expectInt();
    data.resize(len);
    if (len > 0 && !connection.expectBlock((char*)&data[0],sizeof(char)*len)) return false;
    return !connection.isError();
  }

  bool readBottle(yarp::os::ConnectionReader& connection) YARP_OVERRIDE {
    connection.convertTextMode();
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListHeader(3)) return false;

    // *** header ***
    if (!header.read(connection)) return false;

    // *** info ***
    if (!info.read(connection)) return false;

    // *** data ***
    if (connection.expectInt()!=(BOTTLE_TAG_LIST|BOTTLE_TAG_INT)) return false;
    int len = connection.expectInt();
    data.resize(len);
    for (int i=0; i<len; i++) {
      data[i] = (char)connection.expectInt();
    }
    return !connection.isError();
  }

  using yarp::os::idl::WirePortable::read;
  bool read(yarp::os::ConnectionReader& connection) YARP_OVERRIDE {
    if (connection.isBareMode()) return readBare(connection);
    return readBottle(connection);
  }

  bool writeBare(yarp::os::ConnectionWriter& connection) YARP_OVERRIDE {
    // *** header ***
    if (!header.write(connection)) return false;

    // *** info ***
    if (!info.write(connection)) return false;

    // *** data ***
    connection.appendInt(data.size());
    if (data.size()>0) {connection.appendExternalBlock((char*)&data[0],sizeof(char)*data.size());}
    return !connection.isError();
  }

  bool writeBottle(yarp::os::ConnectionWriter& connection) YARP_OVERRIDE {
    connection.appendInt(BOTTLE_TAG_LIST);
    connection.appendInt(3);

    // *** header ***
    if (!header.write(connection)) return false;

    // *** info ***
    if (!info.write(connection)) return false;

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
  bool write(yarp::os::ConnectionWriter& connection) YARP_OVERRIDE {
    if (connection.isBareMode()) return writeBare(connection);
    return writeBottle(connection);
  }

  // This class will serialize ROS style or YARP style depending on protocol.
  // If you need to force a serialization style, use one of these classes:
  typedef yarp::os::idl::BareStyle<nav_msgs_OccupancyGrid> rosStyle;
  typedef yarp::os::idl::BottleStyle<nav_msgs_OccupancyGrid> bottleStyle;

  // Give source text for class, ROS will need this
  yarp::os::ConstString getTypeText() {
    return "# This represents a 2-D grid map, in which each cell represents the probability of\n\
# occupancy.\n\
\n\
Header header \n\
\n\
#MetaData for the map\n\
MapMetaData info\n\
\n\
# The map data, in row-major order, starting with (0,0).  Occupancy\n\
# probabilities are in the range [0,100].  Unknown is -1.\n\
int8[] data\n================================================================================\n\
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
MSG: nav_msgs/MapMetaData\n\
# This hold basic information about the characterists of the OccupancyGrid\n\
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
  yarp::os::Type getType() YARP_OVERRIDE {
    yarp::os::Type typ = yarp::os::Type::byName("nav_msgs/OccupancyGrid","nav_msgs/OccupancyGrid");
    typ.addProperty("md5sum",yarp::os::Value("3381f2d731d4076ec5c71b0759edbe4e"));
    typ.addProperty("message_definition",yarp::os::Value(getTypeText()));
    return typ;
  }
};

#endif

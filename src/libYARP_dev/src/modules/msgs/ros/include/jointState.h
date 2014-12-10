// This is an automatically generated file.
// Generated from this jointState.msg definition:
//   Header header
//   string[]  name
//   float64[] position
//   float64[] velocity
//   float64[] effort
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARPMSG_TYPE_jointState
#define YARPMSG_TYPE_jointState

#include <string>
#include <vector>
#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <TickTime.h>
#include <Header.h>

class jointState : public yarp::os::idl::WirePortable {
public:
  Header header;
  std::vector<std::string> name;
  std::vector<yarp::os::NetFloat64> position;
  std::vector<yarp::os::NetFloat64> velocity;
  std::vector<yarp::os::NetFloat64> effort;

  bool readBare(yarp::os::ConnectionReader& connection) {
    // *** header ***
    if (!header.read(connection)) return false;

    // *** name ***
    int len = connection.expectInt();
    name.resize(len);
    for (int i=0; i<len; i++) {
      int len2 = connection.expectInt();
      name.resize(len2);
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
      name.resize(len2);
      if (!connection.expectBlock((char*)name[i].c_str(),len2)) return false;
    }

    // *** position ***
    if (connection.expectInt()!=(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE)) return false;
    len = connection.expectInt();
    position.resize(len);
    for (size_t i=0; i<len; i++) {
      position[i] = (yarp::os::NetFloat64)connection.expectDouble();
    }

    // *** velocity ***
    if (connection.expectInt()!=(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE)) return false;
    len = connection.expectInt();
    velocity.resize(len);
    for (size_t i=0; i<len; i++) {
      velocity[i] = (yarp::os::NetFloat64)connection.expectDouble();
    }

    // *** effort ***
    if (connection.expectInt()!=(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE)) return false;
    len = connection.expectInt();
    effort.resize(len);
    for (size_t i=0; i<len; i++) {
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
  typedef yarp::os::idl::BareStyle<jointState> rosStyle;
  typedef yarp::os::idl::BottleStyle<jointState> bottleStyle;

  // Name the class, ROS will need this
  yarp::os::Type getType() {
    return yarp::os::Type::byName("jointState");
  }
};

#endif

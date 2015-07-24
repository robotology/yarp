// This is an automatically generated file.
// Generated from this HeaderTest2Reply.msg definition:
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARPMSG_TYPE_HeaderTest2Reply
#define YARPMSG_TYPE_HeaderTest2Reply

#include <string>
#include <vector>
#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <TickTime.h>
#include <Header.h>
#include <HeaderTest.h>

class HeaderTest2Reply : public yarp::os::idl::WirePortable {
public:
  HeaderTest test2;

  HeaderTest2Reply() {
  }

  bool readBare(yarp::os::ConnectionReader& connection) {
    // *** test2 ***
    if (!test2.read(connection)) return false;
    return !connection.isError();
  }

  bool readBottle(yarp::os::ConnectionReader& connection) {
    connection.convertTextMode();
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListHeader(1)) return false;

    // *** test2 ***
    if (!test2.read(connection)) return false;
    return !connection.isError();
  }

  bool read(yarp::os::ConnectionReader& connection) {
    if (connection.isBareMode()) return readBare(connection);
    return readBottle(connection);
  }

  bool writeBare(yarp::os::ConnectionWriter& connection) {
    // *** test2 ***
    if (!test2.write(connection)) return false;
    return !connection.isError();
  }

  bool writeBottle(yarp::os::ConnectionWriter& connection) {
    connection.appendInt(BOTTLE_TAG_LIST);
    connection.appendInt(1);

    // *** test2 ***
    if (!test2.write(connection)) return false;
    connection.convertTextMode();
    return !connection.isError();
  }

  bool write(yarp::os::ConnectionWriter& connection) {
    if (connection.isBareMode()) return writeBare(connection);
    return writeBottle(connection);
  }

  // This class will serialize ROS style or YARP style depending on protocol.
  // If you need to force a serialization style, use one of these classes:
  typedef yarp::os::idl::BareStyle<HeaderTest2Reply> rosStyle;
  typedef yarp::os::idl::BottleStyle<HeaderTest2Reply> bottleStyle;

  // Give source text for class, ROS will need this
  yarp::os::ConstString getTypeText() {
    return "\n================================================================================\n\
MSG: HeaderTest\n\
Header header\n\
string[]  name\n\
float64[] position\n\
float64[] velocity\n\
float64[] effort\n================================================================================\n\
MSG: std_msgs/Header\n\
uint32 seq\n\
time stamp\n\
string frame_id";
  }

  // Name the class, ROS will need this
  yarp::os::Type getType() {
    yarp::os::Type typ = yarp::os::Type::byName("HeaderTest2Reply","HeaderTest2Reply");
    typ.addProperty("md5sum",yarp::os::Value("258833eeea35c84ede8af23bd403de3a"));
    typ.addProperty("message_definition",yarp::os::Value(getTypeText()));
    return typ;
  }
};

#endif

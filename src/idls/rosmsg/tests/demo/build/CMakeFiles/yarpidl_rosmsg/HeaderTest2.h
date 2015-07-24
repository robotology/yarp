// This is an automatically generated file.
// Generated from this HeaderTest2.msg definition:
//   HeaderTest test
//   HeaderTest[] lst
//   ---
//   HeaderTest test2
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARPMSG_TYPE_HeaderTest2
#define YARPMSG_TYPE_HeaderTest2

#include <string>
#include <vector>
#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <TickTime.h>
#include <Header.h>
#include <HeaderTest.h>
#include <HeaderTest2Reply.h>

class HeaderTest2 : public yarp::os::idl::WirePortable {
public:
  HeaderTest test;
  std::vector<HeaderTest> lst;

  HeaderTest2() {
  }

  bool readBare(yarp::os::ConnectionReader& connection) {
    // *** test ***
    if (!test.read(connection)) return false;

    // *** lst ***
    int len = connection.expectInt();
    lst.resize(len);
    for (int i=0; i<len; i++) {
      if (!lst[i].read(connection)) return false;
    }
    return !connection.isError();
  }

  bool readBottle(yarp::os::ConnectionReader& connection) {
    connection.convertTextMode();
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListHeader(2)) return false;

    // *** test ***
    if (!test.read(connection)) return false;

    // *** lst ***
    if (connection.expectInt()!=BOTTLE_TAG_LIST) return false;
    int len = connection.expectInt();
    lst.resize(len);
    for (int i=0; i<len; i++) {
      if (!lst[i].read(connection)) return false;
    }
    return !connection.isError();
  }

  bool read(yarp::os::ConnectionReader& connection) {
    if (connection.isBareMode()) return readBare(connection);
    return readBottle(connection);
  }

  bool writeBare(yarp::os::ConnectionWriter& connection) {
    // *** test ***
    if (!test.write(connection)) return false;

    // *** lst ***
    connection.appendInt(lst.size());
    for (size_t i=0; i<lst.size(); i++) {
      if (!lst[i].write(connection)) return false;
    }
    return !connection.isError();
  }

  bool writeBottle(yarp::os::ConnectionWriter& connection) {
    connection.appendInt(BOTTLE_TAG_LIST);
    connection.appendInt(2);

    // *** test ***
    if (!test.write(connection)) return false;

    // *** lst ***
    connection.appendInt(BOTTLE_TAG_LIST);
    connection.appendInt(lst.size());
    for (size_t i=0; i<lst.size(); i++) {
      if (!lst[i].write(connection)) return false;
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
  typedef yarp::os::idl::BareStyle<HeaderTest2> rosStyle;
  typedef yarp::os::idl::BottleStyle<HeaderTest2> bottleStyle;

  // Give source text for class, ROS will need this
  yarp::os::ConstString getTypeText() {
    return "HeaderTest test\n\
HeaderTest[] lst\n\
---\n\
HeaderTest test2\n================================================================================\n\
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
    yarp::os::Type typ = yarp::os::Type::byName("HeaderTest2","HeaderTest2");
    typ.addProperty("md5sum",yarp::os::Value("e96ae013d4f1fe3b92643f4e6fda7777"));
    typ.addProperty("message_definition",yarp::os::Value(getTypeText()));
    return typ;
  }
};

#endif

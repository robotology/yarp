// This is an automatically generated file.
// Generated from this SharedData.msg definition:
//   string text
//   float64[] content
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARPMSG_TYPE_SharedData
#define YARPMSG_TYPE_SharedData

#include <string>
#include <vector>
#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>

class SharedData : public yarp::os::idl::WirePortable {
public:
  std::string text;
  std::vector<yarp::os::NetFloat64> content;

  SharedData() {
  }

  bool readBare(yarp::os::ConnectionReader& connection) {
    // *** text ***
    int len = connection.expectInt();
    text.resize(len);
    if (!connection.expectBlock((char*)text.c_str(),len)) return false;

    // *** content ***
    len = connection.expectInt();
    content.resize(len);
    if (!connection.expectBlock((char*)&content[0],sizeof(yarp::os::NetFloat64)*len)) return false;
    return !connection.isError();
  }

  bool readBottle(yarp::os::ConnectionReader& connection) {
    connection.convertTextMode();
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListHeader(2)) return false;

    // *** text ***
    if (!reader.readString(text)) return false;

    // *** content ***
    if (connection.expectInt()!=(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE)) return false;
    int len = connection.expectInt();
    content.resize(len);
    for (size_t i=0; i<len; i++) {
      content[i] = (yarp::os::NetFloat64)connection.expectDouble();
    }
    return !connection.isError();
  }

  bool read(yarp::os::ConnectionReader& connection) {
    if (connection.isBareMode()) return readBare(connection);
    return readBottle(connection);
  }

  bool writeBare(yarp::os::ConnectionWriter& connection) {
    // *** text ***
    connection.appendInt(text.length());
    connection.appendExternalBlock((char*)text.c_str(),text.length());

    // *** content ***
    connection.appendInt(content.size());
    connection.appendExternalBlock((char*)&content[0],sizeof(yarp::os::NetFloat64)*content.size());
    return !connection.isError();
  }

  bool writeBottle(yarp::os::ConnectionWriter& connection) {
    connection.appendInt(BOTTLE_TAG_LIST);
    connection.appendInt(2);

    // *** text ***
    connection.appendInt(BOTTLE_TAG_STRING);
    connection.appendInt(text.length()+1);
    connection.appendExternalBlock((char*)text.c_str(),text.length()+1);

    // *** content ***
    connection.appendInt(BOTTLE_TAG_LIST|BOTTLE_TAG_DOUBLE);
    connection.appendInt(content.size());
    for (size_t i=0; i<content.size(); i++) {
      connection.appendDouble((double)content[i]);
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
  typedef yarp::os::idl::BareStyle<SharedData> rosStyle;
  typedef yarp::os::idl::BottleStyle<SharedData> bottleStyle;

  // Give source text for class, ROS will need this
  yarp::os::ConstString getTypeText() {
    return "string text\n\
float64[] content";
  }

  // Name the class, ROS will need this
  yarp::os::Type getType() {
    yarp::os::Type typ = yarp::os::Type::byName("SharedData","SharedData");
    typ.addProperty("md5sum",yarp::os::Value("5df4145faf37f2fa2d5c8f11a780afef"));
    typ.addProperty("message_definition",yarp::os::Value(getTypeText()));
    return typ;
  }
};

#endif

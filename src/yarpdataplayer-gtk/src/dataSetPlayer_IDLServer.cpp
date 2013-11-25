// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <dataSetPlayer_IDLServer.h>
#include <yarp/os/idl/WireTypes.h>



class dataSetPlayer_IDLServer_getHelp : public yarp::os::Portable {
public:
  std::string _return;
  virtual bool write(yarp::os::ConnectionWriter& connection) {
    yarp::os::idl::WireWriter writer(connection);
    if (!writer.writeListHeader(1)) return false;
    if (!writer.writeTag("getHelp",1,1)) return false;
    return true;
  }
  virtual bool read(yarp::os::ConnectionReader& connection) {
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListReturn()) return false;
    if (!reader.readString(_return)) {
      reader.fail();
      return false;
    }
    return true;
  }
};

class dataSetPlayer_IDLServer_step : public yarp::os::Portable {
public:
  bool _return;
  virtual bool write(yarp::os::ConnectionWriter& connection) {
    yarp::os::idl::WireWriter writer(connection);
    if (!writer.writeListHeader(1)) return false;
    if (!writer.writeTag("step",1,1)) return false;
    return true;
  }
  virtual bool read(yarp::os::ConnectionReader& connection) {
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListReturn()) return false;
    if (!reader.readBool(_return)) {
      reader.fail();
      return false;
    }
    return true;
  }
};

class dataSetPlayer_IDLServer_setFrame : public yarp::os::Portable {
public:
  std::string name;
  int32_t frameNum;
  bool _return;
  virtual bool write(yarp::os::ConnectionWriter& connection) {
    yarp::os::idl::WireWriter writer(connection);
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeTag("setFrame",1,1)) return false;
    if (!writer.writeString(name)) return false;
    if (!writer.writeI32(frameNum)) return false;
    return true;
  }
  virtual bool read(yarp::os::ConnectionReader& connection) {
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListReturn()) return false;
    if (!reader.readBool(_return)) {
      reader.fail();
      return false;
    }
    return true;
  }
};

class dataSetPlayer_IDLServer_getFrame : public yarp::os::Portable {
public:
  std::string name;
  int32_t _return;
  virtual bool write(yarp::os::ConnectionWriter& connection) {
    yarp::os::idl::WireWriter writer(connection);
    if (!writer.writeListHeader(2)) return false;
    if (!writer.writeTag("getFrame",1,1)) return false;
    if (!writer.writeString(name)) return false;
    return true;
  }
  virtual bool read(yarp::os::ConnectionReader& connection) {
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListReturn()) return false;
    if (!reader.readI32(_return)) {
      reader.fail();
      return false;
    }
    return true;
  }
};

class dataSetPlayer_IDLServer_load : public yarp::os::Portable {
public:
  std::string path;
  bool _return;
  virtual bool write(yarp::os::ConnectionWriter& connection) {
    yarp::os::idl::WireWriter writer(connection);
    if (!writer.writeListHeader(2)) return false;
    if (!writer.writeTag("load",1,1)) return false;
    if (!writer.writeString(path)) return false;
    return true;
  }
  virtual bool read(yarp::os::ConnectionReader& connection) {
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListReturn()) return false;
    if (!reader.readBool(_return)) {
      reader.fail();
      return false;
    }
    return true;
  }
};

class dataSetPlayer_IDLServer_play : public yarp::os::Portable {
public:
  bool _return;
  virtual bool write(yarp::os::ConnectionWriter& connection) {
    yarp::os::idl::WireWriter writer(connection);
    if (!writer.writeListHeader(1)) return false;
    if (!writer.writeTag("play",1,1)) return false;
    return true;
  }
  virtual bool read(yarp::os::ConnectionReader& connection) {
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListReturn()) return false;
    if (!reader.readBool(_return)) {
      reader.fail();
      return false;
    }
    return true;
  }
};

class dataSetPlayer_IDLServer_pause : public yarp::os::Portable {
public:
  bool _return;
  virtual bool write(yarp::os::ConnectionWriter& connection) {
    yarp::os::idl::WireWriter writer(connection);
    if (!writer.writeListHeader(1)) return false;
    if (!writer.writeTag("pause",1,1)) return false;
    return true;
  }
  virtual bool read(yarp::os::ConnectionReader& connection) {
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListReturn()) return false;
    if (!reader.readBool(_return)) {
      reader.fail();
      return false;
    }
    return true;
  }
};

class dataSetPlayer_IDLServer_stop : public yarp::os::Portable {
public:
  bool _return;
  virtual bool write(yarp::os::ConnectionWriter& connection) {
    yarp::os::idl::WireWriter writer(connection);
    if (!writer.writeListHeader(1)) return false;
    if (!writer.writeTag("stop",1,1)) return false;
    return true;
  }
  virtual bool read(yarp::os::ConnectionReader& connection) {
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListReturn()) return false;
    if (!reader.readBool(_return)) {
      reader.fail();
      return false;
    }
    return true;
  }
};

class dataSetPlayer_IDLServer_quit : public yarp::os::Portable {
public:
  bool _return;
  virtual bool write(yarp::os::ConnectionWriter& connection) {
    yarp::os::idl::WireWriter writer(connection);
    if (!writer.writeListHeader(1)) return false;
    if (!writer.writeTag("quit",1,1)) return false;
    return true;
  }
  virtual bool read(yarp::os::ConnectionReader& connection) {
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListReturn()) return false;
    if (!reader.readBool(_return)) {
      reader.fail();
      return false;
    }
    return true;
  }
};

std::string dataSetPlayer_IDLServer::getHelp() {
  std::string _return = "";
  dataSetPlayer_IDLServer_getHelp helper;
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
bool dataSetPlayer_IDLServer::step() {
  bool _return = false;
  dataSetPlayer_IDLServer_step helper;
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
bool dataSetPlayer_IDLServer::setFrame(const std::string& name, const int32_t frameNum) {
  bool _return = false;
  dataSetPlayer_IDLServer_setFrame helper;
  helper.name = name;
  helper.frameNum = frameNum;
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
int32_t dataSetPlayer_IDLServer::getFrame(const std::string& name) {
  int32_t _return = 0;
  dataSetPlayer_IDLServer_getFrame helper;
  helper.name = name;
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
bool dataSetPlayer_IDLServer::load(const std::string& path) {
  bool _return = false;
  dataSetPlayer_IDLServer_load helper;
  helper.path = path;
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
bool dataSetPlayer_IDLServer::play() {
  bool _return = false;
  dataSetPlayer_IDLServer_play helper;
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
bool dataSetPlayer_IDLServer::pause() {
  bool _return = false;
  dataSetPlayer_IDLServer_pause helper;
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
bool dataSetPlayer_IDLServer::stop() {
  bool _return = false;
  dataSetPlayer_IDLServer_stop helper;
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
bool dataSetPlayer_IDLServer::quit() {
  bool _return = false;
  dataSetPlayer_IDLServer_quit helper;
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}

bool dataSetPlayer_IDLServer::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  reader.expectAccept();
  if (!reader.readListHeader()) { reader.fail(); return false; }
  yarp::os::ConstString tag = reader.readTag();
  while (!reader.isError()) {
    // TODO: use quick lookup, this is just a test
    if (tag == "getHelp") {
      std::string _return;
      _return = getHelp();
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "step") {
      bool _return;
      _return = step();
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeBool(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "setFrame") {
      std::string name;
      int32_t frameNum;
      if (!reader.readString(name)) {
        reader.fail();
        return false;
      }
      if (!reader.readI32(frameNum)) {
        reader.fail();
        return false;
      }
      bool _return;
      _return = setFrame(name,frameNum);
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeBool(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "getFrame") {
      std::string name;
      if (!reader.readString(name)) {
        reader.fail();
        return false;
      }
      int32_t _return;
      _return = getFrame(name);
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeI32(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "load") {
      std::string path;
      if (!reader.readString(path)) {
        reader.fail();
        return false;
      }
      bool _return;
      _return = load(path);
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeBool(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "play") {
      bool _return;
      _return = play();
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeBool(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "pause") {
      bool _return;
      _return = pause();
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeBool(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "stop") {
      bool _return;
      _return = stop();
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeBool(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "quit") {
      bool _return;
      _return = quit();
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeBool(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (reader.noMore()) { reader.fail(); return false; }
    yarp::os::ConstString next_tag = reader.readTag();
    if (next_tag=="") break;
    tag = tag + "_" + next_tag;
  }
  return false;
}



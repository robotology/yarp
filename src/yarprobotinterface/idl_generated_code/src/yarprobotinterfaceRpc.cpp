// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <yarprobotinterfaceRpc.h>
#include <yarp/os/idl/WireTypes.h>



class yarprobotinterfaceRpc_get_phase : public yarp::os::Portable {
public:
  std::string _return;
  void init();
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class yarprobotinterfaceRpc_get_level : public yarp::os::Portable {
public:
  int32_t _return;
  void init();
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class yarprobotinterfaceRpc_get_robot : public yarp::os::Portable {
public:
  std::string _return;
  void init();
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class yarprobotinterfaceRpc_is_ready : public yarp::os::Portable {
public:
  bool _return;
  void init();
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class yarprobotinterfaceRpc_quit : public yarp::os::Portable {
public:
  std::string _return;
  void init();
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class yarprobotinterfaceRpc_bye : public yarp::os::Portable {
public:
  std::string _return;
  void init();
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class yarprobotinterfaceRpc_exit : public yarp::os::Portable {
public:
  std::string _return;
  void init();
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

bool yarprobotinterfaceRpc_get_phase::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(2)) return false;
  if (!writer.writeTag("get_phase",1,2)) return false;
  return true;
}

bool yarprobotinterfaceRpc_get_phase::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.readString(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void yarprobotinterfaceRpc_get_phase::init() {
  _return = "";
}

bool yarprobotinterfaceRpc_get_level::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(2)) return false;
  if (!writer.writeTag("get_level",1,2)) return false;
  return true;
}

bool yarprobotinterfaceRpc_get_level::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.readI32(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void yarprobotinterfaceRpc_get_level::init() {
  _return = 0;
}

bool yarprobotinterfaceRpc_get_robot::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(2)) return false;
  if (!writer.writeTag("get_robot",1,2)) return false;
  return true;
}

bool yarprobotinterfaceRpc_get_robot::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.readString(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void yarprobotinterfaceRpc_get_robot::init() {
  _return = "";
}

bool yarprobotinterfaceRpc_is_ready::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(2)) return false;
  if (!writer.writeTag("is_ready",1,2)) return false;
  return true;
}

bool yarprobotinterfaceRpc_is_ready::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.readBool(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void yarprobotinterfaceRpc_is_ready::init() {
  _return = false;
}

bool yarprobotinterfaceRpc_quit::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(1)) return false;
  if (!writer.writeTag("quit",1,1)) return false;
  return true;
}

bool yarprobotinterfaceRpc_quit::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.readString(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void yarprobotinterfaceRpc_quit::init() {
  _return = "";
}

bool yarprobotinterfaceRpc_bye::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(1)) return false;
  if (!writer.writeTag("bye",1,1)) return false;
  return true;
}

bool yarprobotinterfaceRpc_bye::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.readString(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void yarprobotinterfaceRpc_bye::init() {
  _return = "";
}

bool yarprobotinterfaceRpc_exit::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(1)) return false;
  if (!writer.writeTag("exit",1,1)) return false;
  return true;
}

bool yarprobotinterfaceRpc_exit::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.readString(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void yarprobotinterfaceRpc_exit::init() {
  _return = "";
}

yarprobotinterfaceRpc::yarprobotinterfaceRpc() {
  yarp().setOwner(*this);
}
std::string yarprobotinterfaceRpc::get_phase() {
  std::string _return = "";
  yarprobotinterfaceRpc_get_phase helper;
  helper.init();
  if (!yarp().canWrite()) {
    yError("Missing server method '%s'?","std::string yarprobotinterfaceRpc::get_phase()");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
int32_t yarprobotinterfaceRpc::get_level() {
  int32_t _return = 0;
  yarprobotinterfaceRpc_get_level helper;
  helper.init();
  if (!yarp().canWrite()) {
    yError("Missing server method '%s'?","int32_t yarprobotinterfaceRpc::get_level()");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
std::string yarprobotinterfaceRpc::get_robot() {
  std::string _return = "";
  yarprobotinterfaceRpc_get_robot helper;
  helper.init();
  if (!yarp().canWrite()) {
    yError("Missing server method '%s'?","std::string yarprobotinterfaceRpc::get_robot()");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
bool yarprobotinterfaceRpc::is_ready() {
  bool _return = false;
  yarprobotinterfaceRpc_is_ready helper;
  helper.init();
  if (!yarp().canWrite()) {
    yError("Missing server method '%s'?","bool yarprobotinterfaceRpc::is_ready()");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
std::string yarprobotinterfaceRpc::quit() {
  std::string _return = "";
  yarprobotinterfaceRpc_quit helper;
  helper.init();
  if (!yarp().canWrite()) {
    yError("Missing server method '%s'?","std::string yarprobotinterfaceRpc::quit()");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
std::string yarprobotinterfaceRpc::bye() {
  std::string _return = "";
  yarprobotinterfaceRpc_bye helper;
  helper.init();
  if (!yarp().canWrite()) {
    yError("Missing server method '%s'?","std::string yarprobotinterfaceRpc::bye()");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
std::string yarprobotinterfaceRpc::exit() {
  std::string _return = "";
  yarprobotinterfaceRpc_exit helper;
  helper.init();
  if (!yarp().canWrite()) {
    yError("Missing server method '%s'?","std::string yarprobotinterfaceRpc::exit()");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}

bool yarprobotinterfaceRpc::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  reader.expectAccept();
  if (!reader.readListHeader()) { reader.fail(); return false; }
  yarp::os::ConstString tag = reader.readTag();
  bool direct = (tag=="__direct__");
  if (direct) tag = reader.readTag();
  while (!reader.isError()) {
    // TODO: use quick lookup, this is just a test
    if (tag == "get_phase") {
      std::string _return;
      _return = get_phase();
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "get_level") {
      int32_t _return;
      _return = get_level();
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeI32(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "get_robot") {
      std::string _return;
      _return = get_robot();
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "is_ready") {
      bool _return;
      _return = is_ready();
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeBool(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "quit") {
      std::string _return;
      _return = quit();
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "bye") {
      std::string _return;
      _return = bye();
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "exit") {
      std::string _return;
      _return = exit();
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "help") {
      std::string functionName;
      if (!reader.readString(functionName)) {
        functionName = "--all";
      }
      std::vector<std::string> _return=help(functionName);
      yarp::os::idl::WireWriter writer(reader);
        if (!writer.isNull()) {
          if (!writer.writeListHeader(2)) return false;
          if (!writer.writeTag("many",1, 0)) return false;
          if (!writer.writeListBegin(BOTTLE_TAG_INT, static_cast<uint32_t>(_return.size()))) return false;
          std::vector<std::string> ::iterator _iterHelp;
          for (_iterHelp = _return.begin(); _iterHelp != _return.end(); ++_iterHelp)
          {
            if (!writer.writeString(*_iterHelp)) return false;
           }
          if (!writer.writeListEnd()) return false;
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

std::vector<std::string> yarprobotinterfaceRpc::help(const std::string& functionName) {
  bool showAll=(functionName=="--all");
  std::vector<std::string> helpString;
  if(showAll) {
    helpString.push_back("*** Available commands:");
    helpString.push_back("get_phase");
    helpString.push_back("get_level");
    helpString.push_back("get_robot");
    helpString.push_back("is_ready");
    helpString.push_back("quit");
    helpString.push_back("bye");
    helpString.push_back("exit");
    helpString.push_back("help");
  }
  else {
    if (functionName=="get_phase") {
      helpString.push_back("std::string get_phase() ");
      helpString.push_back("Returns current phase. ");
    }
    if (functionName=="get_level") {
      helpString.push_back("int32_t get_level() ");
      helpString.push_back("Returns current level. ");
    }
    if (functionName=="get_robot") {
      helpString.push_back("std::string get_robot() ");
      helpString.push_back("Returns robot name. ");
    }
    if (functionName=="is_ready") {
      helpString.push_back("bool is_ready() ");
      helpString.push_back("Returns true if yarprobotinterface is ready (all startup actions ");
      helpString.push_back("performed and no interrupt called). ");
    }
    if (functionName=="quit") {
      helpString.push_back("std::string quit() ");
      helpString.push_back("Closes yarprobotinterface. ");
    }
    if (functionName=="bye") {
      helpString.push_back("std::string bye() ");
      helpString.push_back("Closes yarprobotinterface. ");
    }
    if (functionName=="exit") {
      helpString.push_back("std::string exit() ");
      helpString.push_back("Closes yarprobotinterface. ");
    }
    if (functionName=="help") {
      helpString.push_back("std::vector<std::string> help(const std::string& functionName=\"--all\")");
      helpString.push_back("Return list of available commands, or help message for a specific function");
      helpString.push_back("@param functionName name of command for which to get a detailed description. If none or '--all' is provided, print list of available commands");
      helpString.push_back("@return list of strings (one string per line)");
    }
  }
  if ( helpString.empty()) helpString.push_back("Command not found");
  return helpString;
}



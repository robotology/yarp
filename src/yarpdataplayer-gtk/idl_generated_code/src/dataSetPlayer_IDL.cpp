// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <dataSetPlayer_IDL.h>
#include <yarp/os/idl/WireTypes.h>



class dataSetPlayer_IDL_step : public yarp::os::Portable {
public:
  bool _return;
  void init();
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class dataSetPlayer_IDL_setFrame : public yarp::os::Portable {
public:
  std::string name;
  int32_t frameNum;
  bool _return;
  void init(const std::string& name, const int32_t frameNum);
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class dataSetPlayer_IDL_getFrame : public yarp::os::Portable {
public:
  std::string name;
  int32_t _return;
  void init(const std::string& name);
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class dataSetPlayer_IDL_load : public yarp::os::Portable {
public:
  std::string path;
  bool _return;
  void init(const std::string& path);
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class dataSetPlayer_IDL_play : public yarp::os::Portable {
public:
  bool _return;
  void init();
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class dataSetPlayer_IDL_pause : public yarp::os::Portable {
public:
  bool _return;
  void init();
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class dataSetPlayer_IDL_stop : public yarp::os::Portable {
public:
  bool _return;
  void init();
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class dataSetPlayer_IDL_quit : public yarp::os::Portable {
public:
  bool _return;
  void init();
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

bool dataSetPlayer_IDL_step::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(1)) return false;
  if (!writer.writeTag("step",1,1)) return false;
  return true;
}

bool dataSetPlayer_IDL_step::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.readBool(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void dataSetPlayer_IDL_step::init() {
  _return = false;
}

bool dataSetPlayer_IDL_setFrame::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(3)) return false;
  if (!writer.writeTag("setFrame",1,1)) return false;
  if (!writer.writeString(name)) return false;
  if (!writer.writeI32(frameNum)) return false;
  return true;
}

bool dataSetPlayer_IDL_setFrame::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.readBool(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void dataSetPlayer_IDL_setFrame::init(const std::string& name, const int32_t frameNum) {
  _return = false;
  this->name = name;
  this->frameNum = frameNum;
}

bool dataSetPlayer_IDL_getFrame::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(2)) return false;
  if (!writer.writeTag("getFrame",1,1)) return false;
  if (!writer.writeString(name)) return false;
  return true;
}

bool dataSetPlayer_IDL_getFrame::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.readI32(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void dataSetPlayer_IDL_getFrame::init(const std::string& name) {
  _return = 0;
  this->name = name;
}

bool dataSetPlayer_IDL_load::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(2)) return false;
  if (!writer.writeTag("load",1,1)) return false;
  if (!writer.writeString(path)) return false;
  return true;
}

bool dataSetPlayer_IDL_load::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.readBool(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void dataSetPlayer_IDL_load::init(const std::string& path) {
  _return = false;
  this->path = path;
}

bool dataSetPlayer_IDL_play::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(1)) return false;
  if (!writer.writeTag("play",1,1)) return false;
  return true;
}

bool dataSetPlayer_IDL_play::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.readBool(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void dataSetPlayer_IDL_play::init() {
  _return = false;
}

bool dataSetPlayer_IDL_pause::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(1)) return false;
  if (!writer.writeTag("pause",1,1)) return false;
  return true;
}

bool dataSetPlayer_IDL_pause::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.readBool(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void dataSetPlayer_IDL_pause::init() {
  _return = false;
}

bool dataSetPlayer_IDL_stop::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(1)) return false;
  if (!writer.writeTag("stop",1,1)) return false;
  return true;
}

bool dataSetPlayer_IDL_stop::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.readBool(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void dataSetPlayer_IDL_stop::init() {
  _return = false;
}

bool dataSetPlayer_IDL_quit::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(1)) return false;
  if (!writer.writeTag("quit",1,1)) return false;
  return true;
}

bool dataSetPlayer_IDL_quit::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.readBool(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void dataSetPlayer_IDL_quit::init() {
  _return = false;
}

dataSetPlayer_IDL::dataSetPlayer_IDL() {
  yarp().setOwner(*this);
}
bool dataSetPlayer_IDL::step() {
  bool _return = false;
  dataSetPlayer_IDL_step helper;
  helper.init();
  if (!yarp().canWrite()) {
    yError("Missing server method '%s'?","bool dataSetPlayer_IDL::step()");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
bool dataSetPlayer_IDL::setFrame(const std::string& name, const int32_t frameNum) {
  bool _return = false;
  dataSetPlayer_IDL_setFrame helper;
  helper.init(name,frameNum);
  if (!yarp().canWrite()) {
    yError("Missing server method '%s'?","bool dataSetPlayer_IDL::setFrame(const std::string& name, const int32_t frameNum)");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
int32_t dataSetPlayer_IDL::getFrame(const std::string& name) {
  int32_t _return = 0;
  dataSetPlayer_IDL_getFrame helper;
  helper.init(name);
  if (!yarp().canWrite()) {
    yError("Missing server method '%s'?","int32_t dataSetPlayer_IDL::getFrame(const std::string& name)");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
bool dataSetPlayer_IDL::load(const std::string& path) {
  bool _return = false;
  dataSetPlayer_IDL_load helper;
  helper.init(path);
  if (!yarp().canWrite()) {
    yError("Missing server method '%s'?","bool dataSetPlayer_IDL::load(const std::string& path)");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
bool dataSetPlayer_IDL::play() {
  bool _return = false;
  dataSetPlayer_IDL_play helper;
  helper.init();
  if (!yarp().canWrite()) {
    yError("Missing server method '%s'?","bool dataSetPlayer_IDL::play()");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
bool dataSetPlayer_IDL::pause() {
  bool _return = false;
  dataSetPlayer_IDL_pause helper;
  helper.init();
  if (!yarp().canWrite()) {
    yError("Missing server method '%s'?","bool dataSetPlayer_IDL::pause()");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
bool dataSetPlayer_IDL::stop() {
  bool _return = false;
  dataSetPlayer_IDL_stop helper;
  helper.init();
  if (!yarp().canWrite()) {
    yError("Missing server method '%s'?","bool dataSetPlayer_IDL::stop()");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
bool dataSetPlayer_IDL::quit() {
  bool _return = false;
  dataSetPlayer_IDL_quit helper;
  helper.init();
  if (!yarp().canWrite()) {
    yError("Missing server method '%s'?","bool dataSetPlayer_IDL::quit()");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}

bool dataSetPlayer_IDL::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  reader.expectAccept();
  if (!reader.readListHeader()) { reader.fail(); return false; }
  yarp::os::ConstString tag = reader.readTag();
  bool direct = (tag=="__direct__");
  if (direct) tag = reader.readTag();
  while (!reader.isError()) {
    // TODO: use quick lookup, this is just a test
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

std::vector<std::string> dataSetPlayer_IDL::help(const std::string& functionName) {
  bool showAll=(functionName=="--all");
  std::vector<std::string> helpString;
  if(showAll) {
    helpString.push_back("*** Available commands:");
    helpString.push_back("step");
    helpString.push_back("setFrame");
    helpString.push_back("getFrame");
    helpString.push_back("load");
    helpString.push_back("play");
    helpString.push_back("pause");
    helpString.push_back("stop");
    helpString.push_back("quit");
    helpString.push_back("help");
  }
  else {
    if (functionName=="step") {
      helpString.push_back("bool step() ");
      helpString.push_back("Steps the player once. The player will be stepped ");
      helpString.push_back("until all parts have sent data ");
      helpString.push_back("@return true/false on success/failure ");
    }
    if (functionName=="setFrame") {
      helpString.push_back("bool setFrame(const std::string& name, const int32_t frameNum) ");
      helpString.push_back("Sets the frame number to the user desired frame. ");
      helpString.push_back("@param name specifies the name of the loaded data ");
      helpString.push_back("@param frameNum specifies the frame number the user ");
      helpString.push_back(" would like to skip to ");
      helpString.push_back("@return true/false on success/failure ");
    }
    if (functionName=="getFrame") {
      helpString.push_back("int32_t getFrame(const std::string& name) ");
      helpString.push_back("Gets the frame number the user is requesting ");
      helpString.push_back("@param name specifies the name of the data to modify ");
      helpString.push_back(" would like to skip to ");
      helpString.push_back("@return i32 returns the current frame index ");
    }
    if (functionName=="load") {
      helpString.push_back("bool load(const std::string& path) ");
      helpString.push_back("Loads a dataset from a path ");
      helpString.push_back("@return true/false on success/failure ");
    }
    if (functionName=="play") {
      helpString.push_back("bool play() ");
      helpString.push_back("Plays the dataSets ");
      helpString.push_back("@return true/false on success/failure ");
    }
    if (functionName=="pause") {
      helpString.push_back("bool pause() ");
      helpString.push_back("Pauses the dataSets ");
      helpString.push_back("@return true/false on success/failure ");
    }
    if (functionName=="stop") {
      helpString.push_back("bool stop() ");
      helpString.push_back("Stops the dataSets ");
      helpString.push_back("@return true/false on success/failure ");
    }
    if (functionName=="quit") {
      helpString.push_back("bool quit() ");
      helpString.push_back("Quit the module. ");
      helpString.push_back("@return true/false on success/failure ");
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



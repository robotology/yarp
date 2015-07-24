// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <Wrapping.h>
#include <yarp/os/idl/WireTypes.h>



class Wrapping_check : public yarp::os::Portable {
public:
  yarp::os::Value param;
  int32_t _return;
  void init(const yarp::os::Value& param);
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class Wrapping_getBottle : public yarp::os::Portable {
public:
  yarp::os::Bottle _return;
  void init();
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

bool Wrapping_check::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(2)) return false;
  if (!writer.writeTag("check",1,1)) return false;
  if (!writer.write(param)) return false;
  return true;
}

bool Wrapping_check::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.readI32(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void Wrapping_check::init(const yarp::os::Value& param) {
  _return = 0;
  this->param = param;
}

bool Wrapping_getBottle::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(1)) return false;
  if (!writer.writeTag("getBottle",1,1)) return false;
  return true;
}

bool Wrapping_getBottle::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.read(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void Wrapping_getBottle::init() {
  _return;
}

Wrapping::Wrapping() {
  yarp().setOwner(*this);
}
int32_t Wrapping::check(const yarp::os::Value& param) {
  int32_t _return = 0;
  Wrapping_check helper;
  helper.init(param);
  if (!yarp().canWrite()) {
    fprintf(stderr,"Missing server method '%s'?\n","int32_t Wrapping::check(const yarp::os::Value& param)");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
yarp::os::Bottle Wrapping::getBottle() {
  yarp::os::Bottle _return;
  Wrapping_getBottle helper;
  helper.init();
  if (!yarp().canWrite()) {
    fprintf(stderr,"Missing server method '%s'?\n","yarp::os::Bottle Wrapping::getBottle()");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}

bool Wrapping::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  reader.expectAccept();
  if (!reader.readListHeader()) { reader.fail(); return false; }
  yarp::os::ConstString tag = reader.readTag();
  bool direct = (tag=="__direct__");
  if (direct) tag = reader.readTag();
  while (!reader.isError()) {
    // TODO: use quick lookup, this is just a test
    if (tag == "check") {
      yarp::os::Value param;
      if (!reader.read(param)) {
        reader.fail();
        return false;
      }
      int32_t _return;
      _return = check(param);
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeI32(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "getBottle") {
      yarp::os::Bottle _return;
      _return = getBottle();
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.write(_return)) return false;
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

std::vector<std::string> Wrapping::help(const std::string& functionName) {
  bool showAll=(functionName=="--all");
  std::vector<std::string> helpString;
  if(showAll) {
    helpString.push_back("*** Available commands:");
    helpString.push_back("check");
    helpString.push_back("getBottle");
    helpString.push_back("help");
  }
  else {
    if (functionName=="check") {
      helpString.push_back("int32_t check(const yarp::os::Value& param) ");
    }
    if (functionName=="getBottle") {
      helpString.push_back("yarp::os::Bottle getBottle() ");
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



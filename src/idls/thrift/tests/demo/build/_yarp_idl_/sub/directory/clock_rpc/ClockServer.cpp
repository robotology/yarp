// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <sub/directory/ClockServer.h>
#include <yarp/os/idl/WireTypes.h>

namespace testing {


class ClockServer_pauseSimulation : public yarp::os::Portable {
public:
  void init();
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class ClockServer_continueSimulation : public yarp::os::Portable {
public:
  void init();
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class ClockServer_stepSimulation : public yarp::os::Portable {
public:
  int32_t numberOfSteps;
  void init(const int32_t numberOfSteps);
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

bool ClockServer_pauseSimulation::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(1)) return false;
  if (!writer.writeTag("pauseSimulation",1,1)) return false;
  return true;
}

bool ClockServer_pauseSimulation::read(yarp::os::ConnectionReader& connection) {
  YARP_UNUSED(connection);
  return true;
}

void ClockServer_pauseSimulation::init() {
}

bool ClockServer_continueSimulation::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(1)) return false;
  if (!writer.writeTag("continueSimulation",1,1)) return false;
  return true;
}

bool ClockServer_continueSimulation::read(yarp::os::ConnectionReader& connection) {
  YARP_UNUSED(connection);
  return true;
}

void ClockServer_continueSimulation::init() {
}

bool ClockServer_stepSimulation::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(2)) return false;
  if (!writer.writeTag("stepSimulation",1,1)) return false;
  if (!writer.writeI32(numberOfSteps)) return false;
  return true;
}

bool ClockServer_stepSimulation::read(yarp::os::ConnectionReader& connection) {
  YARP_UNUSED(connection);
  return true;
}

void ClockServer_stepSimulation::init(const int32_t numberOfSteps) {
  this->numberOfSteps = numberOfSteps;
}

ClockServer::ClockServer() {
  yarp().setOwner(*this);
}
void ClockServer::pauseSimulation() {
  ClockServer_pauseSimulation helper;
  helper.init();
  if (!yarp().canWrite()) {
    fprintf(stderr,"Missing server method '%s'?\n","void ClockServer::pauseSimulation()");
  }
  yarp().write(helper);
}
void ClockServer::continueSimulation() {
  ClockServer_continueSimulation helper;
  helper.init();
  if (!yarp().canWrite()) {
    fprintf(stderr,"Missing server method '%s'?\n","void ClockServer::continueSimulation()");
  }
  yarp().write(helper);
}
void ClockServer::stepSimulation(const int32_t numberOfSteps) {
  ClockServer_stepSimulation helper;
  helper.init(numberOfSteps);
  if (!yarp().canWrite()) {
    fprintf(stderr,"Missing server method '%s'?\n","void ClockServer::stepSimulation(const int32_t numberOfSteps)");
  }
  yarp().write(helper);
}

bool ClockServer::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  reader.expectAccept();
  if (!reader.readListHeader()) { reader.fail(); return false; }
  yarp::os::ConstString tag = reader.readTag();
  bool direct = (tag=="__direct__");
  if (direct) tag = reader.readTag();
  while (!reader.isError()) {
    // TODO: use quick lookup, this is just a test
    if (tag == "pauseSimulation") {
      if (!direct) {
        ClockServer_pauseSimulation helper;
        helper.init();
        yarp().callback(helper,*this,"__direct__");
      } else {
        pauseSimulation();
      }
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeOnewayResponse()) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "continueSimulation") {
      if (!direct) {
        ClockServer_continueSimulation helper;
        helper.init();
        yarp().callback(helper,*this,"__direct__");
      } else {
        continueSimulation();
      }
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeOnewayResponse()) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "stepSimulation") {
      int32_t numberOfSteps;
      if (!reader.readI32(numberOfSteps)) {
        numberOfSteps = 1;
      }
      if (!direct) {
        ClockServer_stepSimulation helper;
        helper.init(numberOfSteps);
        yarp().callback(helper,*this,"__direct__");
      } else {
        stepSimulation(numberOfSteps);
      }
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeOnewayResponse()) return false;
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

std::vector<std::string> ClockServer::help(const std::string& functionName) {
  bool showAll=(functionName=="--all");
  std::vector<std::string> helpString;
  if(showAll) {
    helpString.push_back("*** Available commands:");
    helpString.push_back("pauseSimulation");
    helpString.push_back("continueSimulation");
    helpString.push_back("stepSimulation");
    helpString.push_back("help");
  }
  else {
    if (functionName=="pauseSimulation") {
      helpString.push_back("void pauseSimulation() ");
    }
    if (functionName=="continueSimulation") {
      helpString.push_back("void continueSimulation() ");
    }
    if (functionName=="stepSimulation") {
      helpString.push_back("void stepSimulation(const int32_t numberOfSteps = 1) ");
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
} // namespace



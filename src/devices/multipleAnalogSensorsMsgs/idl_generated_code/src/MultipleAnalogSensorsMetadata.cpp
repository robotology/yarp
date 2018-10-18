/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <MultipleAnalogSensorsMetadata.h>
#include <yarp/os/idl/WireTypes.h>



class MultipleAnalogSensorsMetadata_getMetadata : public yarp::os::Portable {
public:
  SensorRPCData _return;
  void init();
  virtual bool write(yarp::os::ConnectionWriter& connection) const override;
  virtual bool read(yarp::os::ConnectionReader& connection) override;
};

bool MultipleAnalogSensorsMetadata_getMetadata::write(yarp::os::ConnectionWriter& connection) const {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(1)) return false;
  if (!writer.writeTag("getMetadata",1,1)) return false;
  return true;
}

bool MultipleAnalogSensorsMetadata_getMetadata::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.read(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void MultipleAnalogSensorsMetadata_getMetadata::init() {
}

MultipleAnalogSensorsMetadata::MultipleAnalogSensorsMetadata() {
  yarp().setOwner(*this);
}
SensorRPCData MultipleAnalogSensorsMetadata::getMetadata() {
  SensorRPCData _return;
  MultipleAnalogSensorsMetadata_getMetadata helper;
  helper.init();
  if (!yarp().canWrite()) {
    yError("Missing server method '%s'?","SensorRPCData MultipleAnalogSensorsMetadata::getMetadata()");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}

bool MultipleAnalogSensorsMetadata::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  reader.expectAccept();
  if (!reader.readListHeader()) { reader.fail(); return false; }
  std::string tag = reader.readTag();
  bool direct = (tag=="__direct__");
  if (direct) tag = reader.readTag();
  while (!reader.isError()) {
    // TODO: use quick lookup, this is just a test
    if (tag == "getMetadata") {
      SensorRPCData _return;
      _return = getMetadata();
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(9)) return false;
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
          if (!writer.writeListBegin(BOTTLE_TAG_INT32, static_cast<uint32_t>(_return.size()))) return false;
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
    std::string next_tag = reader.readTag();
    if (next_tag=="") break;
    tag.append("_").append(next_tag);
  }
  return false;
}

std::vector<std::string> MultipleAnalogSensorsMetadata::help(const std::string& functionName) {
  bool showAll=(functionName=="--all");
  std::vector<std::string> helpString;
  if(showAll) {
    helpString.push_back("*** Available commands:");
    helpString.push_back("getMetadata");
    helpString.push_back("help");
  }
  else {
    if (functionName=="getMetadata") {
      helpString.push_back("SensorRPCData getMetadata() ");
      helpString.push_back("Read the sensor metadata necessary to configure the MultipleAnalogSensorsClient device. ");
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



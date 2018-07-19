/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <secondInterface/DemoExtended.h>
#include <yarp/os/idl/WireTypes.h>

namespace yarp { namespace test {


class DemoExtended_multiply_point : public yarp::os::Portable {
public:
  Point3D x;
  double factor;
  Point3D _return;
  void init(const Point3D& x, const double factor);
  virtual bool write(yarp::os::ConnectionWriter& connection) const override;
  virtual bool read(yarp::os::ConnectionReader& connection) override;
};

bool DemoExtended_multiply_point::write(yarp::os::ConnectionWriter& connection) const {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(4)) return false;
  if (!writer.writeTag("multiply_point",1,2)) return false;
  if (!writer.write(x)) return false;
  if (!writer.writeFloat64(factor)) return false;
  return true;
}

bool DemoExtended_multiply_point::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.read(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void DemoExtended_multiply_point::init(const Point3D& x, const double factor) {
  this->x = x;
  this->factor = factor;
}

DemoExtended::DemoExtended() {
  yarp().setOwner(*this);
}
Point3D DemoExtended::multiply_point(const Point3D& x, const double factor) {
  Point3D _return;
  DemoExtended_multiply_point helper;
  helper.init(x,factor);
  if (!yarp().canWrite()) {
    yError("Missing server method '%s'?","Point3D DemoExtended::multiply_point(const Point3D& x, const double factor)");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}

bool DemoExtended::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  reader.expectAccept();
  if (!reader.readListHeader()) { reader.fail(); return false; }
  std::string tag = reader.readTag();
  bool direct = (tag=="__direct__");
  if (direct) tag = reader.readTag();
  while (!reader.isError()) {
    // TODO: use quick lookup, this is just a test
    if (tag == "multiply_point") {
      Point3D x;
      double factor;
      if (!reader.read(x)) {
        reader.fail();
        return false;
      }
      if (!reader.readFloat64(factor)) {
        reader.fail();
        return false;
      }
      Point3D _return;
      _return = multiply_point(x,factor);
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
    tag = tag + "_" + next_tag;
  }
  return false;
}

std::vector<std::string> DemoExtended::help(const std::string& functionName) {
  bool showAll=(functionName=="--all");
  std::vector<std::string> helpString;
  if(showAll) {
    helpString.push_back("*** Available commands:");
    helpString.push_back("multiply_point");
    helpString.push_back("help");
  }
  else {
    if (functionName=="multiply_point") {
      helpString.push_back("Point3D multiply_point(const Point3D& x, const double factor) ");
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
}} // namespace



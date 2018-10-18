/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <secondInterface/Demo.h>
#include <yarp/os/idl/WireTypes.h>

namespace yarp { namespace test {


class Demo_get_answer : public yarp::os::Portable {
public:
  std::int32_t _return;
  void init();
  virtual bool write(yarp::os::ConnectionWriter& connection) const override;
  virtual bool read(yarp::os::ConnectionReader& connection) override;
};

class Demo_add_one : public yarp::os::Portable {
public:
  std::int32_t x;
  std::int32_t _return;
  void init(const std::int32_t x);
  virtual bool write(yarp::os::ConnectionWriter& connection) const override;
  virtual bool read(yarp::os::ConnectionReader& connection) override;
};

class Demo_double_down : public yarp::os::Portable {
public:
  std::int32_t x;
  std::int32_t _return;
  void init(const std::int32_t x);
  virtual bool write(yarp::os::ConnectionWriter& connection) const override;
  virtual bool read(yarp::os::ConnectionReader& connection) override;
};

class Demo_add_point : public yarp::os::Portable {
public:
   ::yarp::test::PointD x;
   ::yarp::test::PointD y;
   ::yarp::test::PointD _return;
  void init(const  ::yarp::test::PointD& x, const  ::yarp::test::PointD& y);
  virtual bool write(yarp::os::ConnectionWriter& connection) const override;
  virtual bool read(yarp::os::ConnectionReader& connection) override;
};

bool Demo_get_answer::write(yarp::os::ConnectionWriter& connection) const {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(2)) return false;
  if (!writer.writeTag("get_answer",1,2)) return false;
  return true;
}

bool Demo_get_answer::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.readI32(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void Demo_get_answer::init() {
  _return = 0;
}

bool Demo_add_one::write(yarp::os::ConnectionWriter& connection) const {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(3)) return false;
  if (!writer.writeTag("add_one",1,2)) return false;
  if (!writer.writeI32(x)) return false;
  return true;
}

bool Demo_add_one::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.readI32(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void Demo_add_one::init(const std::int32_t x) {
  _return = 0;
  this->x = x;
}

bool Demo_double_down::write(yarp::os::ConnectionWriter& connection) const {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(3)) return false;
  if (!writer.writeTag("double_down",1,2)) return false;
  if (!writer.writeI32(x)) return false;
  return true;
}

bool Demo_double_down::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.readI32(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void Demo_double_down::init(const std::int32_t x) {
  _return = 0;
  this->x = x;
}

bool Demo_add_point::write(yarp::os::ConnectionWriter& connection) const {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(8)) return false;
  if (!writer.writeTag("add_point",1,2)) return false;
  if (!writer.write(x)) return false;
  if (!writer.write(y)) return false;
  return true;
}

bool Demo_add_point::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.read(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void Demo_add_point::init(const  ::yarp::test::PointD& x, const  ::yarp::test::PointD& y) {
  this->x = x;
  this->y = y;
}

Demo::Demo() {
  yarp().setOwner(*this);
}
std::int32_t Demo::get_answer() {
  std::int32_t _return = 0;
  Demo_get_answer helper;
  helper.init();
  if (!yarp().canWrite()) {
    yError("Missing server method '%s'?","std::int32_t Demo::get_answer()");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
std::int32_t Demo::add_one(const std::int32_t x) {
  std::int32_t _return = 0;
  Demo_add_one helper;
  helper.init(x);
  if (!yarp().canWrite()) {
    yError("Missing server method '%s'?","std::int32_t Demo::add_one(const std::int32_t x)");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
std::int32_t Demo::double_down(const std::int32_t x) {
  std::int32_t _return = 0;
  Demo_double_down helper;
  helper.init(x);
  if (!yarp().canWrite()) {
    yError("Missing server method '%s'?","std::int32_t Demo::double_down(const std::int32_t x)");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
 ::yarp::test::PointD Demo::add_point(const  ::yarp::test::PointD& x, const  ::yarp::test::PointD& y) {
   ::yarp::test::PointD _return;
  Demo_add_point helper;
  helper.init(x,y);
  if (!yarp().canWrite()) {
    yError("Missing server method '%s'?"," ::yarp::test::PointD Demo::add_point(const  ::yarp::test::PointD& x, const  ::yarp::test::PointD& y)");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}

bool Demo::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  reader.expectAccept();
  if (!reader.readListHeader()) { reader.fail(); return false; }
  std::string tag = reader.readTag();
  bool direct = (tag=="__direct__");
  if (direct) tag = reader.readTag();
  while (!reader.isError()) {
    // TODO: use quick lookup, this is just a test
    if (tag == "get_answer") {
      std::int32_t _return;
      _return = get_answer();
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeI32(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "add_one") {
      std::int32_t x;
      if (!reader.readI32(x)) {
        x = 0;
      }
      std::int32_t _return;
      _return = add_one(x);
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeI32(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "double_down") {
      std::int32_t x;
      if (!reader.readI32(x)) {
        reader.fail();
        return false;
      }
      std::int32_t _return;
      _return = double_down(x);
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeI32(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "add_point") {
       ::yarp::test::PointD x;
       ::yarp::test::PointD y;
      if (!reader.read(x)) {
        reader.fail();
        return false;
      }
      if (!reader.read(y)) {
        reader.fail();
        return false;
      }
       ::yarp::test::PointD _return;
      _return = add_point(x,y);
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(3)) return false;
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

std::vector<std::string> Demo::help(const std::string& functionName) {
  bool showAll=(functionName=="--all");
  std::vector<std::string> helpString;
  if(showAll) {
    helpString.push_back("*** Available commands:");
    helpString.push_back("get_answer");
    helpString.push_back("add_one");
    helpString.push_back("double_down");
    helpString.push_back("add_point");
    helpString.push_back("help");
  }
  else {
    if (functionName=="get_answer") {
      helpString.push_back("std::int32_t get_answer() ");
    }
    if (functionName=="add_one") {
      helpString.push_back("std::int32_t add_one(const std::int32_t x = 0) ");
    }
    if (functionName=="double_down") {
      helpString.push_back("std::int32_t double_down(const std::int32_t x) ");
    }
    if (functionName=="add_point") {
      helpString.push_back(" ::yarp::test::PointD add_point(const  ::yarp::test::PointD& x, const  ::yarp::test::PointD& y) ");
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



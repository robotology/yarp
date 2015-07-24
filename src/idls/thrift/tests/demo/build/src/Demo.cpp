// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <Demo.h>
#include <yarp/os/idl/WireTypes.h>



class Demo_get_answer : public yarp::os::Portable {
public:
  int32_t _return;
  void init();
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class Demo_set_answer : public yarp::os::Portable {
public:
  int32_t rightAnswer;
  bool _return;
  void init(const int32_t rightAnswer);
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class Demo_add_one : public yarp::os::Portable {
public:
  int32_t x;
  int32_t _return;
  void init(const int32_t x);
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class Demo_start : public yarp::os::Portable {
public:
  bool _return;
  void init();
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class Demo_stop : public yarp::os::Portable {
public:
  bool _return;
  void init();
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class Demo_is_running : public yarp::os::Portable {
public:
  bool _return;
  void init();
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class Demo_test_void : public yarp::os::Portable {
public:
  int32_t x;
  void init(const int32_t x);
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class Demo_test_1way : public yarp::os::Portable {
public:
  int32_t x;
  void init(const int32_t x);
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class Demo_test_enums : public yarp::os::Portable {
public:
  DemoEnum x;
  DemoEnum _return;
  void init(const DemoEnum x);
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class Demo_test_enum_vector : public yarp::os::Portable {
public:
  std::vector<DemoEnum>  x;
  std::vector<DemoEnum>  _return;
  void init(const std::vector<DemoEnum> & x);
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class Demo_test_defaults : public yarp::os::Portable {
public:
  int32_t x;
  bool _return;
  void init(const int32_t x);
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class Demo_add_pair : public yarp::os::Portable {
public:
  int32_t x;
  int32_t y;
  int32_t _return;
  void init(const int32_t x, const int32_t y);
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class Demo_test_partial : public yarp::os::Portable {
public:
  int32_t x;
  std::vector<int32_t>  lst;
  int32_t y;
  int32_t _return;
  void init(const int32_t x, const std::vector<int32_t> & lst, const int32_t y);
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class Demo_test_tail_defaults : public yarp::os::Portable {
public:
  DemoEnum x;
  int32_t _return;
  void init(const DemoEnum x);
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class Demo_test_longer_tail_defaults : public yarp::os::Portable {
public:
  int32_t ignore;
  DemoEnum _enum;
  int32_t _int;
  std::string _string;
  int32_t _return;
  void init(const int32_t ignore, const DemoEnum _enum, const int32_t _int, const std::string& _string);
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class Demo_do_start_a_service : public yarp::os::Portable {
public:
  void init();
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class Demo_do_check_for_service : public yarp::os::Portable {
public:
  bool _return;
  void init();
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

class Demo_do_stop_a_service : public yarp::os::Portable {
public:
  void init();
  virtual bool write(yarp::os::ConnectionWriter& connection);
  virtual bool read(yarp::os::ConnectionReader& connection);
};

bool Demo_get_answer::write(yarp::os::ConnectionWriter& connection) {
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

bool Demo_set_answer::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(3)) return false;
  if (!writer.writeTag("set_answer",1,2)) return false;
  if (!writer.writeI32(rightAnswer)) return false;
  return true;
}

bool Demo_set_answer::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.readBool(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void Demo_set_answer::init(const int32_t rightAnswer) {
  _return = false;
  this->rightAnswer = rightAnswer;
}

bool Demo_add_one::write(yarp::os::ConnectionWriter& connection) {
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

void Demo_add_one::init(const int32_t x) {
  _return = 0;
  this->x = x;
}

bool Demo_start::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(1)) return false;
  if (!writer.writeTag("start",1,1)) return false;
  return true;
}

bool Demo_start::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.readBool(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void Demo_start::init() {
  _return = false;
}

bool Demo_stop::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(1)) return false;
  if (!writer.writeTag("stop",1,1)) return false;
  return true;
}

bool Demo_stop::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.readBool(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void Demo_stop::init() {
  _return = false;
}

bool Demo_is_running::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(2)) return false;
  if (!writer.writeTag("is_running",1,2)) return false;
  return true;
}

bool Demo_is_running::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.readBool(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void Demo_is_running::init() {
  _return = false;
}

bool Demo_test_void::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(3)) return false;
  if (!writer.writeTag("test_void",1,2)) return false;
  if (!writer.writeI32(x)) return false;
  return true;
}

bool Demo_test_void::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  return true;
}

void Demo_test_void::init(const int32_t x) {
  this->x = x;
}

bool Demo_test_1way::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(3)) return false;
  if (!writer.writeTag("test_1way",1,2)) return false;
  if (!writer.writeI32(x)) return false;
  return true;
}

bool Demo_test_1way::read(yarp::os::ConnectionReader& connection) {
  YARP_UNUSED(connection);
  return true;
}

void Demo_test_1way::init(const int32_t x) {
  this->x = x;
}

bool Demo_test_enums::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(3)) return false;
  if (!writer.writeTag("test_enums",1,2)) return false;
  if (!writer.writeI32((int32_t)x)) return false;
  return true;
}

bool Demo_test_enums::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  int32_t ecast36;
  DemoEnumVocab cvrt37;
  if (!reader.readEnum(ecast36,cvrt37)) {
    reader.fail();
    return false;
  } else {
    _return = (DemoEnum)ecast36;
  }
  return true;
}

void Demo_test_enums::init(const DemoEnum x) {
  _return = (DemoEnum)0;
  this->x = x;
}

bool Demo_test_enum_vector::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(4)) return false;
  if (!writer.writeTag("test_enum_vector",1,3)) return false;
  {
    if (!writer.writeListBegin(BOTTLE_TAG_INT, static_cast<uint32_t>(x.size()))) return false;
    std::vector<DemoEnum> ::iterator _iter38;
    for (_iter38 = x.begin(); _iter38 != x.end(); ++_iter38)
    {
      if (!writer.writeI32((int32_t)(*_iter38))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}

bool Demo_test_enum_vector::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  {
    _return.clear();
    uint32_t _size39;
    yarp::os::idl::WireState _etype42;
    reader.readListBegin(_etype42, _size39);
    _return.resize(_size39);
    uint32_t _i43;
    for (_i43 = 0; _i43 < _size39; ++_i43)
    {
      int32_t ecast44;
      DemoEnumVocab cvrt45;
      if (!reader.readEnum(ecast44,cvrt45)) {
        reader.fail();
        return false;
      } else {
        _return[_i43] = (DemoEnum)ecast44;
      }
    }
    reader.readListEnd();
  }
  return true;
}

void Demo_test_enum_vector::init(const std::vector<DemoEnum> & x) {
  _return;
  this->x = x;
}

bool Demo_test_defaults::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(3)) return false;
  if (!writer.writeTag("test_defaults",1,2)) return false;
  if (!writer.writeI32(x)) return false;
  return true;
}

bool Demo_test_defaults::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.readBool(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void Demo_test_defaults::init(const int32_t x) {
  _return = false;
  this->x = x;
}

bool Demo_add_pair::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(4)) return false;
  if (!writer.writeTag("add_pair",1,2)) return false;
  if (!writer.writeI32(x)) return false;
  if (!writer.writeI32(y)) return false;
  return true;
}

bool Demo_add_pair::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.readI32(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void Demo_add_pair::init(const int32_t x, const int32_t y) {
  _return = 0;
  this->x = x;
  this->y = y;
}

bool Demo_test_partial::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(5)) return false;
  if (!writer.writeTag("test_partial",1,2)) return false;
  if (!writer.writeI32(x)) return false;
  {
    if (!writer.writeListBegin(BOTTLE_TAG_INT, static_cast<uint32_t>(lst.size()))) return false;
    std::vector<int32_t> ::iterator _iter46;
    for (_iter46 = lst.begin(); _iter46 != lst.end(); ++_iter46)
    {
      if (!writer.writeI32((*_iter46))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  if (!writer.writeI32(y)) return false;
  return true;
}

bool Demo_test_partial::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.readI32(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void Demo_test_partial::init(const int32_t x, const std::vector<int32_t> & lst, const int32_t y) {
  _return = 0;
  this->x = x;
  this->lst = lst;
  this->y = y;
}

bool Demo_test_tail_defaults::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(4)) return false;
  if (!writer.writeTag("test_tail_defaults",1,3)) return false;
  if (!writer.writeI32((int32_t)x)) return false;
  return true;
}

bool Demo_test_tail_defaults::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.readI32(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void Demo_test_tail_defaults::init(const DemoEnum x) {
  _return = 0;
  this->x = x;
}

bool Demo_test_longer_tail_defaults::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(8)) return false;
  if (!writer.writeTag("test_longer_tail_defaults",1,4)) return false;
  if (!writer.writeI32(ignore)) return false;
  if (!writer.writeI32((int32_t)_enum)) return false;
  if (!writer.writeI32(_int)) return false;
  if (!writer.writeString(_string)) return false;
  return true;
}

bool Demo_test_longer_tail_defaults::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.readI32(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void Demo_test_longer_tail_defaults::init(const int32_t ignore, const DemoEnum _enum, const int32_t _int, const std::string& _string) {
  _return = 0;
  this->ignore = ignore;
  this->_enum = _enum;
  this->_int = _int;
  this->_string = _string;
}

bool Demo_do_start_a_service::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(4)) return false;
  if (!writer.writeTag("do_start_a_service",1,4)) return false;
  return true;
}

bool Demo_do_start_a_service::read(yarp::os::ConnectionReader& connection) {
  YARP_UNUSED(connection);
  return true;
}

void Demo_do_start_a_service::init() {
}

bool Demo_do_check_for_service::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(4)) return false;
  if (!writer.writeTag("do_check_for_service",1,4)) return false;
  return true;
}

bool Demo_do_check_for_service::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  if (!reader.readBool(_return)) {
    reader.fail();
    return false;
  }
  return true;
}

void Demo_do_check_for_service::init() {
  _return = false;
}

bool Demo_do_stop_a_service::write(yarp::os::ConnectionWriter& connection) {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(4)) return false;
  if (!writer.writeTag("do_stop_a_service",1,4)) return false;
  return true;
}

bool Demo_do_stop_a_service::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListReturn()) return false;
  return true;
}

void Demo_do_stop_a_service::init() {
}

Demo::Demo() {
  yarp().setOwner(*this);
}
int32_t Demo::get_answer() {
  int32_t _return = 0;
  Demo_get_answer helper;
  helper.init();
  if (!yarp().canWrite()) {
    fprintf(stderr,"Missing server method '%s'?\n","int32_t Demo::get_answer()");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
bool Demo::set_answer(const int32_t rightAnswer) {
  bool _return = false;
  Demo_set_answer helper;
  helper.init(rightAnswer);
  if (!yarp().canWrite()) {
    fprintf(stderr,"Missing server method '%s'?\n","bool Demo::set_answer(const int32_t rightAnswer)");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
int32_t Demo::add_one(const int32_t x) {
  int32_t _return = 0;
  Demo_add_one helper;
  helper.init(x);
  if (!yarp().canWrite()) {
    fprintf(stderr,"Missing server method '%s'?\n","int32_t Demo::add_one(const int32_t x)");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
bool Demo::start() {
  bool _return = false;
  Demo_start helper;
  helper.init();
  if (!yarp().canWrite()) {
    fprintf(stderr,"Missing server method '%s'?\n","bool Demo::start()");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
bool Demo::stop() {
  bool _return = false;
  Demo_stop helper;
  helper.init();
  if (!yarp().canWrite()) {
    fprintf(stderr,"Missing server method '%s'?\n","bool Demo::stop()");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
bool Demo::is_running() {
  bool _return = false;
  Demo_is_running helper;
  helper.init();
  if (!yarp().canWrite()) {
    fprintf(stderr,"Missing server method '%s'?\n","bool Demo::is_running()");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
void Demo::test_void(const int32_t x) {
  Demo_test_void helper;
  helper.init(x);
  if (!yarp().canWrite()) {
    fprintf(stderr,"Missing server method '%s'?\n","void Demo::test_void(const int32_t x)");
  }
  yarp().write(helper,helper);
}
void Demo::test_1way(const int32_t x) {
  Demo_test_1way helper;
  helper.init(x);
  if (!yarp().canWrite()) {
    fprintf(stderr,"Missing server method '%s'?\n","void Demo::test_1way(const int32_t x)");
  }
  yarp().write(helper);
}
DemoEnum Demo::test_enums(const DemoEnum x) {
  DemoEnum _return = (DemoEnum)0;
  Demo_test_enums helper;
  helper.init(x);
  if (!yarp().canWrite()) {
    fprintf(stderr,"Missing server method '%s'?\n","DemoEnum Demo::test_enums(const DemoEnum x)");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
std::vector<DemoEnum>  Demo::test_enum_vector(const std::vector<DemoEnum> & x) {
  std::vector<DemoEnum>  _return;
  Demo_test_enum_vector helper;
  helper.init(x);
  if (!yarp().canWrite()) {
    fprintf(stderr,"Missing server method '%s'?\n","std::vector<DemoEnum>  Demo::test_enum_vector(const std::vector<DemoEnum> & x)");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
bool Demo::test_defaults(const int32_t x) {
  bool _return = false;
  Demo_test_defaults helper;
  helper.init(x);
  if (!yarp().canWrite()) {
    fprintf(stderr,"Missing server method '%s'?\n","bool Demo::test_defaults(const int32_t x)");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
int32_t Demo::add_pair(const int32_t x, const int32_t y) {
  int32_t _return = 0;
  Demo_add_pair helper;
  helper.init(x,y);
  if (!yarp().canWrite()) {
    fprintf(stderr,"Missing server method '%s'?\n","int32_t Demo::add_pair(const int32_t x, const int32_t y)");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
int32_t Demo::test_partial(const int32_t x, const std::vector<int32_t> & lst, const int32_t y) {
  int32_t _return = 0;
  Demo_test_partial helper;
  helper.init(x,lst,y);
  if (!yarp().canWrite()) {
    fprintf(stderr,"Missing server method '%s'?\n","int32_t Demo::test_partial(const int32_t x, const std::vector<int32_t> & lst, const int32_t y)");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
int32_t Demo::test_tail_defaults(const DemoEnum x) {
  int32_t _return = 0;
  Demo_test_tail_defaults helper;
  helper.init(x);
  if (!yarp().canWrite()) {
    fprintf(stderr,"Missing server method '%s'?\n","int32_t Demo::test_tail_defaults(const DemoEnum x)");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
int32_t Demo::test_longer_tail_defaults(const int32_t ignore, const DemoEnum _enum, const int32_t _int, const std::string& _string) {
  int32_t _return = 0;
  Demo_test_longer_tail_defaults helper;
  helper.init(ignore,_enum,_int,_string);
  if (!yarp().canWrite()) {
    fprintf(stderr,"Missing server method '%s'?\n","int32_t Demo::test_longer_tail_defaults(const int32_t ignore, const DemoEnum _enum, const int32_t _int, const std::string& _string)");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
void Demo::do_start_a_service() {
  Demo_do_start_a_service helper;
  helper.init();
  if (!yarp().canWrite()) {
    fprintf(stderr,"Missing server method '%s'?\n","void Demo::do_start_a_service()");
  }
  yarp().write(helper);
}
bool Demo::do_check_for_service() {
  bool _return = false;
  Demo_do_check_for_service helper;
  helper.init();
  if (!yarp().canWrite()) {
    fprintf(stderr,"Missing server method '%s'?\n","bool Demo::do_check_for_service()");
  }
  bool ok = yarp().write(helper,helper);
  return ok?helper._return:_return;
}
void Demo::do_stop_a_service() {
  Demo_do_stop_a_service helper;
  helper.init();
  if (!yarp().canWrite()) {
    fprintf(stderr,"Missing server method '%s'?\n","void Demo::do_stop_a_service()");
  }
  yarp().write(helper,helper);
}

bool Demo::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  reader.expectAccept();
  if (!reader.readListHeader()) { reader.fail(); return false; }
  yarp::os::ConstString tag = reader.readTag();
  bool direct = (tag=="__direct__");
  if (direct) tag = reader.readTag();
  while (!reader.isError()) {
    // TODO: use quick lookup, this is just a test
    if (tag == "get_answer") {
      int32_t _return;
      _return = get_answer();
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeI32(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "set_answer") {
      int32_t rightAnswer;
      if (!reader.readI32(rightAnswer)) {
        reader.fail();
        return false;
      }
      bool _return;
      _return = set_answer(rightAnswer);
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeBool(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "add_one") {
      int32_t x;
      if (!reader.readI32(x)) {
        reader.fail();
        return false;
      }
      int32_t _return;
      _return = add_one(x);
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeI32(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "start") {
      bool _return;
      _return = start();
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
    if (tag == "is_running") {
      bool _return;
      _return = is_running();
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeBool(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "test_void") {
      int32_t x;
      if (!reader.readI32(x)) {
        reader.fail();
        return false;
      }
      test_void(x);
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(0)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "test_1way") {
      int32_t x;
      if (!reader.readI32(x)) {
        reader.fail();
        return false;
      }
      if (!direct) {
        Demo_test_1way helper;
        helper.init(x);
        yarp().callback(helper,*this,"__direct__");
      } else {
        test_1way(x);
      }
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeOnewayResponse()) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "test_enums") {
      DemoEnum x;
      int32_t ecast47;
      DemoEnumVocab cvrt48;
      if (!reader.readEnum(ecast47,cvrt48)) {
        reader.fail();
        return false;
      } else {
        x = (DemoEnum)ecast47;
      }
      DemoEnum _return;
      _return = test_enums(x);
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeI32((int32_t)_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "test_enum_vector") {
      std::vector<DemoEnum>  x;
      {
        x.clear();
        uint32_t _size49;
        yarp::os::idl::WireState _etype52;
        reader.readListBegin(_etype52, _size49);
        x.resize(_size49);
        uint32_t _i53;
        for (_i53 = 0; _i53 < _size49; ++_i53)
        {
          int32_t ecast54;
          DemoEnumVocab cvrt55;
          if (!reader.readEnum(ecast54,cvrt55)) {
            reader.fail();
            return false;
          } else {
            x[_i53] = (DemoEnum)ecast54;
          }
        }
        reader.readListEnd();
      }
      std::vector<DemoEnum>  _return;
      _return = test_enum_vector(x);
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        {
          if (!writer.writeListBegin(BOTTLE_TAG_INT, static_cast<uint32_t>(_return.size()))) return false;
          std::vector<DemoEnum> ::iterator _iter56;
          for (_iter56 = _return.begin(); _iter56 != _return.end(); ++_iter56)
          {
            if (!writer.writeI32((int32_t)(*_iter56))) return false;
          }
          if (!writer.writeListEnd()) return false;
        }
      }
      reader.accept();
      return true;
    }
    if (tag == "test_defaults") {
      int32_t x;
      if (!reader.readI32(x)) {
        x = 42;
      }
      bool _return;
      _return = test_defaults(x);
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeBool(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "add_pair") {
      int32_t x;
      int32_t y;
      if (!reader.readI32(x)) {
        reader.fail();
        return false;
      }
      if (!reader.readI32(y)) {
        reader.fail();
        return false;
      }
      int32_t _return;
      _return = add_pair(x,y);
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeI32(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "test_partial") {
      int32_t x;
      std::vector<int32_t>  lst;
      int32_t y;
      if (!reader.readI32(x)) {
        reader.fail();
        return false;
      }
      {
        lst.clear();
        uint32_t _size57;
        yarp::os::idl::WireState _etype60;
        reader.readListBegin(_etype60, _size57);
        lst.resize(_size57);
        uint32_t _i61;
        for (_i61 = 0; _i61 < _size57; ++_i61)
        {
          if (!reader.readI32(lst[_i61])) {
            reader.fail();
            return false;
          }
        }
        reader.readListEnd();
      }
      if (!reader.readI32(y)) {
        reader.fail();
        return false;
      }
      int32_t _return;
      _return = test_partial(x,lst,y);
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeI32(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "test_tail_defaults") {
      DemoEnum x;
      int32_t ecast62;
      DemoEnumVocab cvrt63;
      if (!reader.readEnum(ecast62,cvrt63)) {
        x = ENUM1;
      } else {
        x = (DemoEnum)ecast62;
      }
      int32_t _return;
      _return = test_tail_defaults(x);
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeI32(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "test_longer_tail_defaults") {
      int32_t ignore;
      DemoEnum _enum;
      int32_t _int;
      std::string _string;
      if (!reader.readI32(ignore)) {
        reader.fail();
        return false;
      }
      int32_t ecast64;
      DemoEnumVocab cvrt65;
      if (!reader.readEnum(ecast64,cvrt65)) {
        _enum = ENUM2;
      } else {
        _enum = (DemoEnum)ecast64;
      }
      if (!reader.readI32(_int)) {
        _int = 42;
      }
      if (!reader.readString(_string)) {
        _string = "Space Monkey from the Planet: Space";
      }
      int32_t _return;
      _return = test_longer_tail_defaults(ignore,_enum,_int,_string);
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeI32(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "do_start_a_service") {
      if (!direct) {
        Demo_do_start_a_service helper;
        helper.init();
        yarp().callback(helper,*this,"__direct__");
      } else {
        do_start_a_service();
      }
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeOnewayResponse()) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "do_check_for_service") {
      bool _return;
      _return = do_check_for_service();
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeBool(_return)) return false;
      }
      reader.accept();
      return true;
    }
    if (tag == "do_stop_a_service") {
      do_stop_a_service();
      yarp::os::idl::WireWriter writer(reader);
      if (!writer.isNull()) {
        if (!writer.writeListHeader(0)) return false;
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

std::vector<std::string> Demo::help(const std::string& functionName) {
  bool showAll=(functionName=="--all");
  std::vector<std::string> helpString;
  if(showAll) {
    helpString.push_back("*** Available commands:");
    helpString.push_back("get_answer");
    helpString.push_back("set_answer");
    helpString.push_back("add_one");
    helpString.push_back("start");
    helpString.push_back("stop");
    helpString.push_back("is_running");
    helpString.push_back("test_void");
    helpString.push_back("test_1way");
    helpString.push_back("test_enums");
    helpString.push_back("test_enum_vector");
    helpString.push_back("test_defaults");
    helpString.push_back("add_pair");
    helpString.push_back("test_partial");
    helpString.push_back("test_tail_defaults");
    helpString.push_back("test_longer_tail_defaults");
    helpString.push_back("do_start_a_service");
    helpString.push_back("do_check_for_service");
    helpString.push_back("do_stop_a_service");
    helpString.push_back("help");
  }
  else {
    if (functionName=="get_answer") {
      helpString.push_back("int32_t get_answer() ");
      helpString.push_back("This function gets the answer ");
    }
    if (functionName=="set_answer") {
      helpString.push_back("bool set_answer(const int32_t rightAnswer) ");
    }
    if (functionName=="add_one") {
      helpString.push_back("int32_t add_one(const int32_t x) ");
    }
    if (functionName=="start") {
      helpString.push_back("bool start() ");
    }
    if (functionName=="stop") {
      helpString.push_back("bool stop() ");
    }
    if (functionName=="is_running") {
      helpString.push_back("bool is_running() ");
    }
    if (functionName=="test_void") {
      helpString.push_back("void test_void(const int32_t x) ");
    }
    if (functionName=="test_1way") {
      helpString.push_back("void test_1way(const int32_t x) ");
    }
    if (functionName=="test_enums") {
      helpString.push_back("DemoEnum test_enums(const DemoEnum x) ");
    }
    if (functionName=="test_enum_vector") {
      helpString.push_back("std::vector<DemoEnum>  test_enum_vector(const std::vector<DemoEnum> & x) ");
    }
    if (functionName=="test_defaults") {
      helpString.push_back("bool test_defaults(const int32_t x = 42) ");
    }
    if (functionName=="add_pair") {
      helpString.push_back("int32_t add_pair(const int32_t x, const int32_t y) ");
    }
    if (functionName=="test_partial") {
      helpString.push_back("int32_t test_partial(const int32_t x, const std::vector<int32_t> & lst, const int32_t y) ");
    }
    if (functionName=="test_tail_defaults") {
      helpString.push_back("int32_t test_tail_defaults(const DemoEnum x = ENUM1) ");
    }
    if (functionName=="test_longer_tail_defaults") {
      helpString.push_back("int32_t test_longer_tail_defaults(const int32_t ignore, const DemoEnum _enum = ENUM2, const int32_t _int = 42, const std::string& _string = \"Space Monkey from the Planet: Space\") ");
      helpString.push_back("Test longer tail defaults ");
    }
    if (functionName=="do_start_a_service") {
      helpString.push_back("void do_start_a_service() ");
    }
    if (functionName=="do_check_for_service") {
      helpString.push_back("bool do_check_for_service() ");
    }
    if (functionName=="do_stop_a_service") {
      helpString.push_back("void do_stop_a_service() ");
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



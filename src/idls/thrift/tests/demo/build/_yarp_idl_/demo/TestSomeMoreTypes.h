// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_STRUCT_TestSomeMoreTypes
#define YARP_THRIFT_GENERATOR_STRUCT_TestSomeMoreTypes

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>

class TestSomeMoreTypes;


class TestSomeMoreTypes : public yarp::os::idl::WirePortable {
public:
  // Fields
  bool a_bool;
  int8_t a_byte;
  int16_t a_i16;
  int32_t a_i32;
  int64_t a_i64;
  double a_double;
  std::string a_string;
  std::string a_binary;

  // Default constructor
  TestSomeMoreTypes() : a_bool(0), a_byte(0), a_i16(0), a_i32(0), a_i64(0), a_double(0), a_string(""), a_binary("") {
  }

  // Constructor with field values
  TestSomeMoreTypes(const bool a_bool,const int8_t a_byte,const int16_t a_i16,const int32_t a_i32,const int64_t a_i64,const double a_double,const std::string& a_string,const std::string& a_binary) : a_bool(a_bool), a_byte(a_byte), a_i16(a_i16), a_i32(a_i32), a_i64(a_i64), a_double(a_double), a_string(a_string), a_binary(a_binary) {
  }

  // Copy constructor
  TestSomeMoreTypes(const TestSomeMoreTypes& __alt) : WirePortable(__alt)  {
    this->a_bool = __alt.a_bool;
    this->a_byte = __alt.a_byte;
    this->a_i16 = __alt.a_i16;
    this->a_i32 = __alt.a_i32;
    this->a_i64 = __alt.a_i64;
    this->a_double = __alt.a_double;
    this->a_string = __alt.a_string;
    this->a_binary = __alt.a_binary;
  }

  // Assignment operator
  const TestSomeMoreTypes& operator = (const TestSomeMoreTypes& __alt) {
    this->a_bool = __alt.a_bool;
    this->a_byte = __alt.a_byte;
    this->a_i16 = __alt.a_i16;
    this->a_i32 = __alt.a_i32;
    this->a_i64 = __alt.a_i64;
    this->a_double = __alt.a_double;
    this->a_string = __alt.a_string;
    this->a_binary = __alt.a_binary;
    return *this;
  }

  // read and write structure on a connection
  bool read(yarp::os::idl::WireReader& reader);
  bool read(yarp::os::ConnectionReader& connection);
  bool write(yarp::os::idl::WireWriter& writer);
  bool write(yarp::os::ConnectionWriter& connection);

private:
  bool write_a_bool(yarp::os::idl::WireWriter& writer);
  bool nested_write_a_bool(yarp::os::idl::WireWriter& writer);
  bool write_a_byte(yarp::os::idl::WireWriter& writer);
  bool nested_write_a_byte(yarp::os::idl::WireWriter& writer);
  bool write_a_i16(yarp::os::idl::WireWriter& writer);
  bool nested_write_a_i16(yarp::os::idl::WireWriter& writer);
  bool write_a_i32(yarp::os::idl::WireWriter& writer);
  bool nested_write_a_i32(yarp::os::idl::WireWriter& writer);
  bool write_a_i64(yarp::os::idl::WireWriter& writer);
  bool nested_write_a_i64(yarp::os::idl::WireWriter& writer);
  bool write_a_double(yarp::os::idl::WireWriter& writer);
  bool nested_write_a_double(yarp::os::idl::WireWriter& writer);
  bool write_a_string(yarp::os::idl::WireWriter& writer);
  bool nested_write_a_string(yarp::os::idl::WireWriter& writer);
  bool write_a_binary(yarp::os::idl::WireWriter& writer);
  bool nested_write_a_binary(yarp::os::idl::WireWriter& writer);
  bool read_a_bool(yarp::os::idl::WireReader& reader);
  bool nested_read_a_bool(yarp::os::idl::WireReader& reader);
  bool read_a_byte(yarp::os::idl::WireReader& reader);
  bool nested_read_a_byte(yarp::os::idl::WireReader& reader);
  bool read_a_i16(yarp::os::idl::WireReader& reader);
  bool nested_read_a_i16(yarp::os::idl::WireReader& reader);
  bool read_a_i32(yarp::os::idl::WireReader& reader);
  bool nested_read_a_i32(yarp::os::idl::WireReader& reader);
  bool read_a_i64(yarp::os::idl::WireReader& reader);
  bool nested_read_a_i64(yarp::os::idl::WireReader& reader);
  bool read_a_double(yarp::os::idl::WireReader& reader);
  bool nested_read_a_double(yarp::os::idl::WireReader& reader);
  bool read_a_string(yarp::os::idl::WireReader& reader);
  bool nested_read_a_string(yarp::os::idl::WireReader& reader);
  bool read_a_binary(yarp::os::idl::WireReader& reader);
  bool nested_read_a_binary(yarp::os::idl::WireReader& reader);

public:

  yarp::os::ConstString toString();

  // if you want to serialize this class without nesting, use this helper
  typedef yarp::os::idl::Unwrapped<TestSomeMoreTypes > unwrapped;

  class Editor : public yarp::os::Wire, public yarp::os::PortWriter {
  public:

    Editor() {
      group = 0;
      obj_owned = true;
      obj = new TestSomeMoreTypes;
      dirty_flags(false);
      yarp().setOwner(*this);
    }

    Editor(TestSomeMoreTypes& obj) {
      group = 0;
      obj_owned = false;
      edit(obj,false);
      yarp().setOwner(*this);
    }

    bool edit(TestSomeMoreTypes& obj, bool dirty = true) {
      if (obj_owned) delete this->obj;
      this->obj = &obj;
      obj_owned = false;
      dirty_flags(dirty);
      return true;
    }

    virtual ~Editor() {
    if (obj_owned) delete obj;
    }

    bool isValid() const {
      return obj!=0/*NULL*/;
    }

    TestSomeMoreTypes& state() { return *obj; }

    void begin() { group++; }

    void end() {
      group--;
      if (group==0&&is_dirty) communicate();
    }
    void set_a_bool(const bool a_bool) {
      will_set_a_bool();
      obj->a_bool = a_bool;
      mark_dirty_a_bool();
      communicate();
      did_set_a_bool();
    }
    void set_a_byte(const int8_t a_byte) {
      will_set_a_byte();
      obj->a_byte = a_byte;
      mark_dirty_a_byte();
      communicate();
      did_set_a_byte();
    }
    void set_a_i16(const int16_t a_i16) {
      will_set_a_i16();
      obj->a_i16 = a_i16;
      mark_dirty_a_i16();
      communicate();
      did_set_a_i16();
    }
    void set_a_i32(const int32_t a_i32) {
      will_set_a_i32();
      obj->a_i32 = a_i32;
      mark_dirty_a_i32();
      communicate();
      did_set_a_i32();
    }
    void set_a_i64(const int64_t a_i64) {
      will_set_a_i64();
      obj->a_i64 = a_i64;
      mark_dirty_a_i64();
      communicate();
      did_set_a_i64();
    }
    void set_a_double(const double a_double) {
      will_set_a_double();
      obj->a_double = a_double;
      mark_dirty_a_double();
      communicate();
      did_set_a_double();
    }
    void set_a_string(const std::string& a_string) {
      will_set_a_string();
      obj->a_string = a_string;
      mark_dirty_a_string();
      communicate();
      did_set_a_string();
    }
    void set_a_binary(const std::string& a_binary) {
      will_set_a_binary();
      obj->a_binary = a_binary;
      mark_dirty_a_binary();
      communicate();
      did_set_a_binary();
    }
    bool get_a_bool() {
      return obj->a_bool;
    }
    int8_t get_a_byte() {
      return obj->a_byte;
    }
    int16_t get_a_i16() {
      return obj->a_i16;
    }
    int32_t get_a_i32() {
      return obj->a_i32;
    }
    int64_t get_a_i64() {
      return obj->a_i64;
    }
    double get_a_double() {
      return obj->a_double;
    }
    const std::string& get_a_string() {
      return obj->a_string;
    }
    const std::string& get_a_binary() {
      return obj->a_binary;
    }
    virtual bool will_set_a_bool() { return true; }
    virtual bool will_set_a_byte() { return true; }
    virtual bool will_set_a_i16() { return true; }
    virtual bool will_set_a_i32() { return true; }
    virtual bool will_set_a_i64() { return true; }
    virtual bool will_set_a_double() { return true; }
    virtual bool will_set_a_string() { return true; }
    virtual bool will_set_a_binary() { return true; }
    virtual bool did_set_a_bool() { return true; }
    virtual bool did_set_a_byte() { return true; }
    virtual bool did_set_a_i16() { return true; }
    virtual bool did_set_a_i32() { return true; }
    virtual bool did_set_a_i64() { return true; }
    virtual bool did_set_a_double() { return true; }
    virtual bool did_set_a_string() { return true; }
    virtual bool did_set_a_binary() { return true; }
    void clean() {
      dirty_flags(false);
    }
    bool read(yarp::os::ConnectionReader& connection);
    bool write(yarp::os::ConnectionWriter& connection);
  private:

    TestSomeMoreTypes *obj;

    bool obj_owned;
    int group;

    void communicate() {
      if (group!=0) return;
      if (yarp().canWrite()) {
        yarp().write(*this);
        clean();
      }
    }
    void mark_dirty() {
      is_dirty = true;
    }
    void mark_dirty_a_bool() {
      if (is_dirty_a_bool) return;
      dirty_count++;
      is_dirty_a_bool = true;
      mark_dirty();
    }
    void mark_dirty_a_byte() {
      if (is_dirty_a_byte) return;
      dirty_count++;
      is_dirty_a_byte = true;
      mark_dirty();
    }
    void mark_dirty_a_i16() {
      if (is_dirty_a_i16) return;
      dirty_count++;
      is_dirty_a_i16 = true;
      mark_dirty();
    }
    void mark_dirty_a_i32() {
      if (is_dirty_a_i32) return;
      dirty_count++;
      is_dirty_a_i32 = true;
      mark_dirty();
    }
    void mark_dirty_a_i64() {
      if (is_dirty_a_i64) return;
      dirty_count++;
      is_dirty_a_i64 = true;
      mark_dirty();
    }
    void mark_dirty_a_double() {
      if (is_dirty_a_double) return;
      dirty_count++;
      is_dirty_a_double = true;
      mark_dirty();
    }
    void mark_dirty_a_string() {
      if (is_dirty_a_string) return;
      dirty_count++;
      is_dirty_a_string = true;
      mark_dirty();
    }
    void mark_dirty_a_binary() {
      if (is_dirty_a_binary) return;
      dirty_count++;
      is_dirty_a_binary = true;
      mark_dirty();
    }
    void dirty_flags(bool flag) {
      is_dirty = flag;
      is_dirty_a_bool = flag;
      is_dirty_a_byte = flag;
      is_dirty_a_i16 = flag;
      is_dirty_a_i32 = flag;
      is_dirty_a_i64 = flag;
      is_dirty_a_double = flag;
      is_dirty_a_string = flag;
      is_dirty_a_binary = flag;
      dirty_count = flag ? 8 : 0;
    }
    bool is_dirty;
    int dirty_count;
    bool is_dirty_a_bool;
    bool is_dirty_a_byte;
    bool is_dirty_a_i16;
    bool is_dirty_a_i32;
    bool is_dirty_a_i64;
    bool is_dirty_a_double;
    bool is_dirty_a_string;
    bool is_dirty_a_binary;
  };
};

#endif


// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_STRUCT_DemoStruct
#define YARP_THRIFT_GENERATOR_STRUCT_DemoStruct

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>

class DemoStruct;


/**
 * Documentation for structure
 */
class DemoStruct : public yarp::os::idl::WirePortable {
public:
  // Fields
  /**
   * this is the x part
   */
  int32_t x;
  /**
   * this is the y part
   */
  int32_t y;

  // Default constructor
  DemoStruct() : x(0), y(0) {
  }

  // Constructor with field values
  DemoStruct(const int32_t x,const int32_t y) : x(x), y(y) {
  }

  // Copy constructor
  DemoStruct(const DemoStruct& __alt) : WirePortable(__alt)  {
    this->x = __alt.x;
    this->y = __alt.y;
  }

  // Assignment operator
  const DemoStruct& operator = (const DemoStruct& __alt) {
    this->x = __alt.x;
    this->y = __alt.y;
    return *this;
  }

  // read and write structure on a connection
  bool read(yarp::os::idl::WireReader& reader);
  bool read(yarp::os::ConnectionReader& connection);
  bool write(yarp::os::idl::WireWriter& writer);
  bool write(yarp::os::ConnectionWriter& connection);

private:
  bool write_x(yarp::os::idl::WireWriter& writer);
  bool nested_write_x(yarp::os::idl::WireWriter& writer);
  bool write_y(yarp::os::idl::WireWriter& writer);
  bool nested_write_y(yarp::os::idl::WireWriter& writer);
  bool read_x(yarp::os::idl::WireReader& reader);
  bool nested_read_x(yarp::os::idl::WireReader& reader);
  bool read_y(yarp::os::idl::WireReader& reader);
  bool nested_read_y(yarp::os::idl::WireReader& reader);

public:

  yarp::os::ConstString toString();

  // if you want to serialize this class without nesting, use this helper
  typedef yarp::os::idl::Unwrapped<DemoStruct > unwrapped;

  class Editor : public yarp::os::Wire, public yarp::os::PortWriter {
  public:

    Editor() {
      group = 0;
      obj_owned = true;
      obj = new DemoStruct;
      dirty_flags(false);
      yarp().setOwner(*this);
    }

    Editor(DemoStruct& obj) {
      group = 0;
      obj_owned = false;
      edit(obj,false);
      yarp().setOwner(*this);
    }

    bool edit(DemoStruct& obj, bool dirty = true) {
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

    DemoStruct& state() { return *obj; }

    void begin() { group++; }

    void end() {
      group--;
      if (group==0&&is_dirty) communicate();
    }
    void set_x(const int32_t x) {
      will_set_x();
      obj->x = x;
      mark_dirty_x();
      communicate();
      did_set_x();
    }
    void set_y(const int32_t y) {
      will_set_y();
      obj->y = y;
      mark_dirty_y();
      communicate();
      did_set_y();
    }
    int32_t get_x() {
      return obj->x;
    }
    int32_t get_y() {
      return obj->y;
    }
    virtual bool will_set_x() { return true; }
    virtual bool will_set_y() { return true; }
    virtual bool did_set_x() { return true; }
    virtual bool did_set_y() { return true; }
    void clean() {
      dirty_flags(false);
    }
    bool read(yarp::os::ConnectionReader& connection);
    bool write(yarp::os::ConnectionWriter& connection);
  private:

    DemoStruct *obj;

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
    void mark_dirty_x() {
      if (is_dirty_x) return;
      dirty_count++;
      is_dirty_x = true;
      mark_dirty();
    }
    void mark_dirty_y() {
      if (is_dirty_y) return;
      dirty_count++;
      is_dirty_y = true;
      mark_dirty();
    }
    void dirty_flags(bool flag) {
      is_dirty = flag;
      is_dirty_x = flag;
      is_dirty_y = flag;
      dirty_count = flag ? 2 : 0;
    }
    bool is_dirty;
    int dirty_count;
    bool is_dirty_x;
    bool is_dirty_y;
  };
};

#endif


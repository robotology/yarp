// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_STRUCT_RGBA
#define YARP_THRIFT_GENERATOR_STRUCT_RGBA

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>

class RGBA;


class RGBA : public yarp::os::idl::WirePortable {
public:
  // Fields
  int32_t rgba;

  // Default constructor
  RGBA() : rgba(0) {
  }

  // Constructor with field values
  RGBA(const int32_t rgba) : rgba(rgba) {
  }

  // Copy constructor
  RGBA(const RGBA& __alt) : WirePortable(__alt)  {
    this->rgba = __alt.rgba;
  }

  // Assignment operator
  const RGBA& operator = (const RGBA& __alt) {
    this->rgba = __alt.rgba;
    return *this;
  }

  // read and write structure on a connection
  bool read(yarp::os::idl::WireReader& reader);
  bool read(yarp::os::ConnectionReader& connection);
  bool write(yarp::os::idl::WireWriter& writer);
  bool write(yarp::os::ConnectionWriter& connection);

private:
  bool write_rgba(yarp::os::idl::WireWriter& writer);
  bool nested_write_rgba(yarp::os::idl::WireWriter& writer);
  bool read_rgba(yarp::os::idl::WireReader& reader);
  bool nested_read_rgba(yarp::os::idl::WireReader& reader);

public:

  yarp::os::ConstString toString();

  // if you want to serialize this class without nesting, use this helper
  typedef yarp::os::idl::Unwrapped<RGBA > unwrapped;

  class Editor : public yarp::os::Wire, public yarp::os::PortWriter {
  public:

    Editor() {
      group = 0;
      obj_owned = true;
      obj = new RGBA;
      dirty_flags(false);
      yarp().setOwner(*this);
    }

    Editor(RGBA& obj) {
      group = 0;
      obj_owned = false;
      edit(obj,false);
      yarp().setOwner(*this);
    }

    bool edit(RGBA& obj, bool dirty = true) {
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

    RGBA& state() { return *obj; }

    void begin() { group++; }

    void end() {
      group--;
      if (group==0&&is_dirty) communicate();
    }
    void set_rgba(const int32_t rgba) {
      will_set_rgba();
      obj->rgba = rgba;
      mark_dirty_rgba();
      communicate();
      did_set_rgba();
    }
    int32_t get_rgba() {
      return obj->rgba;
    }
    virtual bool will_set_rgba() { return true; }
    virtual bool did_set_rgba() { return true; }
    void clean() {
      dirty_flags(false);
    }
    bool read(yarp::os::ConnectionReader& connection);
    bool write(yarp::os::ConnectionWriter& connection);
  private:

    RGBA *obj;

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
    void mark_dirty_rgba() {
      if (is_dirty_rgba) return;
      dirty_count++;
      is_dirty_rgba = true;
      mark_dirty();
    }
    void dirty_flags(bool flag) {
      is_dirty = flag;
      is_dirty_rgba = flag;
      dirty_count = flag ? 1 : 0;
    }
    bool is_dirty;
    int dirty_count;
    bool is_dirty_rgba;
  };
};

#endif


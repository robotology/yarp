// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_STRUCT_DemoStructExt
#define YARP_THRIFT_GENERATOR_STRUCT_DemoStructExt

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <DemoStruct.h>

class DemoStructExt;


class DemoStructExt : public yarp::os::idl::WirePortable {
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
  /**
   * this is a list of ints
   */
  std::vector<int32_t>  int_list;
  /**
   * this is a list of structs
   */
  std::vector<DemoStruct>  ds_list;

  // Default constructor
  DemoStructExt() : x(0), y(0) {
  }

  // Constructor with field values
  DemoStructExt(const int32_t x,const int32_t y,const std::vector<int32_t> & int_list,const std::vector<DemoStruct> & ds_list) : x(x), y(y), int_list(int_list), ds_list(ds_list) {
  }

  // Copy constructor
  DemoStructExt(const DemoStructExt& __alt) : WirePortable(__alt)  {
    this->x = __alt.x;
    this->y = __alt.y;
    this->int_list = __alt.int_list;
    this->ds_list = __alt.ds_list;
  }

  // Assignment operator
  const DemoStructExt& operator = (const DemoStructExt& __alt) {
    this->x = __alt.x;
    this->y = __alt.y;
    this->int_list = __alt.int_list;
    this->ds_list = __alt.ds_list;
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
  bool write_int_list(yarp::os::idl::WireWriter& writer);
  bool nested_write_int_list(yarp::os::idl::WireWriter& writer);
  bool write_ds_list(yarp::os::idl::WireWriter& writer);
  bool nested_write_ds_list(yarp::os::idl::WireWriter& writer);
  bool read_x(yarp::os::idl::WireReader& reader);
  bool nested_read_x(yarp::os::idl::WireReader& reader);
  bool read_y(yarp::os::idl::WireReader& reader);
  bool nested_read_y(yarp::os::idl::WireReader& reader);
  bool read_int_list(yarp::os::idl::WireReader& reader);
  bool nested_read_int_list(yarp::os::idl::WireReader& reader);
  bool read_ds_list(yarp::os::idl::WireReader& reader);
  bool nested_read_ds_list(yarp::os::idl::WireReader& reader);

public:

  yarp::os::ConstString toString();

  // if you want to serialize this class without nesting, use this helper
  typedef yarp::os::idl::Unwrapped<DemoStructExt > unwrapped;

  class Editor : public yarp::os::Wire, public yarp::os::PortWriter {
  public:

    Editor() {
      group = 0;
      obj_owned = true;
      obj = new DemoStructExt;
      dirty_flags(false);
      yarp().setOwner(*this);
    }

    Editor(DemoStructExt& obj) {
      group = 0;
      obj_owned = false;
      edit(obj,false);
      yarp().setOwner(*this);
    }

    bool edit(DemoStructExt& obj, bool dirty = true) {
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

    DemoStructExt& state() { return *obj; }

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
    void set_int_list(const std::vector<int32_t> & int_list) {
      will_set_int_list();
      obj->int_list = int_list;
      mark_dirty_int_list();
      communicate();
      did_set_int_list();
    }
    void set_int_list(int index, const int32_t elem) {
      will_set_int_list();
      obj->int_list[index] = elem;
      mark_dirty_int_list();
      communicate();
      did_set_int_list();
    }
    void set_ds_list(const std::vector<DemoStruct> & ds_list) {
      will_set_ds_list();
      obj->ds_list = ds_list;
      mark_dirty_ds_list();
      communicate();
      did_set_ds_list();
    }
    void set_ds_list(int index, const DemoStruct& elem) {
      will_set_ds_list();
      obj->ds_list[index] = elem;
      mark_dirty_ds_list();
      communicate();
      did_set_ds_list();
    }
    int32_t get_x() {
      return obj->x;
    }
    int32_t get_y() {
      return obj->y;
    }
    const std::vector<int32_t> & get_int_list() {
      return obj->int_list;
    }
    const std::vector<DemoStruct> & get_ds_list() {
      return obj->ds_list;
    }
    virtual bool will_set_x() { return true; }
    virtual bool will_set_y() { return true; }
    virtual bool will_set_int_list() { return true; }
    virtual bool will_set_ds_list() { return true; }
    virtual bool did_set_x() { return true; }
    virtual bool did_set_y() { return true; }
    virtual bool did_set_int_list() { return true; }
    virtual bool did_set_ds_list() { return true; }
    void clean() {
      dirty_flags(false);
    }
    bool read(yarp::os::ConnectionReader& connection);
    bool write(yarp::os::ConnectionWriter& connection);
  private:

    DemoStructExt *obj;

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
    void mark_dirty_int_list() {
      if (is_dirty_int_list) return;
      dirty_count++;
      is_dirty_int_list = true;
      mark_dirty();
    }
    void mark_dirty_ds_list() {
      if (is_dirty_ds_list) return;
      dirty_count++;
      is_dirty_ds_list = true;
      mark_dirty();
    }
    void dirty_flags(bool flag) {
      is_dirty = flag;
      is_dirty_x = flag;
      is_dirty_y = flag;
      is_dirty_int_list = flag;
      is_dirty_ds_list = flag;
      dirty_count = flag ? 4 : 0;
    }
    bool is_dirty;
    int dirty_count;
    bool is_dirty_x;
    bool is_dirty_y;
    bool is_dirty_int_list;
    bool is_dirty_ds_list;
  };
};

#endif


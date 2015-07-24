// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_STRUCT_Settings
#define YARP_THRIFT_GENERATOR_STRUCT_Settings

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <Point.h>

class Settings;


class Settings : public yarp::os::idl::WirePortable {
public:
  // Fields
  int32_t id;
  std::string name;
  Point pt;
  std::vector<int32_t>  ints;

  // Default constructor
  Settings() : id(0), name("") {
  }

  // Constructor with field values
  Settings(const int32_t id,const std::string& name,const Point& pt,const std::vector<int32_t> & ints) : id(id), name(name), pt(pt), ints(ints) {
  }

  // Copy constructor
  Settings(const Settings& __alt) : WirePortable(__alt)  {
    this->id = __alt.id;
    this->name = __alt.name;
    this->pt = __alt.pt;
    this->ints = __alt.ints;
  }

  // Assignment operator
  const Settings& operator = (const Settings& __alt) {
    this->id = __alt.id;
    this->name = __alt.name;
    this->pt = __alt.pt;
    this->ints = __alt.ints;
    return *this;
  }

  // read and write structure on a connection
  bool read(yarp::os::idl::WireReader& reader);
  bool read(yarp::os::ConnectionReader& connection);
  bool write(yarp::os::idl::WireWriter& writer);
  bool write(yarp::os::ConnectionWriter& connection);

private:
  bool write_id(yarp::os::idl::WireWriter& writer);
  bool nested_write_id(yarp::os::idl::WireWriter& writer);
  bool write_name(yarp::os::idl::WireWriter& writer);
  bool nested_write_name(yarp::os::idl::WireWriter& writer);
  bool write_pt(yarp::os::idl::WireWriter& writer);
  bool nested_write_pt(yarp::os::idl::WireWriter& writer);
  bool write_ints(yarp::os::idl::WireWriter& writer);
  bool nested_write_ints(yarp::os::idl::WireWriter& writer);
  bool read_id(yarp::os::idl::WireReader& reader);
  bool nested_read_id(yarp::os::idl::WireReader& reader);
  bool read_name(yarp::os::idl::WireReader& reader);
  bool nested_read_name(yarp::os::idl::WireReader& reader);
  bool read_pt(yarp::os::idl::WireReader& reader);
  bool nested_read_pt(yarp::os::idl::WireReader& reader);
  bool read_ints(yarp::os::idl::WireReader& reader);
  bool nested_read_ints(yarp::os::idl::WireReader& reader);

public:

  yarp::os::ConstString toString();

  // if you want to serialize this class without nesting, use this helper
  typedef yarp::os::idl::Unwrapped<Settings > unwrapped;

  class Editor : public yarp::os::Wire, public yarp::os::PortWriter {
  public:

    Editor() {
      group = 0;
      obj_owned = true;
      obj = new Settings;
      dirty_flags(false);
      yarp().setOwner(*this);
    }

    Editor(Settings& obj) {
      group = 0;
      obj_owned = false;
      edit(obj,false);
      yarp().setOwner(*this);
    }

    bool edit(Settings& obj, bool dirty = true) {
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

    Settings& state() { return *obj; }

    void begin() { group++; }

    void end() {
      group--;
      if (group==0&&is_dirty) communicate();
    }
    void set_id(const int32_t id) {
      will_set_id();
      obj->id = id;
      mark_dirty_id();
      communicate();
      did_set_id();
    }
    void set_name(const std::string& name) {
      will_set_name();
      obj->name = name;
      mark_dirty_name();
      communicate();
      did_set_name();
    }
    void set_pt(const Point& pt) {
      will_set_pt();
      obj->pt = pt;
      mark_dirty_pt();
      communicate();
      did_set_pt();
    }
    void set_ints(const std::vector<int32_t> & ints) {
      will_set_ints();
      obj->ints = ints;
      mark_dirty_ints();
      communicate();
      did_set_ints();
    }
    void set_ints(int index, const int32_t elem) {
      will_set_ints();
      obj->ints[index] = elem;
      mark_dirty_ints();
      communicate();
      did_set_ints();
    }
    int32_t get_id() {
      return obj->id;
    }
    const std::string& get_name() {
      return obj->name;
    }
    const Point& get_pt() {
      return obj->pt;
    }
    const std::vector<int32_t> & get_ints() {
      return obj->ints;
    }
    virtual bool will_set_id() { return true; }
    virtual bool will_set_name() { return true; }
    virtual bool will_set_pt() { return true; }
    virtual bool will_set_ints() { return true; }
    virtual bool did_set_id() { return true; }
    virtual bool did_set_name() { return true; }
    virtual bool did_set_pt() { return true; }
    virtual bool did_set_ints() { return true; }
    void clean() {
      dirty_flags(false);
    }
    bool read(yarp::os::ConnectionReader& connection);
    bool write(yarp::os::ConnectionWriter& connection);
  private:

    Settings *obj;

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
    void mark_dirty_id() {
      if (is_dirty_id) return;
      dirty_count++;
      is_dirty_id = true;
      mark_dirty();
    }
    void mark_dirty_name() {
      if (is_dirty_name) return;
      dirty_count++;
      is_dirty_name = true;
      mark_dirty();
    }
    void mark_dirty_pt() {
      if (is_dirty_pt) return;
      dirty_count++;
      is_dirty_pt = true;
      mark_dirty();
    }
    void mark_dirty_ints() {
      if (is_dirty_ints) return;
      dirty_count++;
      is_dirty_ints = true;
      mark_dirty();
    }
    void dirty_flags(bool flag) {
      is_dirty = flag;
      is_dirty_id = flag;
      is_dirty_name = flag;
      is_dirty_pt = flag;
      is_dirty_ints = flag;
      dirty_count = flag ? 4 : 0;
    }
    bool is_dirty;
    int dirty_count;
    bool is_dirty_id;
    bool is_dirty_name;
    bool is_dirty_pt;
    bool is_dirty_ints;
  };
};

#endif


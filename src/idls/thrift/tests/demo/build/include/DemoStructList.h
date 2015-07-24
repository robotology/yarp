// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_STRUCT_DemoStructList
#define YARP_THRIFT_GENERATOR_STRUCT_DemoStructList

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <DemoStruct.h>

class DemoStructList;


class DemoStructList : public yarp::os::idl::WirePortable {
public:
  // Fields
  std::vector<DemoStruct>  lst;

  // Default constructor
  DemoStructList() {
  }

  // Constructor with field values
  DemoStructList(const std::vector<DemoStruct> & lst) : lst(lst) {
  }

  // Copy constructor
  DemoStructList(const DemoStructList& __alt) : WirePortable(__alt)  {
    this->lst = __alt.lst;
  }

  // Assignment operator
  const DemoStructList& operator = (const DemoStructList& __alt) {
    this->lst = __alt.lst;
    return *this;
  }

  // read and write structure on a connection
  bool read(yarp::os::idl::WireReader& reader);
  bool read(yarp::os::ConnectionReader& connection);
  bool write(yarp::os::idl::WireWriter& writer);
  bool write(yarp::os::ConnectionWriter& connection);

private:
  bool write_lst(yarp::os::idl::WireWriter& writer);
  bool nested_write_lst(yarp::os::idl::WireWriter& writer);
  bool read_lst(yarp::os::idl::WireReader& reader);
  bool nested_read_lst(yarp::os::idl::WireReader& reader);

public:

  yarp::os::ConstString toString();

  // if you want to serialize this class without nesting, use this helper
  typedef yarp::os::idl::Unwrapped<DemoStructList > unwrapped;

  class Editor : public yarp::os::Wire, public yarp::os::PortWriter {
  public:

    Editor() {
      group = 0;
      obj_owned = true;
      obj = new DemoStructList;
      dirty_flags(false);
      yarp().setOwner(*this);
    }

    Editor(DemoStructList& obj) {
      group = 0;
      obj_owned = false;
      edit(obj,false);
      yarp().setOwner(*this);
    }

    bool edit(DemoStructList& obj, bool dirty = true) {
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

    DemoStructList& state() { return *obj; }

    void begin() { group++; }

    void end() {
      group--;
      if (group==0&&is_dirty) communicate();
    }
    void set_lst(const std::vector<DemoStruct> & lst) {
      will_set_lst();
      obj->lst = lst;
      mark_dirty_lst();
      communicate();
      did_set_lst();
    }
    void set_lst(int index, const DemoStruct& elem) {
      will_set_lst();
      obj->lst[index] = elem;
      mark_dirty_lst();
      communicate();
      did_set_lst();
    }
    const std::vector<DemoStruct> & get_lst() {
      return obj->lst;
    }
    virtual bool will_set_lst() { return true; }
    virtual bool did_set_lst() { return true; }
    void clean() {
      dirty_flags(false);
    }
    bool read(yarp::os::ConnectionReader& connection);
    bool write(yarp::os::ConnectionWriter& connection);
  private:

    DemoStructList *obj;

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
    void mark_dirty_lst() {
      if (is_dirty_lst) return;
      dirty_count++;
      is_dirty_lst = true;
      mark_dirty();
    }
    void dirty_flags(bool flag) {
      is_dirty = flag;
      is_dirty_lst = flag;
      dirty_count = flag ? 1 : 0;
    }
    bool is_dirty;
    int dirty_count;
    bool is_dirty_lst;
  };
};

#endif


// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_STRUCT_Polygon
#define YARP_THRIFT_GENERATOR_STRUCT_Polygon

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>

class Polygon;


class Polygon : public yarp::os::idl::WirePortable {
public:
  // Fields
  std::vector<int32_t>  vertices;

  // Default constructor
  Polygon() {
  }

  // Constructor with field values
  Polygon(const std::vector<int32_t> & vertices) : vertices(vertices) {
  }

  // Copy constructor
  Polygon(const Polygon& __alt) : WirePortable(__alt)  {
    this->vertices = __alt.vertices;
  }

  // Assignment operator
  const Polygon& operator = (const Polygon& __alt) {
    this->vertices = __alt.vertices;
    return *this;
  }

  // read and write structure on a connection
  bool read(yarp::os::idl::WireReader& reader);
  bool read(yarp::os::ConnectionReader& connection);
  bool write(yarp::os::idl::WireWriter& writer);
  bool write(yarp::os::ConnectionWriter& connection);

private:
  bool write_vertices(yarp::os::idl::WireWriter& writer);
  bool nested_write_vertices(yarp::os::idl::WireWriter& writer);
  bool read_vertices(yarp::os::idl::WireReader& reader);
  bool nested_read_vertices(yarp::os::idl::WireReader& reader);

public:

  yarp::os::ConstString toString();

  // if you want to serialize this class without nesting, use this helper
  typedef yarp::os::idl::Unwrapped<Polygon > unwrapped;

  class Editor : public yarp::os::Wire, public yarp::os::PortWriter {
  public:

    Editor() {
      group = 0;
      obj_owned = true;
      obj = new Polygon;
      dirty_flags(false);
      yarp().setOwner(*this);
    }

    Editor(Polygon& obj) {
      group = 0;
      obj_owned = false;
      edit(obj,false);
      yarp().setOwner(*this);
    }

    bool edit(Polygon& obj, bool dirty = true) {
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

    Polygon& state() { return *obj; }

    void begin() { group++; }

    void end() {
      group--;
      if (group==0&&is_dirty) communicate();
    }
    void set_vertices(const std::vector<int32_t> & vertices) {
      will_set_vertices();
      obj->vertices = vertices;
      mark_dirty_vertices();
      communicate();
      did_set_vertices();
    }
    void set_vertices(int index, const int32_t elem) {
      will_set_vertices();
      obj->vertices[index] = elem;
      mark_dirty_vertices();
      communicate();
      did_set_vertices();
    }
    const std::vector<int32_t> & get_vertices() {
      return obj->vertices;
    }
    virtual bool will_set_vertices() { return true; }
    virtual bool did_set_vertices() { return true; }
    void clean() {
      dirty_flags(false);
    }
    bool read(yarp::os::ConnectionReader& connection);
    bool write(yarp::os::ConnectionWriter& connection);
  private:

    Polygon *obj;

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
    void mark_dirty_vertices() {
      if (is_dirty_vertices) return;
      dirty_count++;
      is_dirty_vertices = true;
      mark_dirty();
    }
    void dirty_flags(bool flag) {
      is_dirty = flag;
      is_dirty_vertices = flag;
      dirty_count = flag ? 1 : 0;
    }
    bool is_dirty;
    int dirty_count;
    bool is_dirty_vertices;
  };
};

#endif


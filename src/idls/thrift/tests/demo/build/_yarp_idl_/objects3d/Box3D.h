// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_STRUCT_Box3D
#define YARP_THRIFT_GENERATOR_STRUCT_Box3D

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <PointXYZ.h>
#include <yarp/sig/Matrix.h>

class Box3D;


class Box3D : public yarp::os::idl::WirePortable {
public:
  // Fields
  std::vector<PointXYZ>  corners;
  yarp::sig::Matrix orientation;

  // Default constructor
  Box3D() {
  }

  // Constructor with field values
  Box3D(const std::vector<PointXYZ> & corners,const yarp::sig::Matrix& orientation) : corners(corners), orientation(orientation) {
  }

  // Copy constructor
  Box3D(const Box3D& __alt) : WirePortable(__alt)  {
    this->corners = __alt.corners;
    this->orientation = __alt.orientation;
  }

  // Assignment operator
  const Box3D& operator = (const Box3D& __alt) {
    this->corners = __alt.corners;
    this->orientation = __alt.orientation;
    return *this;
  }

  // read and write structure on a connection
  bool read(yarp::os::idl::WireReader& reader);
  bool read(yarp::os::ConnectionReader& connection);
  bool write(yarp::os::idl::WireWriter& writer);
  bool write(yarp::os::ConnectionWriter& connection);

private:
  bool write_corners(yarp::os::idl::WireWriter& writer);
  bool nested_write_corners(yarp::os::idl::WireWriter& writer);
  bool write_orientation(yarp::os::idl::WireWriter& writer);
  bool nested_write_orientation(yarp::os::idl::WireWriter& writer);
  bool read_corners(yarp::os::idl::WireReader& reader);
  bool nested_read_corners(yarp::os::idl::WireReader& reader);
  bool read_orientation(yarp::os::idl::WireReader& reader);
  bool nested_read_orientation(yarp::os::idl::WireReader& reader);

public:

  yarp::os::ConstString toString();

  // if you want to serialize this class without nesting, use this helper
  typedef yarp::os::idl::Unwrapped<Box3D > unwrapped;

  class Editor : public yarp::os::Wire, public yarp::os::PortWriter {
  public:

    Editor() {
      group = 0;
      obj_owned = true;
      obj = new Box3D;
      dirty_flags(false);
      yarp().setOwner(*this);
    }

    Editor(Box3D& obj) {
      group = 0;
      obj_owned = false;
      edit(obj,false);
      yarp().setOwner(*this);
    }

    bool edit(Box3D& obj, bool dirty = true) {
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

    Box3D& state() { return *obj; }

    void begin() { group++; }

    void end() {
      group--;
      if (group==0&&is_dirty) communicate();
    }
    void set_corners(const std::vector<PointXYZ> & corners) {
      will_set_corners();
      obj->corners = corners;
      mark_dirty_corners();
      communicate();
      did_set_corners();
    }
    void set_corners(int index, const PointXYZ& elem) {
      will_set_corners();
      obj->corners[index] = elem;
      mark_dirty_corners();
      communicate();
      did_set_corners();
    }
    void set_orientation(const yarp::sig::Matrix& orientation) {
      will_set_orientation();
      obj->orientation = orientation;
      mark_dirty_orientation();
      communicate();
      did_set_orientation();
    }
    const std::vector<PointXYZ> & get_corners() {
      return obj->corners;
    }
    const yarp::sig::Matrix& get_orientation() {
      return obj->orientation;
    }
    virtual bool will_set_corners() { return true; }
    virtual bool will_set_orientation() { return true; }
    virtual bool did_set_corners() { return true; }
    virtual bool did_set_orientation() { return true; }
    void clean() {
      dirty_flags(false);
    }
    bool read(yarp::os::ConnectionReader& connection);
    bool write(yarp::os::ConnectionWriter& connection);
  private:

    Box3D *obj;

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
    void mark_dirty_corners() {
      if (is_dirty_corners) return;
      dirty_count++;
      is_dirty_corners = true;
      mark_dirty();
    }
    void mark_dirty_orientation() {
      if (is_dirty_orientation) return;
      dirty_count++;
      is_dirty_orientation = true;
      mark_dirty();
    }
    void dirty_flags(bool flag) {
      is_dirty = flag;
      is_dirty_corners = flag;
      is_dirty_orientation = flag;
      dirty_count = flag ? 2 : 0;
    }
    bool is_dirty;
    int dirty_count;
    bool is_dirty_corners;
    bool is_dirty_orientation;
  };
};

#endif


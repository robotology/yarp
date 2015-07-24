// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_STRUCT_SurfaceMeshWithBoundingBox
#define YARP_THRIFT_GENERATOR_STRUCT_SurfaceMeshWithBoundingBox

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <Box3D.h>
#include <SurfaceMesh.h>

class SurfaceMeshWithBoundingBox;


class SurfaceMeshWithBoundingBox : public yarp::os::idl::WirePortable {
public:
  // Fields
  SurfaceMesh mesh;
  Box3D boundingBox;

  // Default constructor
  SurfaceMeshWithBoundingBox() {
  }

  // Constructor with field values
  SurfaceMeshWithBoundingBox(const SurfaceMesh& mesh,const Box3D& boundingBox) : mesh(mesh), boundingBox(boundingBox) {
  }

  // Copy constructor
  SurfaceMeshWithBoundingBox(const SurfaceMeshWithBoundingBox& __alt) : WirePortable(__alt)  {
    this->mesh = __alt.mesh;
    this->boundingBox = __alt.boundingBox;
  }

  // Assignment operator
  const SurfaceMeshWithBoundingBox& operator = (const SurfaceMeshWithBoundingBox& __alt) {
    this->mesh = __alt.mesh;
    this->boundingBox = __alt.boundingBox;
    return *this;
  }

  // read and write structure on a connection
  bool read(yarp::os::idl::WireReader& reader);
  bool read(yarp::os::ConnectionReader& connection);
  bool write(yarp::os::idl::WireWriter& writer);
  bool write(yarp::os::ConnectionWriter& connection);

private:
  bool write_mesh(yarp::os::idl::WireWriter& writer);
  bool nested_write_mesh(yarp::os::idl::WireWriter& writer);
  bool write_boundingBox(yarp::os::idl::WireWriter& writer);
  bool nested_write_boundingBox(yarp::os::idl::WireWriter& writer);
  bool read_mesh(yarp::os::idl::WireReader& reader);
  bool nested_read_mesh(yarp::os::idl::WireReader& reader);
  bool read_boundingBox(yarp::os::idl::WireReader& reader);
  bool nested_read_boundingBox(yarp::os::idl::WireReader& reader);

public:

  yarp::os::ConstString toString();

  // if you want to serialize this class without nesting, use this helper
  typedef yarp::os::idl::Unwrapped<SurfaceMeshWithBoundingBox > unwrapped;

  class Editor : public yarp::os::Wire, public yarp::os::PortWriter {
  public:

    Editor() {
      group = 0;
      obj_owned = true;
      obj = new SurfaceMeshWithBoundingBox;
      dirty_flags(false);
      yarp().setOwner(*this);
    }

    Editor(SurfaceMeshWithBoundingBox& obj) {
      group = 0;
      obj_owned = false;
      edit(obj,false);
      yarp().setOwner(*this);
    }

    bool edit(SurfaceMeshWithBoundingBox& obj, bool dirty = true) {
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

    SurfaceMeshWithBoundingBox& state() { return *obj; }

    void begin() { group++; }

    void end() {
      group--;
      if (group==0&&is_dirty) communicate();
    }
    void set_mesh(const SurfaceMesh& mesh) {
      will_set_mesh();
      obj->mesh = mesh;
      mark_dirty_mesh();
      communicate();
      did_set_mesh();
    }
    void set_boundingBox(const Box3D& boundingBox) {
      will_set_boundingBox();
      obj->boundingBox = boundingBox;
      mark_dirty_boundingBox();
      communicate();
      did_set_boundingBox();
    }
    const SurfaceMesh& get_mesh() {
      return obj->mesh;
    }
    const Box3D& get_boundingBox() {
      return obj->boundingBox;
    }
    virtual bool will_set_mesh() { return true; }
    virtual bool will_set_boundingBox() { return true; }
    virtual bool did_set_mesh() { return true; }
    virtual bool did_set_boundingBox() { return true; }
    void clean() {
      dirty_flags(false);
    }
    bool read(yarp::os::ConnectionReader& connection);
    bool write(yarp::os::ConnectionWriter& connection);
  private:

    SurfaceMeshWithBoundingBox *obj;

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
    void mark_dirty_mesh() {
      if (is_dirty_mesh) return;
      dirty_count++;
      is_dirty_mesh = true;
      mark_dirty();
    }
    void mark_dirty_boundingBox() {
      if (is_dirty_boundingBox) return;
      dirty_count++;
      is_dirty_boundingBox = true;
      mark_dirty();
    }
    void dirty_flags(bool flag) {
      is_dirty = flag;
      is_dirty_mesh = flag;
      is_dirty_boundingBox = flag;
      dirty_count = flag ? 2 : 0;
    }
    bool is_dirty;
    int dirty_count;
    bool is_dirty_mesh;
    bool is_dirty_boundingBox;
  };
};

#endif


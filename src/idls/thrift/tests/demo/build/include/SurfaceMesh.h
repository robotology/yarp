// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_STRUCT_SurfaceMesh
#define YARP_THRIFT_GENERATOR_STRUCT_SurfaceMesh

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <PointXYZ.h>
#include <Polygon.h>
#include <RGBA.h>

class SurfaceMesh;


class SurfaceMesh : public yarp::os::idl::WirePortable {
public:
  // Fields
  std::string meshName;
  std::vector<PointXYZ>  points;
  std::vector<RGBA>  rgbColour;
  std::vector<Polygon>  polygons;

  // Default constructor
  SurfaceMesh() : meshName("") {
  }

  // Constructor with field values
  SurfaceMesh(const std::string& meshName,const std::vector<PointXYZ> & points,const std::vector<RGBA> & rgbColour,const std::vector<Polygon> & polygons) : meshName(meshName), points(points), rgbColour(rgbColour), polygons(polygons) {
  }

  // Copy constructor
  SurfaceMesh(const SurfaceMesh& __alt) : WirePortable(__alt)  {
    this->meshName = __alt.meshName;
    this->points = __alt.points;
    this->rgbColour = __alt.rgbColour;
    this->polygons = __alt.polygons;
  }

  // Assignment operator
  const SurfaceMesh& operator = (const SurfaceMesh& __alt) {
    this->meshName = __alt.meshName;
    this->points = __alt.points;
    this->rgbColour = __alt.rgbColour;
    this->polygons = __alt.polygons;
    return *this;
  }

  // read and write structure on a connection
  bool read(yarp::os::idl::WireReader& reader);
  bool read(yarp::os::ConnectionReader& connection);
  bool write(yarp::os::idl::WireWriter& writer);
  bool write(yarp::os::ConnectionWriter& connection);

private:
  bool write_meshName(yarp::os::idl::WireWriter& writer);
  bool nested_write_meshName(yarp::os::idl::WireWriter& writer);
  bool write_points(yarp::os::idl::WireWriter& writer);
  bool nested_write_points(yarp::os::idl::WireWriter& writer);
  bool write_rgbColour(yarp::os::idl::WireWriter& writer);
  bool nested_write_rgbColour(yarp::os::idl::WireWriter& writer);
  bool write_polygons(yarp::os::idl::WireWriter& writer);
  bool nested_write_polygons(yarp::os::idl::WireWriter& writer);
  bool read_meshName(yarp::os::idl::WireReader& reader);
  bool nested_read_meshName(yarp::os::idl::WireReader& reader);
  bool read_points(yarp::os::idl::WireReader& reader);
  bool nested_read_points(yarp::os::idl::WireReader& reader);
  bool read_rgbColour(yarp::os::idl::WireReader& reader);
  bool nested_read_rgbColour(yarp::os::idl::WireReader& reader);
  bool read_polygons(yarp::os::idl::WireReader& reader);
  bool nested_read_polygons(yarp::os::idl::WireReader& reader);

public:

  yarp::os::ConstString toString();

  // if you want to serialize this class without nesting, use this helper
  typedef yarp::os::idl::Unwrapped<SurfaceMesh > unwrapped;

  class Editor : public yarp::os::Wire, public yarp::os::PortWriter {
  public:

    Editor() {
      group = 0;
      obj_owned = true;
      obj = new SurfaceMesh;
      dirty_flags(false);
      yarp().setOwner(*this);
    }

    Editor(SurfaceMesh& obj) {
      group = 0;
      obj_owned = false;
      edit(obj,false);
      yarp().setOwner(*this);
    }

    bool edit(SurfaceMesh& obj, bool dirty = true) {
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

    SurfaceMesh& state() { return *obj; }

    void begin() { group++; }

    void end() {
      group--;
      if (group==0&&is_dirty) communicate();
    }
    void set_meshName(const std::string& meshName) {
      will_set_meshName();
      obj->meshName = meshName;
      mark_dirty_meshName();
      communicate();
      did_set_meshName();
    }
    void set_points(const std::vector<PointXYZ> & points) {
      will_set_points();
      obj->points = points;
      mark_dirty_points();
      communicate();
      did_set_points();
    }
    void set_points(int index, const PointXYZ& elem) {
      will_set_points();
      obj->points[index] = elem;
      mark_dirty_points();
      communicate();
      did_set_points();
    }
    void set_rgbColour(const std::vector<RGBA> & rgbColour) {
      will_set_rgbColour();
      obj->rgbColour = rgbColour;
      mark_dirty_rgbColour();
      communicate();
      did_set_rgbColour();
    }
    void set_rgbColour(int index, const RGBA& elem) {
      will_set_rgbColour();
      obj->rgbColour[index] = elem;
      mark_dirty_rgbColour();
      communicate();
      did_set_rgbColour();
    }
    void set_polygons(const std::vector<Polygon> & polygons) {
      will_set_polygons();
      obj->polygons = polygons;
      mark_dirty_polygons();
      communicate();
      did_set_polygons();
    }
    void set_polygons(int index, const Polygon& elem) {
      will_set_polygons();
      obj->polygons[index] = elem;
      mark_dirty_polygons();
      communicate();
      did_set_polygons();
    }
    const std::string& get_meshName() {
      return obj->meshName;
    }
    const std::vector<PointXYZ> & get_points() {
      return obj->points;
    }
    const std::vector<RGBA> & get_rgbColour() {
      return obj->rgbColour;
    }
    const std::vector<Polygon> & get_polygons() {
      return obj->polygons;
    }
    virtual bool will_set_meshName() { return true; }
    virtual bool will_set_points() { return true; }
    virtual bool will_set_rgbColour() { return true; }
    virtual bool will_set_polygons() { return true; }
    virtual bool did_set_meshName() { return true; }
    virtual bool did_set_points() { return true; }
    virtual bool did_set_rgbColour() { return true; }
    virtual bool did_set_polygons() { return true; }
    void clean() {
      dirty_flags(false);
    }
    bool read(yarp::os::ConnectionReader& connection);
    bool write(yarp::os::ConnectionWriter& connection);
  private:

    SurfaceMesh *obj;

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
    void mark_dirty_meshName() {
      if (is_dirty_meshName) return;
      dirty_count++;
      is_dirty_meshName = true;
      mark_dirty();
    }
    void mark_dirty_points() {
      if (is_dirty_points) return;
      dirty_count++;
      is_dirty_points = true;
      mark_dirty();
    }
    void mark_dirty_rgbColour() {
      if (is_dirty_rgbColour) return;
      dirty_count++;
      is_dirty_rgbColour = true;
      mark_dirty();
    }
    void mark_dirty_polygons() {
      if (is_dirty_polygons) return;
      dirty_count++;
      is_dirty_polygons = true;
      mark_dirty();
    }
    void dirty_flags(bool flag) {
      is_dirty = flag;
      is_dirty_meshName = flag;
      is_dirty_points = flag;
      is_dirty_rgbColour = flag;
      is_dirty_polygons = flag;
      dirty_count = flag ? 4 : 0;
    }
    bool is_dirty;
    int dirty_count;
    bool is_dirty_meshName;
    bool is_dirty_points;
    bool is_dirty_rgbColour;
    bool is_dirty_polygons;
  };
};

#endif


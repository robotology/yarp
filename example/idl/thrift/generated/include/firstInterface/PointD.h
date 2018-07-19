/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_STRUCT_PointD
#define YARP_THRIFT_GENERATOR_STRUCT_PointD

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>

namespace yarp {
  namespace test {
    class PointD;
  }
}


class yarp::test::PointD : public yarp::os::idl::WirePortable {
public:
  // Fields
  std::int32_t x;
  std::int32_t y;
  std::int32_t z;

  // Default constructor
  PointD() : x(0), y(0), z(0) {
  }

  // Constructor with field values
  PointD(const std::int32_t x,const std::int32_t y,const std::int32_t z) : x(x), y(y), z(z) {
  }

  // Copy constructor
  PointD(const PointD& __alt) : WirePortable(__alt)  {
    this->x = __alt.x;
    this->y = __alt.y;
    this->z = __alt.z;
  }

  // Assignment operator
  const PointD& operator = (const PointD& __alt) {
    this->x = __alt.x;
    this->y = __alt.y;
    this->z = __alt.z;
    return *this;
  }

  // read and write structure on a connection
  bool read(yarp::os::idl::WireReader& reader) override;
  bool read(yarp::os::ConnectionReader& connection) override;
  bool write(const yarp::os::idl::WireWriter& writer) const override;
  bool write(yarp::os::ConnectionWriter& connection) const override;

private:
  bool write_x(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_x(const yarp::os::idl::WireWriter& writer) const;
  bool write_y(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_y(const yarp::os::idl::WireWriter& writer) const;
  bool write_z(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_z(const yarp::os::idl::WireWriter& writer) const;
  bool read_x(yarp::os::idl::WireReader& reader);
  bool nested_read_x(yarp::os::idl::WireReader& reader);
  bool read_y(yarp::os::idl::WireReader& reader);
  bool nested_read_y(yarp::os::idl::WireReader& reader);
  bool read_z(yarp::os::idl::WireReader& reader);
  bool nested_read_z(yarp::os::idl::WireReader& reader);

public:

  std::string toString() const;

  // if you want to serialize this class without nesting, use this helper
  typedef yarp::os::idl::Unwrapped<yarp::test::PointD > unwrapped;

  class Editor : public yarp::os::Wire, public yarp::os::PortWriter {
  public:

    Editor() {
      group = 0;
      obj_owned = true;
      obj = new PointD;
      dirty_flags(false);
      yarp().setOwner(*this);
    }

    Editor(PointD& obj) {
      group = 0;
      obj_owned = false;
      edit(obj,false);
      yarp().setOwner(*this);
    }

    bool edit(PointD& obj, bool dirty = true) {
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

    PointD& state() { return *obj; }

    void begin() { group++; }

    void end() {
      group--;
      if (group==0&&is_dirty) communicate();
    }
    void set_x(const std::int32_t x) {
      will_set_x();
      obj->x = x;
      mark_dirty_x();
      communicate();
      did_set_x();
    }
    void set_y(const std::int32_t y) {
      will_set_y();
      obj->y = y;
      mark_dirty_y();
      communicate();
      did_set_y();
    }
    void set_z(const std::int32_t z) {
      will_set_z();
      obj->z = z;
      mark_dirty_z();
      communicate();
      did_set_z();
    }
    std::int32_t get_x() {
      return obj->x;
    }
    std::int32_t get_y() {
      return obj->y;
    }
    std::int32_t get_z() {
      return obj->z;
    }
    virtual bool will_set_x() { return true; }
    virtual bool will_set_y() { return true; }
    virtual bool will_set_z() { return true; }
    virtual bool did_set_x() { return true; }
    virtual bool did_set_y() { return true; }
    virtual bool did_set_z() { return true; }
    void clean() {
      dirty_flags(false);
    }
    bool read(yarp::os::ConnectionReader& connection) override;
    bool write(yarp::os::ConnectionWriter& connection) const override;
  private:

    PointD *obj;

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
    void mark_dirty_z() {
      if (is_dirty_z) return;
      dirty_count++;
      is_dirty_z = true;
      mark_dirty();
    }
    void dirty_flags(bool flag) {
      is_dirty = flag;
      is_dirty_x = flag;
      is_dirty_y = flag;
      is_dirty_z = flag;
      dirty_count = flag ? 3 : 0;
    }
    bool is_dirty;
    int dirty_count;
    bool is_dirty_x;
    bool is_dirty_y;
    bool is_dirty_z;
  };
};

#endif

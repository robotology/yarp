/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_STRUCT_PointWithQuality
#define YARP_THRIFT_GENERATOR_STRUCT_PointWithQuality

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <firstInterface/PointD.h>
#include <firstInterface/PointQuality.h>

namespace yarp {
  namespace test {
    class PointWithQuality;
  }
}


class yarp::test::PointWithQuality : public yarp::os::idl::WirePortable {
public:
  // Fields
  PointD point;
  PointQuality quality;

  // Default constructor
  PointWithQuality() : quality((PointQuality)0) {
    quality = (PointQuality)0;

  }

  // Constructor with field values
  PointWithQuality(const PointD& point,const PointQuality quality) : point(point), quality(quality) {
  }

  // Copy constructor
  PointWithQuality(const PointWithQuality& __alt) : WirePortable(__alt)  {
    this->point = __alt.point;
    this->quality = __alt.quality;
  }

  // Assignment operator
  const PointWithQuality& operator = (const PointWithQuality& __alt) {
    this->point = __alt.point;
    this->quality = __alt.quality;
    return *this;
  }

  // read and write structure on a connection
  bool read(yarp::os::idl::WireReader& reader) override;
  bool read(yarp::os::ConnectionReader& connection) override;
  bool write(const yarp::os::idl::WireWriter& writer) const override;
  bool write(yarp::os::ConnectionWriter& connection) const override;

private:
  bool write_point(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_point(const yarp::os::idl::WireWriter& writer) const;
  bool write_quality(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_quality(const yarp::os::idl::WireWriter& writer) const;
  bool read_point(yarp::os::idl::WireReader& reader);
  bool nested_read_point(yarp::os::idl::WireReader& reader);
  bool read_quality(yarp::os::idl::WireReader& reader);
  bool nested_read_quality(yarp::os::idl::WireReader& reader);

public:

  std::string toString() const;

  // if you want to serialize this class without nesting, use this helper
  typedef yarp::os::idl::Unwrapped<yarp::test::PointWithQuality > unwrapped;

  class Editor : public yarp::os::Wire, public yarp::os::PortWriter {
  public:

    Editor() {
      group = 0;
      obj_owned = true;
      obj = new PointWithQuality;
      dirty_flags(false);
      yarp().setOwner(*this);
    }

    Editor(PointWithQuality& obj) {
      group = 0;
      obj_owned = false;
      edit(obj,false);
      yarp().setOwner(*this);
    }

    bool edit(PointWithQuality& obj, bool dirty = true) {
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

    PointWithQuality& state() { return *obj; }

    void begin() { group++; }

    void end() {
      group--;
      if (group==0&&is_dirty) communicate();
    }
    void set_point(const PointD& point) {
      will_set_point();
      obj->point = point;
      mark_dirty_point();
      communicate();
      did_set_point();
    }
    void set_quality(const PointQuality quality) {
      will_set_quality();
      obj->quality = quality;
      mark_dirty_quality();
      communicate();
      did_set_quality();
    }
    const PointD& get_point() {
      return obj->point;
    }
    const PointQuality get_quality() {
      return obj->quality;
    }
    virtual bool will_set_point() { return true; }
    virtual bool will_set_quality() { return true; }
    virtual bool did_set_point() { return true; }
    virtual bool did_set_quality() { return true; }
    void clean() {
      dirty_flags(false);
    }
    bool read(yarp::os::ConnectionReader& connection) override;
    bool write(yarp::os::ConnectionWriter& connection) const override;
  private:

    PointWithQuality *obj;

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
    void mark_dirty_point() {
      if (is_dirty_point) return;
      dirty_count++;
      is_dirty_point = true;
      mark_dirty();
    }
    void mark_dirty_quality() {
      if (is_dirty_quality) return;
      dirty_count++;
      is_dirty_quality = true;
      mark_dirty();
    }
    void dirty_flags(bool flag) {
      is_dirty = flag;
      is_dirty_point = flag;
      is_dirty_quality = flag;
      dirty_count = flag ? 2 : 0;
    }
    bool is_dirty;
    int dirty_count;
    bool is_dirty_point;
    bool is_dirty_quality;
  };
};

#endif

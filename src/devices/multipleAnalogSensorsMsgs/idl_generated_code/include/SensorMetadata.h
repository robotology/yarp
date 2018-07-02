/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_STRUCT_SensorMetadata
#define YARP_THRIFT_GENERATOR_STRUCT_SensorMetadata

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>

class SensorMetadata;


class SensorMetadata : public yarp::os::idl::WirePortable {
public:
  // Fields
  std::string name;
  std::string frameName;
  std::string additionalMetadata;

  // Default constructor
  SensorMetadata() : name(""), frameName(""), additionalMetadata("") {
  }

  // Constructor with field values
  SensorMetadata(const std::string& name,const std::string& frameName,const std::string& additionalMetadata) : name(name), frameName(frameName), additionalMetadata(additionalMetadata) {
  }

  // Copy constructor
  SensorMetadata(const SensorMetadata& __alt) : WirePortable(__alt)  {
    this->name = __alt.name;
    this->frameName = __alt.frameName;
    this->additionalMetadata = __alt.additionalMetadata;
  }

  // Assignment operator
  const SensorMetadata& operator = (const SensorMetadata& __alt) {
    this->name = __alt.name;
    this->frameName = __alt.frameName;
    this->additionalMetadata = __alt.additionalMetadata;
    return *this;
  }

  // read and write structure on a connection
  bool read(yarp::os::idl::WireReader& reader) override;
  bool read(yarp::os::ConnectionReader& connection) override;
  bool write(const yarp::os::idl::WireWriter& writer) const override;
  bool write(yarp::os::ConnectionWriter& connection) const override;

private:
  bool write_name(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_name(const yarp::os::idl::WireWriter& writer) const;
  bool write_frameName(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_frameName(const yarp::os::idl::WireWriter& writer) const;
  bool write_additionalMetadata(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_additionalMetadata(const yarp::os::idl::WireWriter& writer) const;
  bool read_name(yarp::os::idl::WireReader& reader);
  bool nested_read_name(yarp::os::idl::WireReader& reader);
  bool read_frameName(yarp::os::idl::WireReader& reader);
  bool nested_read_frameName(yarp::os::idl::WireReader& reader);
  bool read_additionalMetadata(yarp::os::idl::WireReader& reader);
  bool nested_read_additionalMetadata(yarp::os::idl::WireReader& reader);

public:

  std::string toString() const;

  // if you want to serialize this class without nesting, use this helper
  typedef yarp::os::idl::Unwrapped<SensorMetadata > unwrapped;

  class Editor : public yarp::os::Wire, public yarp::os::PortWriter {
  public:

    Editor() {
      group = 0;
      obj_owned = true;
      obj = new SensorMetadata;
      dirty_flags(false);
      yarp().setOwner(*this);
    }

    Editor(SensorMetadata& obj) {
      group = 0;
      obj_owned = false;
      edit(obj,false);
      yarp().setOwner(*this);
    }

    bool edit(SensorMetadata& obj, bool dirty = true) {
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

    SensorMetadata& state() { return *obj; }

    void begin() { group++; }

    void end() {
      group--;
      if (group==0&&is_dirty) communicate();
    }
    void set_name(const std::string& name) {
      will_set_name();
      obj->name = name;
      mark_dirty_name();
      communicate();
      did_set_name();
    }
    void set_frameName(const std::string& frameName) {
      will_set_frameName();
      obj->frameName = frameName;
      mark_dirty_frameName();
      communicate();
      did_set_frameName();
    }
    void set_additionalMetadata(const std::string& additionalMetadata) {
      will_set_additionalMetadata();
      obj->additionalMetadata = additionalMetadata;
      mark_dirty_additionalMetadata();
      communicate();
      did_set_additionalMetadata();
    }
    const std::string& get_name() {
      return obj->name;
    }
    const std::string& get_frameName() {
      return obj->frameName;
    }
    const std::string& get_additionalMetadata() {
      return obj->additionalMetadata;
    }
    virtual bool will_set_name() { return true; }
    virtual bool will_set_frameName() { return true; }
    virtual bool will_set_additionalMetadata() { return true; }
    virtual bool did_set_name() { return true; }
    virtual bool did_set_frameName() { return true; }
    virtual bool did_set_additionalMetadata() { return true; }
    void clean() {
      dirty_flags(false);
    }
    bool read(yarp::os::ConnectionReader& connection) override;
    bool write(yarp::os::ConnectionWriter& connection) const override;
  private:

    SensorMetadata *obj;

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
    void mark_dirty_name() {
      if (is_dirty_name) return;
      dirty_count++;
      is_dirty_name = true;
      mark_dirty();
    }
    void mark_dirty_frameName() {
      if (is_dirty_frameName) return;
      dirty_count++;
      is_dirty_frameName = true;
      mark_dirty();
    }
    void mark_dirty_additionalMetadata() {
      if (is_dirty_additionalMetadata) return;
      dirty_count++;
      is_dirty_additionalMetadata = true;
      mark_dirty();
    }
    void dirty_flags(bool flag) {
      is_dirty = flag;
      is_dirty_name = flag;
      is_dirty_frameName = flag;
      is_dirty_additionalMetadata = flag;
      dirty_count = flag ? 3 : 0;
    }
    bool is_dirty;
    int dirty_count;
    bool is_dirty_name;
    bool is_dirty_frameName;
    bool is_dirty_additionalMetadata;
  };
};

#endif

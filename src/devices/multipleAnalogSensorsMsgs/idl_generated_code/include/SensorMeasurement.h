/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_STRUCT_SensorMeasurement
#define YARP_THRIFT_GENERATOR_STRUCT_SensorMeasurement

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <yarp/sig/Vector.h>

class SensorMeasurement;


class SensorMeasurement : public yarp::os::idl::WirePortable {
public:
  // Fields
  yarp::sig::Vector measurement;
  double timestamp;

  // Default constructor
  SensorMeasurement() : timestamp(0) {
  }

  // Constructor with field values
  SensorMeasurement(const yarp::sig::Vector& measurement,const double timestamp) : measurement(measurement), timestamp(timestamp) {
  }

  // Copy constructor
  SensorMeasurement(const SensorMeasurement& __alt) : WirePortable(__alt)  {
    this->measurement = __alt.measurement;
    this->timestamp = __alt.timestamp;
  }

  // Assignment operator
  const SensorMeasurement& operator = (const SensorMeasurement& __alt) {
    this->measurement = __alt.measurement;
    this->timestamp = __alt.timestamp;
    return *this;
  }

  // read and write structure on a connection
  bool read(yarp::os::idl::WireReader& reader) override;
  bool read(yarp::os::ConnectionReader& connection) override;
  bool write(const yarp::os::idl::WireWriter& writer) const override;
  bool write(yarp::os::ConnectionWriter& connection) const override;

private:
  bool write_measurement(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_measurement(const yarp::os::idl::WireWriter& writer) const;
  bool write_timestamp(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_timestamp(const yarp::os::idl::WireWriter& writer) const;
  bool read_measurement(yarp::os::idl::WireReader& reader);
  bool nested_read_measurement(yarp::os::idl::WireReader& reader);
  bool read_timestamp(yarp::os::idl::WireReader& reader);
  bool nested_read_timestamp(yarp::os::idl::WireReader& reader);

public:

  std::string toString() const;

  // if you want to serialize this class without nesting, use this helper
  typedef yarp::os::idl::Unwrapped<SensorMeasurement > unwrapped;

  class Editor : public yarp::os::Wire, public yarp::os::PortWriter {
  public:

    Editor() {
      group = 0;
      obj_owned = true;
      obj = new SensorMeasurement;
      dirty_flags(false);
      yarp().setOwner(*this);
    }

    Editor(SensorMeasurement& obj) {
      group = 0;
      obj_owned = false;
      edit(obj,false);
      yarp().setOwner(*this);
    }

    bool edit(SensorMeasurement& obj, bool dirty = true) {
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

    SensorMeasurement& state() { return *obj; }

    void begin() { group++; }

    void end() {
      group--;
      if (group==0&&is_dirty) communicate();
    }
    void set_measurement(const yarp::sig::Vector& measurement) {
      will_set_measurement();
      obj->measurement = measurement;
      mark_dirty_measurement();
      communicate();
      did_set_measurement();
    }
    void set_timestamp(const double timestamp) {
      will_set_timestamp();
      obj->timestamp = timestamp;
      mark_dirty_timestamp();
      communicate();
      did_set_timestamp();
    }
    const yarp::sig::Vector& get_measurement() {
      return obj->measurement;
    }
    double get_timestamp() {
      return obj->timestamp;
    }
    virtual bool will_set_measurement() { return true; }
    virtual bool will_set_timestamp() { return true; }
    virtual bool did_set_measurement() { return true; }
    virtual bool did_set_timestamp() { return true; }
    void clean() {
      dirty_flags(false);
    }
    bool read(yarp::os::ConnectionReader& connection) override;
    bool write(yarp::os::ConnectionWriter& connection) const override;
  private:

    SensorMeasurement *obj;

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
    void mark_dirty_measurement() {
      if (is_dirty_measurement) return;
      dirty_count++;
      is_dirty_measurement = true;
      mark_dirty();
    }
    void mark_dirty_timestamp() {
      if (is_dirty_timestamp) return;
      dirty_count++;
      is_dirty_timestamp = true;
      mark_dirty();
    }
    void dirty_flags(bool flag) {
      is_dirty = flag;
      is_dirty_measurement = flag;
      is_dirty_timestamp = flag;
      dirty_count = flag ? 2 : 0;
    }
    bool is_dirty;
    int dirty_count;
    bool is_dirty_measurement;
    bool is_dirty_timestamp;
  };
};

#endif

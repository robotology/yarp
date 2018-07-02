/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_STRUCT_SensorMeasurements
#define YARP_THRIFT_GENERATOR_STRUCT_SensorMeasurements

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <SensorMeasurement.h>

class SensorMeasurements;


class SensorMeasurements : public yarp::os::idl::WirePortable {
public:
  // Fields
  std::vector<SensorMeasurement>  measurements;

  // Default constructor
  SensorMeasurements() {
  }

  // Constructor with field values
  SensorMeasurements(const std::vector<SensorMeasurement> & measurements) : measurements(measurements) {
  }

  // Copy constructor
  SensorMeasurements(const SensorMeasurements& __alt) : WirePortable(__alt)  {
    this->measurements = __alt.measurements;
  }

  // Assignment operator
  const SensorMeasurements& operator = (const SensorMeasurements& __alt) {
    this->measurements = __alt.measurements;
    return *this;
  }

  // read and write structure on a connection
  bool read(yarp::os::idl::WireReader& reader) override;
  bool read(yarp::os::ConnectionReader& connection) override;
  bool write(const yarp::os::idl::WireWriter& writer) const override;
  bool write(yarp::os::ConnectionWriter& connection) const override;

private:
  bool write_measurements(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_measurements(const yarp::os::idl::WireWriter& writer) const;
  bool read_measurements(yarp::os::idl::WireReader& reader);
  bool nested_read_measurements(yarp::os::idl::WireReader& reader);

public:

  std::string toString() const;

  // if you want to serialize this class without nesting, use this helper
  typedef yarp::os::idl::Unwrapped<SensorMeasurements > unwrapped;

  class Editor : public yarp::os::Wire, public yarp::os::PortWriter {
  public:

    Editor() {
      group = 0;
      obj_owned = true;
      obj = new SensorMeasurements;
      dirty_flags(false);
      yarp().setOwner(*this);
    }

    Editor(SensorMeasurements& obj) {
      group = 0;
      obj_owned = false;
      edit(obj,false);
      yarp().setOwner(*this);
    }

    bool edit(SensorMeasurements& obj, bool dirty = true) {
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

    SensorMeasurements& state() { return *obj; }

    void begin() { group++; }

    void end() {
      group--;
      if (group==0&&is_dirty) communicate();
    }
    void set_measurements(const std::vector<SensorMeasurement> & measurements) {
      will_set_measurements();
      obj->measurements = measurements;
      mark_dirty_measurements();
      communicate();
      did_set_measurements();
    }
    void set_measurements(int index, const SensorMeasurement& elem) {
      will_set_measurements();
      obj->measurements[index] = elem;
      mark_dirty_measurements();
      communicate();
      did_set_measurements();
    }
    const std::vector<SensorMeasurement> & get_measurements() {
      return obj->measurements;
    }
    virtual bool will_set_measurements() { return true; }
    virtual bool did_set_measurements() { return true; }
    void clean() {
      dirty_flags(false);
    }
    bool read(yarp::os::ConnectionReader& connection) override;
    bool write(yarp::os::ConnectionWriter& connection) const override;
  private:

    SensorMeasurements *obj;

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
    void mark_dirty_measurements() {
      if (is_dirty_measurements) return;
      dirty_count++;
      is_dirty_measurements = true;
      mark_dirty();
    }
    void dirty_flags(bool flag) {
      is_dirty = flag;
      is_dirty_measurements = flag;
      dirty_count = flag ? 1 : 0;
    }
    bool is_dirty;
    int dirty_count;
    bool is_dirty_measurements;
  };
};

#endif

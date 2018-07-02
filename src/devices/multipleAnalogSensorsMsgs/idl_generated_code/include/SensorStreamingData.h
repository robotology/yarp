/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_STRUCT_SensorStreamingData
#define YARP_THRIFT_GENERATOR_STRUCT_SensorStreamingData

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <SensorMeasurements.h>

class SensorStreamingData;


class SensorStreamingData : public yarp::os::idl::WirePortable {
public:
  // Fields
  SensorMeasurements ThreeAxisGyroscopes;
  SensorMeasurements ThreeAxisLinearAccelerometers;
  SensorMeasurements ThreeAxisMagnetometers;
  SensorMeasurements OrientationSensors;
  SensorMeasurements TemperatureSensors;
  SensorMeasurements SixAxisForceTorqueSensors;
  SensorMeasurements ContactLoadCellArrays;
  SensorMeasurements EncoderArrays;
  SensorMeasurements SkinPatches;

  // Default constructor
  SensorStreamingData() {
  }

  // Constructor with field values
  SensorStreamingData(const SensorMeasurements& ThreeAxisGyroscopes,const SensorMeasurements& ThreeAxisLinearAccelerometers,const SensorMeasurements& ThreeAxisMagnetometers,const SensorMeasurements& OrientationSensors,const SensorMeasurements& TemperatureSensors,const SensorMeasurements& SixAxisForceTorqueSensors,const SensorMeasurements& ContactLoadCellArrays,const SensorMeasurements& EncoderArrays,const SensorMeasurements& SkinPatches) : ThreeAxisGyroscopes(ThreeAxisGyroscopes), ThreeAxisLinearAccelerometers(ThreeAxisLinearAccelerometers), ThreeAxisMagnetometers(ThreeAxisMagnetometers), OrientationSensors(OrientationSensors), TemperatureSensors(TemperatureSensors), SixAxisForceTorqueSensors(SixAxisForceTorqueSensors), ContactLoadCellArrays(ContactLoadCellArrays), EncoderArrays(EncoderArrays), SkinPatches(SkinPatches) {
  }

  // Copy constructor
  SensorStreamingData(const SensorStreamingData& __alt) : WirePortable(__alt)  {
    this->ThreeAxisGyroscopes = __alt.ThreeAxisGyroscopes;
    this->ThreeAxisLinearAccelerometers = __alt.ThreeAxisLinearAccelerometers;
    this->ThreeAxisMagnetometers = __alt.ThreeAxisMagnetometers;
    this->OrientationSensors = __alt.OrientationSensors;
    this->TemperatureSensors = __alt.TemperatureSensors;
    this->SixAxisForceTorqueSensors = __alt.SixAxisForceTorqueSensors;
    this->ContactLoadCellArrays = __alt.ContactLoadCellArrays;
    this->EncoderArrays = __alt.EncoderArrays;
    this->SkinPatches = __alt.SkinPatches;
  }

  // Assignment operator
  const SensorStreamingData& operator = (const SensorStreamingData& __alt) {
    this->ThreeAxisGyroscopes = __alt.ThreeAxisGyroscopes;
    this->ThreeAxisLinearAccelerometers = __alt.ThreeAxisLinearAccelerometers;
    this->ThreeAxisMagnetometers = __alt.ThreeAxisMagnetometers;
    this->OrientationSensors = __alt.OrientationSensors;
    this->TemperatureSensors = __alt.TemperatureSensors;
    this->SixAxisForceTorqueSensors = __alt.SixAxisForceTorqueSensors;
    this->ContactLoadCellArrays = __alt.ContactLoadCellArrays;
    this->EncoderArrays = __alt.EncoderArrays;
    this->SkinPatches = __alt.SkinPatches;
    return *this;
  }

  // read and write structure on a connection
  bool read(yarp::os::idl::WireReader& reader) override;
  bool read(yarp::os::ConnectionReader& connection) override;
  bool write(const yarp::os::idl::WireWriter& writer) const override;
  bool write(yarp::os::ConnectionWriter& connection) const override;

private:
  bool write_ThreeAxisGyroscopes(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_ThreeAxisGyroscopes(const yarp::os::idl::WireWriter& writer) const;
  bool write_ThreeAxisLinearAccelerometers(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_ThreeAxisLinearAccelerometers(const yarp::os::idl::WireWriter& writer) const;
  bool write_ThreeAxisMagnetometers(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_ThreeAxisMagnetometers(const yarp::os::idl::WireWriter& writer) const;
  bool write_OrientationSensors(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_OrientationSensors(const yarp::os::idl::WireWriter& writer) const;
  bool write_TemperatureSensors(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_TemperatureSensors(const yarp::os::idl::WireWriter& writer) const;
  bool write_SixAxisForceTorqueSensors(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_SixAxisForceTorqueSensors(const yarp::os::idl::WireWriter& writer) const;
  bool write_ContactLoadCellArrays(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_ContactLoadCellArrays(const yarp::os::idl::WireWriter& writer) const;
  bool write_EncoderArrays(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_EncoderArrays(const yarp::os::idl::WireWriter& writer) const;
  bool write_SkinPatches(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_SkinPatches(const yarp::os::idl::WireWriter& writer) const;
  bool read_ThreeAxisGyroscopes(yarp::os::idl::WireReader& reader);
  bool nested_read_ThreeAxisGyroscopes(yarp::os::idl::WireReader& reader);
  bool read_ThreeAxisLinearAccelerometers(yarp::os::idl::WireReader& reader);
  bool nested_read_ThreeAxisLinearAccelerometers(yarp::os::idl::WireReader& reader);
  bool read_ThreeAxisMagnetometers(yarp::os::idl::WireReader& reader);
  bool nested_read_ThreeAxisMagnetometers(yarp::os::idl::WireReader& reader);
  bool read_OrientationSensors(yarp::os::idl::WireReader& reader);
  bool nested_read_OrientationSensors(yarp::os::idl::WireReader& reader);
  bool read_TemperatureSensors(yarp::os::idl::WireReader& reader);
  bool nested_read_TemperatureSensors(yarp::os::idl::WireReader& reader);
  bool read_SixAxisForceTorqueSensors(yarp::os::idl::WireReader& reader);
  bool nested_read_SixAxisForceTorqueSensors(yarp::os::idl::WireReader& reader);
  bool read_ContactLoadCellArrays(yarp::os::idl::WireReader& reader);
  bool nested_read_ContactLoadCellArrays(yarp::os::idl::WireReader& reader);
  bool read_EncoderArrays(yarp::os::idl::WireReader& reader);
  bool nested_read_EncoderArrays(yarp::os::idl::WireReader& reader);
  bool read_SkinPatches(yarp::os::idl::WireReader& reader);
  bool nested_read_SkinPatches(yarp::os::idl::WireReader& reader);

public:

  std::string toString() const;

  // if you want to serialize this class without nesting, use this helper
  typedef yarp::os::idl::Unwrapped<SensorStreamingData > unwrapped;

  class Editor : public yarp::os::Wire, public yarp::os::PortWriter {
  public:

    Editor() {
      group = 0;
      obj_owned = true;
      obj = new SensorStreamingData;
      dirty_flags(false);
      yarp().setOwner(*this);
    }

    Editor(SensorStreamingData& obj) {
      group = 0;
      obj_owned = false;
      edit(obj,false);
      yarp().setOwner(*this);
    }

    bool edit(SensorStreamingData& obj, bool dirty = true) {
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

    SensorStreamingData& state() { return *obj; }

    void begin() { group++; }

    void end() {
      group--;
      if (group==0&&is_dirty) communicate();
    }
    void set_ThreeAxisGyroscopes(const SensorMeasurements& ThreeAxisGyroscopes) {
      will_set_ThreeAxisGyroscopes();
      obj->ThreeAxisGyroscopes = ThreeAxisGyroscopes;
      mark_dirty_ThreeAxisGyroscopes();
      communicate();
      did_set_ThreeAxisGyroscopes();
    }
    void set_ThreeAxisLinearAccelerometers(const SensorMeasurements& ThreeAxisLinearAccelerometers) {
      will_set_ThreeAxisLinearAccelerometers();
      obj->ThreeAxisLinearAccelerometers = ThreeAxisLinearAccelerometers;
      mark_dirty_ThreeAxisLinearAccelerometers();
      communicate();
      did_set_ThreeAxisLinearAccelerometers();
    }
    void set_ThreeAxisMagnetometers(const SensorMeasurements& ThreeAxisMagnetometers) {
      will_set_ThreeAxisMagnetometers();
      obj->ThreeAxisMagnetometers = ThreeAxisMagnetometers;
      mark_dirty_ThreeAxisMagnetometers();
      communicate();
      did_set_ThreeAxisMagnetometers();
    }
    void set_OrientationSensors(const SensorMeasurements& OrientationSensors) {
      will_set_OrientationSensors();
      obj->OrientationSensors = OrientationSensors;
      mark_dirty_OrientationSensors();
      communicate();
      did_set_OrientationSensors();
    }
    void set_TemperatureSensors(const SensorMeasurements& TemperatureSensors) {
      will_set_TemperatureSensors();
      obj->TemperatureSensors = TemperatureSensors;
      mark_dirty_TemperatureSensors();
      communicate();
      did_set_TemperatureSensors();
    }
    void set_SixAxisForceTorqueSensors(const SensorMeasurements& SixAxisForceTorqueSensors) {
      will_set_SixAxisForceTorqueSensors();
      obj->SixAxisForceTorqueSensors = SixAxisForceTorqueSensors;
      mark_dirty_SixAxisForceTorqueSensors();
      communicate();
      did_set_SixAxisForceTorqueSensors();
    }
    void set_ContactLoadCellArrays(const SensorMeasurements& ContactLoadCellArrays) {
      will_set_ContactLoadCellArrays();
      obj->ContactLoadCellArrays = ContactLoadCellArrays;
      mark_dirty_ContactLoadCellArrays();
      communicate();
      did_set_ContactLoadCellArrays();
    }
    void set_EncoderArrays(const SensorMeasurements& EncoderArrays) {
      will_set_EncoderArrays();
      obj->EncoderArrays = EncoderArrays;
      mark_dirty_EncoderArrays();
      communicate();
      did_set_EncoderArrays();
    }
    void set_SkinPatches(const SensorMeasurements& SkinPatches) {
      will_set_SkinPatches();
      obj->SkinPatches = SkinPatches;
      mark_dirty_SkinPatches();
      communicate();
      did_set_SkinPatches();
    }
    const SensorMeasurements& get_ThreeAxisGyroscopes() {
      return obj->ThreeAxisGyroscopes;
    }
    const SensorMeasurements& get_ThreeAxisLinearAccelerometers() {
      return obj->ThreeAxisLinearAccelerometers;
    }
    const SensorMeasurements& get_ThreeAxisMagnetometers() {
      return obj->ThreeAxisMagnetometers;
    }
    const SensorMeasurements& get_OrientationSensors() {
      return obj->OrientationSensors;
    }
    const SensorMeasurements& get_TemperatureSensors() {
      return obj->TemperatureSensors;
    }
    const SensorMeasurements& get_SixAxisForceTorqueSensors() {
      return obj->SixAxisForceTorqueSensors;
    }
    const SensorMeasurements& get_ContactLoadCellArrays() {
      return obj->ContactLoadCellArrays;
    }
    const SensorMeasurements& get_EncoderArrays() {
      return obj->EncoderArrays;
    }
    const SensorMeasurements& get_SkinPatches() {
      return obj->SkinPatches;
    }
    virtual bool will_set_ThreeAxisGyroscopes() { return true; }
    virtual bool will_set_ThreeAxisLinearAccelerometers() { return true; }
    virtual bool will_set_ThreeAxisMagnetometers() { return true; }
    virtual bool will_set_OrientationSensors() { return true; }
    virtual bool will_set_TemperatureSensors() { return true; }
    virtual bool will_set_SixAxisForceTorqueSensors() { return true; }
    virtual bool will_set_ContactLoadCellArrays() { return true; }
    virtual bool will_set_EncoderArrays() { return true; }
    virtual bool will_set_SkinPatches() { return true; }
    virtual bool did_set_ThreeAxisGyroscopes() { return true; }
    virtual bool did_set_ThreeAxisLinearAccelerometers() { return true; }
    virtual bool did_set_ThreeAxisMagnetometers() { return true; }
    virtual bool did_set_OrientationSensors() { return true; }
    virtual bool did_set_TemperatureSensors() { return true; }
    virtual bool did_set_SixAxisForceTorqueSensors() { return true; }
    virtual bool did_set_ContactLoadCellArrays() { return true; }
    virtual bool did_set_EncoderArrays() { return true; }
    virtual bool did_set_SkinPatches() { return true; }
    void clean() {
      dirty_flags(false);
    }
    bool read(yarp::os::ConnectionReader& connection) override;
    bool write(yarp::os::ConnectionWriter& connection) const override;
  private:

    SensorStreamingData *obj;

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
    void mark_dirty_ThreeAxisGyroscopes() {
      if (is_dirty_ThreeAxisGyroscopes) return;
      dirty_count++;
      is_dirty_ThreeAxisGyroscopes = true;
      mark_dirty();
    }
    void mark_dirty_ThreeAxisLinearAccelerometers() {
      if (is_dirty_ThreeAxisLinearAccelerometers) return;
      dirty_count++;
      is_dirty_ThreeAxisLinearAccelerometers = true;
      mark_dirty();
    }
    void mark_dirty_ThreeAxisMagnetometers() {
      if (is_dirty_ThreeAxisMagnetometers) return;
      dirty_count++;
      is_dirty_ThreeAxisMagnetometers = true;
      mark_dirty();
    }
    void mark_dirty_OrientationSensors() {
      if (is_dirty_OrientationSensors) return;
      dirty_count++;
      is_dirty_OrientationSensors = true;
      mark_dirty();
    }
    void mark_dirty_TemperatureSensors() {
      if (is_dirty_TemperatureSensors) return;
      dirty_count++;
      is_dirty_TemperatureSensors = true;
      mark_dirty();
    }
    void mark_dirty_SixAxisForceTorqueSensors() {
      if (is_dirty_SixAxisForceTorqueSensors) return;
      dirty_count++;
      is_dirty_SixAxisForceTorqueSensors = true;
      mark_dirty();
    }
    void mark_dirty_ContactLoadCellArrays() {
      if (is_dirty_ContactLoadCellArrays) return;
      dirty_count++;
      is_dirty_ContactLoadCellArrays = true;
      mark_dirty();
    }
    void mark_dirty_EncoderArrays() {
      if (is_dirty_EncoderArrays) return;
      dirty_count++;
      is_dirty_EncoderArrays = true;
      mark_dirty();
    }
    void mark_dirty_SkinPatches() {
      if (is_dirty_SkinPatches) return;
      dirty_count++;
      is_dirty_SkinPatches = true;
      mark_dirty();
    }
    void dirty_flags(bool flag) {
      is_dirty = flag;
      is_dirty_ThreeAxisGyroscopes = flag;
      is_dirty_ThreeAxisLinearAccelerometers = flag;
      is_dirty_ThreeAxisMagnetometers = flag;
      is_dirty_OrientationSensors = flag;
      is_dirty_TemperatureSensors = flag;
      is_dirty_SixAxisForceTorqueSensors = flag;
      is_dirty_ContactLoadCellArrays = flag;
      is_dirty_EncoderArrays = flag;
      is_dirty_SkinPatches = flag;
      dirty_count = flag ? 9 : 0;
    }
    bool is_dirty;
    int dirty_count;
    bool is_dirty_ThreeAxisGyroscopes;
    bool is_dirty_ThreeAxisLinearAccelerometers;
    bool is_dirty_ThreeAxisMagnetometers;
    bool is_dirty_OrientationSensors;
    bool is_dirty_TemperatureSensors;
    bool is_dirty_SixAxisForceTorqueSensors;
    bool is_dirty_ContactLoadCellArrays;
    bool is_dirty_EncoderArrays;
    bool is_dirty_SkinPatches;
  };
};

#endif

/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_STRUCT_audioBufferSizeData
#define YARP_THRIFT_GENERATOR_STRUCT_audioBufferSizeData

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>

namespace yarp {
  namespace dev {
    class audioBufferSizeData;
  }
}


class yarp::dev::audioBufferSizeData : public yarp::os::idl::WirePortable {
public:
  // Fields
  std::int32_t m_samples;
  std::int32_t m_channels;
  std::int32_t m_depth;
  std::int32_t size;

  // Default constructor
  audioBufferSizeData() : m_samples(0), m_channels(0), m_depth(0), size(0) {
  }

  // Constructor with field values
  audioBufferSizeData(const std::int32_t m_samples,const std::int32_t m_channels,const std::int32_t m_depth,const std::int32_t size) : m_samples(m_samples), m_channels(m_channels), m_depth(m_depth), size(size) {
  }

  // Copy constructor
  audioBufferSizeData(const audioBufferSizeData& __alt) : WirePortable(__alt)  {
    this->m_samples = __alt.m_samples;
    this->m_channels = __alt.m_channels;
    this->m_depth = __alt.m_depth;
    this->size = __alt.size;
  }

  // Assignment operator
  const audioBufferSizeData& operator = (const audioBufferSizeData& __alt) {
    this->m_samples = __alt.m_samples;
    this->m_channels = __alt.m_channels;
    this->m_depth = __alt.m_depth;
    this->size = __alt.size;
    return *this;
  }

  // read and write structure on a connection
  bool read(yarp::os::idl::WireReader& reader) override;
  bool read(yarp::os::ConnectionReader& connection) override;
  bool write(const yarp::os::idl::WireWriter& writer) const override;
  bool write(yarp::os::ConnectionWriter& connection) const override;

private:
  bool write_m_samples(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_m_samples(const yarp::os::idl::WireWriter& writer) const;
  bool write_m_channels(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_m_channels(const yarp::os::idl::WireWriter& writer) const;
  bool write_m_depth(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_m_depth(const yarp::os::idl::WireWriter& writer) const;
  bool write_size(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_size(const yarp::os::idl::WireWriter& writer) const;
  bool read_m_samples(yarp::os::idl::WireReader& reader);
  bool nested_read_m_samples(yarp::os::idl::WireReader& reader);
  bool read_m_channels(yarp::os::idl::WireReader& reader);
  bool nested_read_m_channels(yarp::os::idl::WireReader& reader);
  bool read_m_depth(yarp::os::idl::WireReader& reader);
  bool nested_read_m_depth(yarp::os::idl::WireReader& reader);
  bool read_size(yarp::os::idl::WireReader& reader);
  bool nested_read_size(yarp::os::idl::WireReader& reader);

public:

  std::string toString() const;

  // if you want to serialize this class without nesting, use this helper
  typedef yarp::os::idl::Unwrapped<yarp::dev::audioBufferSizeData > unwrapped;

  class Editor : public yarp::os::Wire, public yarp::os::PortWriter {
  public:

    Editor() {
      group = 0;
      obj_owned = true;
      obj = new audioBufferSizeData;
      dirty_flags(false);
      yarp().setOwner(*this);
    }

    Editor(audioBufferSizeData& obj) {
      group = 0;
      obj_owned = false;
      edit(obj,false);
      yarp().setOwner(*this);
    }

    bool edit(audioBufferSizeData& obj, bool dirty = true) {
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

    audioBufferSizeData& state() { return *obj; }

    void begin() { group++; }

    void end() {
      group--;
      if (group==0&&is_dirty) communicate();
    }
    void set_m_samples(const std::int32_t m_samples) {
      will_set_m_samples();
      obj->m_samples = m_samples;
      mark_dirty_m_samples();
      communicate();
      did_set_m_samples();
    }
    void set_m_channels(const std::int32_t m_channels) {
      will_set_m_channels();
      obj->m_channels = m_channels;
      mark_dirty_m_channels();
      communicate();
      did_set_m_channels();
    }
    void set_m_depth(const std::int32_t m_depth) {
      will_set_m_depth();
      obj->m_depth = m_depth;
      mark_dirty_m_depth();
      communicate();
      did_set_m_depth();
    }
    void set_size(const std::int32_t size) {
      will_set_size();
      obj->size = size;
      mark_dirty_size();
      communicate();
      did_set_size();
    }
    std::int32_t get_m_samples() {
      return obj->m_samples;
    }
    std::int32_t get_m_channels() {
      return obj->m_channels;
    }
    std::int32_t get_m_depth() {
      return obj->m_depth;
    }
    std::int32_t get_size() {
      return obj->size;
    }
    virtual bool will_set_m_samples() { return true; }
    virtual bool will_set_m_channels() { return true; }
    virtual bool will_set_m_depth() { return true; }
    virtual bool will_set_size() { return true; }
    virtual bool did_set_m_samples() { return true; }
    virtual bool did_set_m_channels() { return true; }
    virtual bool did_set_m_depth() { return true; }
    virtual bool did_set_size() { return true; }
    void clean() {
      dirty_flags(false);
    }
    bool read(yarp::os::ConnectionReader& connection) override;
    bool write(yarp::os::ConnectionWriter& connection) const override;
  private:

    audioBufferSizeData *obj;

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
    void mark_dirty_m_samples() {
      if (is_dirty_m_samples) return;
      dirty_count++;
      is_dirty_m_samples = true;
      mark_dirty();
    }
    void mark_dirty_m_channels() {
      if (is_dirty_m_channels) return;
      dirty_count++;
      is_dirty_m_channels = true;
      mark_dirty();
    }
    void mark_dirty_m_depth() {
      if (is_dirty_m_depth) return;
      dirty_count++;
      is_dirty_m_depth = true;
      mark_dirty();
    }
    void mark_dirty_size() {
      if (is_dirty_size) return;
      dirty_count++;
      is_dirty_size = true;
      mark_dirty();
    }
    void dirty_flags(bool flag) {
      is_dirty = flag;
      is_dirty_m_samples = flag;
      is_dirty_m_channels = flag;
      is_dirty_m_depth = flag;
      is_dirty_size = flag;
      dirty_count = flag ? 4 : 0;
    }
    bool is_dirty;
    int dirty_count;
    bool is_dirty_m_samples;
    bool is_dirty_m_channels;
    bool is_dirty_m_depth;
    bool is_dirty_size;
  };
};

#endif

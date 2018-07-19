/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_STRUCT_PointDLists
#define YARP_THRIFT_GENERATOR_STRUCT_PointDLists

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <firstInterface/PointD.h>

namespace yarp {
  namespace test {
    class PointDLists;
  }
}


class yarp::test::PointDLists : public yarp::os::idl::WirePortable {
public:
  // Fields
  std::string name;
  std::vector<PointD>  firstList;
  std::vector<PointD>  secondList;

  // Default constructor
  PointDLists() : name("pointLists") {
  }

  // Constructor with field values
  PointDLists(const std::string& name,const std::vector<PointD> & firstList,const std::vector<PointD> & secondList) : name(name), firstList(firstList), secondList(secondList) {
  }

  // Copy constructor
  PointDLists(const PointDLists& __alt) : WirePortable(__alt)  {
    this->name = __alt.name;
    this->firstList = __alt.firstList;
    this->secondList = __alt.secondList;
  }

  // Assignment operator
  const PointDLists& operator = (const PointDLists& __alt) {
    this->name = __alt.name;
    this->firstList = __alt.firstList;
    this->secondList = __alt.secondList;
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
  bool write_firstList(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_firstList(const yarp::os::idl::WireWriter& writer) const;
  bool write_secondList(const yarp::os::idl::WireWriter& writer) const;
  bool nested_write_secondList(const yarp::os::idl::WireWriter& writer) const;
  bool read_name(yarp::os::idl::WireReader& reader);
  bool nested_read_name(yarp::os::idl::WireReader& reader);
  bool read_firstList(yarp::os::idl::WireReader& reader);
  bool nested_read_firstList(yarp::os::idl::WireReader& reader);
  bool read_secondList(yarp::os::idl::WireReader& reader);
  bool nested_read_secondList(yarp::os::idl::WireReader& reader);

public:

  std::string toString() const;

  // if you want to serialize this class without nesting, use this helper
  typedef yarp::os::idl::Unwrapped<yarp::test::PointDLists > unwrapped;

  class Editor : public yarp::os::Wire, public yarp::os::PortWriter {
  public:

    Editor() {
      group = 0;
      obj_owned = true;
      obj = new PointDLists;
      dirty_flags(false);
      yarp().setOwner(*this);
    }

    Editor(PointDLists& obj) {
      group = 0;
      obj_owned = false;
      edit(obj,false);
      yarp().setOwner(*this);
    }

    bool edit(PointDLists& obj, bool dirty = true) {
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

    PointDLists& state() { return *obj; }

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
    void set_firstList(const std::vector<PointD> & firstList) {
      will_set_firstList();
      obj->firstList = firstList;
      mark_dirty_firstList();
      communicate();
      did_set_firstList();
    }
    void set_firstList(int index, const PointD& elem) {
      will_set_firstList();
      obj->firstList[index] = elem;
      mark_dirty_firstList();
      communicate();
      did_set_firstList();
    }
    void set_secondList(const std::vector<PointD> & secondList) {
      will_set_secondList();
      obj->secondList = secondList;
      mark_dirty_secondList();
      communicate();
      did_set_secondList();
    }
    void set_secondList(int index, const PointD& elem) {
      will_set_secondList();
      obj->secondList[index] = elem;
      mark_dirty_secondList();
      communicate();
      did_set_secondList();
    }
    const std::string& get_name() {
      return obj->name;
    }
    const std::vector<PointD> & get_firstList() {
      return obj->firstList;
    }
    const std::vector<PointD> & get_secondList() {
      return obj->secondList;
    }
    virtual bool will_set_name() { return true; }
    virtual bool will_set_firstList() { return true; }
    virtual bool will_set_secondList() { return true; }
    virtual bool did_set_name() { return true; }
    virtual bool did_set_firstList() { return true; }
    virtual bool did_set_secondList() { return true; }
    void clean() {
      dirty_flags(false);
    }
    bool read(yarp::os::ConnectionReader& connection) override;
    bool write(yarp::os::ConnectionWriter& connection) const override;
  private:

    PointDLists *obj;

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
    void mark_dirty_firstList() {
      if (is_dirty_firstList) return;
      dirty_count++;
      is_dirty_firstList = true;
      mark_dirty();
    }
    void mark_dirty_secondList() {
      if (is_dirty_secondList) return;
      dirty_count++;
      is_dirty_secondList = true;
      mark_dirty();
    }
    void dirty_flags(bool flag) {
      is_dirty = flag;
      is_dirty_name = flag;
      is_dirty_firstList = flag;
      is_dirty_secondList = flag;
      dirty_count = flag ? 3 : 0;
    }
    bool is_dirty;
    int dirty_count;
    bool is_dirty_name;
    bool is_dirty_firstList;
    bool is_dirty_secondList;
  };
};

#endif

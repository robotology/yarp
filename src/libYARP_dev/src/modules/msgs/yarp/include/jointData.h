// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_STRUCT_jointData
#define YARP_THRIFT_GENERATOR_STRUCT_jointData

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>

class jointData;


class jointData : public yarp::os::idl::WirePortable {
public:
  // Fields
  std::vector<double>  jointPosition;
  std::vector<double>  jointVelocity;
  std::vector<double>  jointAcceleration;
  std::vector<double>  motorPosition;
  std::vector<double>  motorVelocity;
  std::vector<double>  motorAcceleration;
  std::vector<double>  torque;
  std::vector<double>  pidOutput;
  std::vector<int32_t>  controlMode;
  std::vector<int32_t>  interactionMode;

  // Default constructor
  jointData() {
  }

  // Constructor with field values
  jointData(const std::vector<double> & jointPosition,const std::vector<double> & jointVelocity,const std::vector<double> & jointAcceleration,const std::vector<double> & motorPosition,const std::vector<double> & motorVelocity,const std::vector<double> & motorAcceleration,const std::vector<double> & torque,const std::vector<double> & pidOutput,const std::vector<int32_t> & controlMode,const std::vector<int32_t> & interactionMode) : jointPosition(jointPosition), jointVelocity(jointVelocity), jointAcceleration(jointAcceleration), motorPosition(motorPosition), motorVelocity(motorVelocity), motorAcceleration(motorAcceleration), torque(torque), pidOutput(pidOutput), controlMode(controlMode), interactionMode(interactionMode) {
  }

  // Copy constructor
  jointData(const jointData& __alt) : WirePortable(__alt)  {
    this->jointPosition = __alt.jointPosition;
    this->jointVelocity = __alt.jointVelocity;
    this->jointAcceleration = __alt.jointAcceleration;
    this->motorPosition = __alt.motorPosition;
    this->motorVelocity = __alt.motorVelocity;
    this->motorAcceleration = __alt.motorAcceleration;
    this->torque = __alt.torque;
    this->pidOutput = __alt.pidOutput;
    this->controlMode = __alt.controlMode;
    this->interactionMode = __alt.interactionMode;
  }

  // Assignment operator
  const jointData& operator = (const jointData& __alt) {
    this->jointPosition = __alt.jointPosition;
    this->jointVelocity = __alt.jointVelocity;
    this->jointAcceleration = __alt.jointAcceleration;
    this->motorPosition = __alt.motorPosition;
    this->motorVelocity = __alt.motorVelocity;
    this->motorAcceleration = __alt.motorAcceleration;
    this->torque = __alt.torque;
    this->pidOutput = __alt.pidOutput;
    this->controlMode = __alt.controlMode;
    this->interactionMode = __alt.interactionMode;
    return *this;
  }

  // read and write structure on a connection
  bool read(yarp::os::idl::WireReader& reader);
  bool read(yarp::os::ConnectionReader& connection);
  bool write(yarp::os::idl::WireWriter& writer);
  bool write(yarp::os::ConnectionWriter& connection);

private:
  bool write_jointPosition(yarp::os::idl::WireWriter& writer);
  bool nested_write_jointPosition(yarp::os::idl::WireWriter& writer);
  bool write_jointVelocity(yarp::os::idl::WireWriter& writer);
  bool nested_write_jointVelocity(yarp::os::idl::WireWriter& writer);
  bool write_jointAcceleration(yarp::os::idl::WireWriter& writer);
  bool nested_write_jointAcceleration(yarp::os::idl::WireWriter& writer);
  bool write_motorPosition(yarp::os::idl::WireWriter& writer);
  bool nested_write_motorPosition(yarp::os::idl::WireWriter& writer);
  bool write_motorVelocity(yarp::os::idl::WireWriter& writer);
  bool nested_write_motorVelocity(yarp::os::idl::WireWriter& writer);
  bool write_motorAcceleration(yarp::os::idl::WireWriter& writer);
  bool nested_write_motorAcceleration(yarp::os::idl::WireWriter& writer);
  bool write_torque(yarp::os::idl::WireWriter& writer);
  bool nested_write_torque(yarp::os::idl::WireWriter& writer);
  bool write_pidOutput(yarp::os::idl::WireWriter& writer);
  bool nested_write_pidOutput(yarp::os::idl::WireWriter& writer);
  bool write_controlMode(yarp::os::idl::WireWriter& writer);
  bool nested_write_controlMode(yarp::os::idl::WireWriter& writer);
  bool write_interactionMode(yarp::os::idl::WireWriter& writer);
  bool nested_write_interactionMode(yarp::os::idl::WireWriter& writer);
  bool read_jointPosition(yarp::os::idl::WireReader& reader);
  bool nested_read_jointPosition(yarp::os::idl::WireReader& reader);
  bool read_jointVelocity(yarp::os::idl::WireReader& reader);
  bool nested_read_jointVelocity(yarp::os::idl::WireReader& reader);
  bool read_jointAcceleration(yarp::os::idl::WireReader& reader);
  bool nested_read_jointAcceleration(yarp::os::idl::WireReader& reader);
  bool read_motorPosition(yarp::os::idl::WireReader& reader);
  bool nested_read_motorPosition(yarp::os::idl::WireReader& reader);
  bool read_motorVelocity(yarp::os::idl::WireReader& reader);
  bool nested_read_motorVelocity(yarp::os::idl::WireReader& reader);
  bool read_motorAcceleration(yarp::os::idl::WireReader& reader);
  bool nested_read_motorAcceleration(yarp::os::idl::WireReader& reader);
  bool read_torque(yarp::os::idl::WireReader& reader);
  bool nested_read_torque(yarp::os::idl::WireReader& reader);
  bool read_pidOutput(yarp::os::idl::WireReader& reader);
  bool nested_read_pidOutput(yarp::os::idl::WireReader& reader);
  bool read_controlMode(yarp::os::idl::WireReader& reader);
  bool nested_read_controlMode(yarp::os::idl::WireReader& reader);
  bool read_interactionMode(yarp::os::idl::WireReader& reader);
  bool nested_read_interactionMode(yarp::os::idl::WireReader& reader);

public:

  yarp::os::ConstString toString();

  // if you want to serialize this class without nesting, use this helper
  typedef yarp::os::idl::Unwrapped<jointData > unwrapped;

  class Editor : public yarp::os::Wire, public yarp::os::PortWriter {
  public:

    Editor() {
      group = 0;
      obj_owned = true;
      obj = new jointData;
      dirty_flags(false);
      yarp().setOwner(*this);
    }

    Editor(jointData& obj) {
      group = 0;
      obj_owned = false;
      edit(obj,false);
      yarp().setOwner(*this);
    }

    bool edit(jointData& obj, bool dirty = true) {
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

    jointData& state() { return *obj; }

    void begin() { group++; }

    void end() {
      group--;
      if (group==0&&is_dirty) communicate();
    }
    void set_jointPosition(const std::vector<double> & jointPosition) {
      will_set_jointPosition();
      obj->jointPosition = jointPosition;
      mark_dirty_jointPosition();
      communicate();
      did_set_jointPosition();
    }
    void set_jointPosition(int index, const double elem) {
      will_set_jointPosition();
      obj->jointPosition[index] = elem;
      mark_dirty_jointPosition();
      communicate();
      did_set_jointPosition();
    }
    void set_jointVelocity(const std::vector<double> & jointVelocity) {
      will_set_jointVelocity();
      obj->jointVelocity = jointVelocity;
      mark_dirty_jointVelocity();
      communicate();
      did_set_jointVelocity();
    }
    void set_jointVelocity(int index, const double elem) {
      will_set_jointVelocity();
      obj->jointVelocity[index] = elem;
      mark_dirty_jointVelocity();
      communicate();
      did_set_jointVelocity();
    }
    void set_jointAcceleration(const std::vector<double> & jointAcceleration) {
      will_set_jointAcceleration();
      obj->jointAcceleration = jointAcceleration;
      mark_dirty_jointAcceleration();
      communicate();
      did_set_jointAcceleration();
    }
    void set_jointAcceleration(int index, const double elem) {
      will_set_jointAcceleration();
      obj->jointAcceleration[index] = elem;
      mark_dirty_jointAcceleration();
      communicate();
      did_set_jointAcceleration();
    }
    void set_motorPosition(const std::vector<double> & motorPosition) {
      will_set_motorPosition();
      obj->motorPosition = motorPosition;
      mark_dirty_motorPosition();
      communicate();
      did_set_motorPosition();
    }
    void set_motorPosition(int index, const double elem) {
      will_set_motorPosition();
      obj->motorPosition[index] = elem;
      mark_dirty_motorPosition();
      communicate();
      did_set_motorPosition();
    }
    void set_motorVelocity(const std::vector<double> & motorVelocity) {
      will_set_motorVelocity();
      obj->motorVelocity = motorVelocity;
      mark_dirty_motorVelocity();
      communicate();
      did_set_motorVelocity();
    }
    void set_motorVelocity(int index, const double elem) {
      will_set_motorVelocity();
      obj->motorVelocity[index] = elem;
      mark_dirty_motorVelocity();
      communicate();
      did_set_motorVelocity();
    }
    void set_motorAcceleration(const std::vector<double> & motorAcceleration) {
      will_set_motorAcceleration();
      obj->motorAcceleration = motorAcceleration;
      mark_dirty_motorAcceleration();
      communicate();
      did_set_motorAcceleration();
    }
    void set_motorAcceleration(int index, const double elem) {
      will_set_motorAcceleration();
      obj->motorAcceleration[index] = elem;
      mark_dirty_motorAcceleration();
      communicate();
      did_set_motorAcceleration();
    }
    void set_torque(const std::vector<double> & torque) {
      will_set_torque();
      obj->torque = torque;
      mark_dirty_torque();
      communicate();
      did_set_torque();
    }
    void set_torque(int index, const double elem) {
      will_set_torque();
      obj->torque[index] = elem;
      mark_dirty_torque();
      communicate();
      did_set_torque();
    }
    void set_pidOutput(const std::vector<double> & pidOutput) {
      will_set_pidOutput();
      obj->pidOutput = pidOutput;
      mark_dirty_pidOutput();
      communicate();
      did_set_pidOutput();
    }
    void set_pidOutput(int index, const double elem) {
      will_set_pidOutput();
      obj->pidOutput[index] = elem;
      mark_dirty_pidOutput();
      communicate();
      did_set_pidOutput();
    }
    void set_controlMode(const std::vector<int32_t> & controlMode) {
      will_set_controlMode();
      obj->controlMode = controlMode;
      mark_dirty_controlMode();
      communicate();
      did_set_controlMode();
    }
    void set_controlMode(int index, const int32_t elem) {
      will_set_controlMode();
      obj->controlMode[index] = elem;
      mark_dirty_controlMode();
      communicate();
      did_set_controlMode();
    }
    void set_interactionMode(const std::vector<int32_t> & interactionMode) {
      will_set_interactionMode();
      obj->interactionMode = interactionMode;
      mark_dirty_interactionMode();
      communicate();
      did_set_interactionMode();
    }
    void set_interactionMode(int index, const int32_t elem) {
      will_set_interactionMode();
      obj->interactionMode[index] = elem;
      mark_dirty_interactionMode();
      communicate();
      did_set_interactionMode();
    }
    const std::vector<double> & get_jointPosition() {
      return obj->jointPosition;
    }
    const std::vector<double> & get_jointVelocity() {
      return obj->jointVelocity;
    }
    const std::vector<double> & get_jointAcceleration() {
      return obj->jointAcceleration;
    }
    const std::vector<double> & get_motorPosition() {
      return obj->motorPosition;
    }
    const std::vector<double> & get_motorVelocity() {
      return obj->motorVelocity;
    }
    const std::vector<double> & get_motorAcceleration() {
      return obj->motorAcceleration;
    }
    const std::vector<double> & get_torque() {
      return obj->torque;
    }
    const std::vector<double> & get_pidOutput() {
      return obj->pidOutput;
    }
    const std::vector<int32_t> & get_controlMode() {
      return obj->controlMode;
    }
    const std::vector<int32_t> & get_interactionMode() {
      return obj->interactionMode;
    }
    virtual bool will_set_jointPosition() { return true; }
    virtual bool will_set_jointVelocity() { return true; }
    virtual bool will_set_jointAcceleration() { return true; }
    virtual bool will_set_motorPosition() { return true; }
    virtual bool will_set_motorVelocity() { return true; }
    virtual bool will_set_motorAcceleration() { return true; }
    virtual bool will_set_torque() { return true; }
    virtual bool will_set_pidOutput() { return true; }
    virtual bool will_set_controlMode() { return true; }
    virtual bool will_set_interactionMode() { return true; }
    virtual bool did_set_jointPosition() { return true; }
    virtual bool did_set_jointVelocity() { return true; }
    virtual bool did_set_jointAcceleration() { return true; }
    virtual bool did_set_motorPosition() { return true; }
    virtual bool did_set_motorVelocity() { return true; }
    virtual bool did_set_motorAcceleration() { return true; }
    virtual bool did_set_torque() { return true; }
    virtual bool did_set_pidOutput() { return true; }
    virtual bool did_set_controlMode() { return true; }
    virtual bool did_set_interactionMode() { return true; }
    void clean() {
      dirty_flags(false);
    }
    bool read(yarp::os::ConnectionReader& connection);
    bool write(yarp::os::ConnectionWriter& connection);
  private:

    jointData *obj;

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
    void mark_dirty_jointPosition() {
      if (is_dirty_jointPosition) return;
      dirty_count++;
      is_dirty_jointPosition = true;
      mark_dirty();
    }
    void mark_dirty_jointVelocity() {
      if (is_dirty_jointVelocity) return;
      dirty_count++;
      is_dirty_jointVelocity = true;
      mark_dirty();
    }
    void mark_dirty_jointAcceleration() {
      if (is_dirty_jointAcceleration) return;
      dirty_count++;
      is_dirty_jointAcceleration = true;
      mark_dirty();
    }
    void mark_dirty_motorPosition() {
      if (is_dirty_motorPosition) return;
      dirty_count++;
      is_dirty_motorPosition = true;
      mark_dirty();
    }
    void mark_dirty_motorVelocity() {
      if (is_dirty_motorVelocity) return;
      dirty_count++;
      is_dirty_motorVelocity = true;
      mark_dirty();
    }
    void mark_dirty_motorAcceleration() {
      if (is_dirty_motorAcceleration) return;
      dirty_count++;
      is_dirty_motorAcceleration = true;
      mark_dirty();
    }
    void mark_dirty_torque() {
      if (is_dirty_torque) return;
      dirty_count++;
      is_dirty_torque = true;
      mark_dirty();
    }
    void mark_dirty_pidOutput() {
      if (is_dirty_pidOutput) return;
      dirty_count++;
      is_dirty_pidOutput = true;
      mark_dirty();
    }
    void mark_dirty_controlMode() {
      if (is_dirty_controlMode) return;
      dirty_count++;
      is_dirty_controlMode = true;
      mark_dirty();
    }
    void mark_dirty_interactionMode() {
      if (is_dirty_interactionMode) return;
      dirty_count++;
      is_dirty_interactionMode = true;
      mark_dirty();
    }
    void dirty_flags(bool flag) {
      is_dirty = flag;
      is_dirty_jointPosition = flag;
      is_dirty_jointVelocity = flag;
      is_dirty_jointAcceleration = flag;
      is_dirty_motorPosition = flag;
      is_dirty_motorVelocity = flag;
      is_dirty_motorAcceleration = flag;
      is_dirty_torque = flag;
      is_dirty_pidOutput = flag;
      is_dirty_controlMode = flag;
      is_dirty_interactionMode = flag;
      dirty_count = flag ? 10 : 0;
    }
    bool is_dirty;
    int dirty_count;
    bool is_dirty_jointPosition;
    bool is_dirty_jointVelocity;
    bool is_dirty_jointAcceleration;
    bool is_dirty_motorPosition;
    bool is_dirty_motorVelocity;
    bool is_dirty_motorAcceleration;
    bool is_dirty_torque;
    bool is_dirty_pidOutput;
    bool is_dirty_controlMode;
    bool is_dirty_interactionMode;
  };
};

#endif


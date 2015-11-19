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
  bool jointPosition_isValid;
  std::vector<double>  jointVelocity;
  bool jointVelocity_isValid;
  std::vector<double>  jointAcceleration;
  bool jointAcceleration_isValid;
  std::vector<double>  motorPosition;
  bool motorPosition_isValid;
  std::vector<double>  motorVelocity;
  bool motorVelocity_isValid;
  std::vector<double>  motorAcceleration;
  bool motorAcceleration_isValid;
  std::vector<double>  torque;
  bool torque_isValid;
  std::vector<double>  pidOutput;
  bool pidOutput_isValid;
  std::vector<int32_t>  controlMode;
  bool controlMode_isValid;
  std::vector<int32_t>  interactionMode;
  bool interactionMode_isValid;

  // Default constructor
  jointData() : jointPosition_isValid(0), jointVelocity_isValid(0), jointAcceleration_isValid(0), motorPosition_isValid(0), motorVelocity_isValid(0), motorAcceleration_isValid(0), torque_isValid(0), pidOutput_isValid(0), controlMode_isValid(0), interactionMode_isValid(0) {
  }

  // Constructor with field values
  jointData(const std::vector<double> & jointPosition,const bool jointPosition_isValid,const std::vector<double> & jointVelocity,const bool jointVelocity_isValid,const std::vector<double> & jointAcceleration,const bool jointAcceleration_isValid,const std::vector<double> & motorPosition,const bool motorPosition_isValid,const std::vector<double> & motorVelocity,const bool motorVelocity_isValid,const std::vector<double> & motorAcceleration,const bool motorAcceleration_isValid,const std::vector<double> & torque,const bool torque_isValid,const std::vector<double> & pidOutput,const bool pidOutput_isValid,const std::vector<int32_t> & controlMode,const bool controlMode_isValid,const std::vector<int32_t> & interactionMode,const bool interactionMode_isValid) : jointPosition(jointPosition), jointPosition_isValid(jointPosition_isValid), jointVelocity(jointVelocity), jointVelocity_isValid(jointVelocity_isValid), jointAcceleration(jointAcceleration), jointAcceleration_isValid(jointAcceleration_isValid), motorPosition(motorPosition), motorPosition_isValid(motorPosition_isValid), motorVelocity(motorVelocity), motorVelocity_isValid(motorVelocity_isValid), motorAcceleration(motorAcceleration), motorAcceleration_isValid(motorAcceleration_isValid), torque(torque), torque_isValid(torque_isValid), pidOutput(pidOutput), pidOutput_isValid(pidOutput_isValid), controlMode(controlMode), controlMode_isValid(controlMode_isValid), interactionMode(interactionMode), interactionMode_isValid(interactionMode_isValid) {
  }

  // Copy constructor
  jointData(const jointData& __alt) : WirePortable(__alt)  {
    this->jointPosition = __alt.jointPosition;
    this->jointPosition_isValid = __alt.jointPosition_isValid;
    this->jointVelocity = __alt.jointVelocity;
    this->jointVelocity_isValid = __alt.jointVelocity_isValid;
    this->jointAcceleration = __alt.jointAcceleration;
    this->jointAcceleration_isValid = __alt.jointAcceleration_isValid;
    this->motorPosition = __alt.motorPosition;
    this->motorPosition_isValid = __alt.motorPosition_isValid;
    this->motorVelocity = __alt.motorVelocity;
    this->motorVelocity_isValid = __alt.motorVelocity_isValid;
    this->motorAcceleration = __alt.motorAcceleration;
    this->motorAcceleration_isValid = __alt.motorAcceleration_isValid;
    this->torque = __alt.torque;
    this->torque_isValid = __alt.torque_isValid;
    this->pidOutput = __alt.pidOutput;
    this->pidOutput_isValid = __alt.pidOutput_isValid;
    this->controlMode = __alt.controlMode;
    this->controlMode_isValid = __alt.controlMode_isValid;
    this->interactionMode = __alt.interactionMode;
    this->interactionMode_isValid = __alt.interactionMode_isValid;
  }

  // Assignment operator
  const jointData& operator = (const jointData& __alt) {
    this->jointPosition = __alt.jointPosition;
    this->jointPosition_isValid = __alt.jointPosition_isValid;
    this->jointVelocity = __alt.jointVelocity;
    this->jointVelocity_isValid = __alt.jointVelocity_isValid;
    this->jointAcceleration = __alt.jointAcceleration;
    this->jointAcceleration_isValid = __alt.jointAcceleration_isValid;
    this->motorPosition = __alt.motorPosition;
    this->motorPosition_isValid = __alt.motorPosition_isValid;
    this->motorVelocity = __alt.motorVelocity;
    this->motorVelocity_isValid = __alt.motorVelocity_isValid;
    this->motorAcceleration = __alt.motorAcceleration;
    this->motorAcceleration_isValid = __alt.motorAcceleration_isValid;
    this->torque = __alt.torque;
    this->torque_isValid = __alt.torque_isValid;
    this->pidOutput = __alt.pidOutput;
    this->pidOutput_isValid = __alt.pidOutput_isValid;
    this->controlMode = __alt.controlMode;
    this->controlMode_isValid = __alt.controlMode_isValid;
    this->interactionMode = __alt.interactionMode;
    this->interactionMode_isValid = __alt.interactionMode_isValid;
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
  bool write_jointPosition_isValid(yarp::os::idl::WireWriter& writer);
  bool nested_write_jointPosition_isValid(yarp::os::idl::WireWriter& writer);
  bool write_jointVelocity(yarp::os::idl::WireWriter& writer);
  bool nested_write_jointVelocity(yarp::os::idl::WireWriter& writer);
  bool write_jointVelocity_isValid(yarp::os::idl::WireWriter& writer);
  bool nested_write_jointVelocity_isValid(yarp::os::idl::WireWriter& writer);
  bool write_jointAcceleration(yarp::os::idl::WireWriter& writer);
  bool nested_write_jointAcceleration(yarp::os::idl::WireWriter& writer);
  bool write_jointAcceleration_isValid(yarp::os::idl::WireWriter& writer);
  bool nested_write_jointAcceleration_isValid(yarp::os::idl::WireWriter& writer);
  bool write_motorPosition(yarp::os::idl::WireWriter& writer);
  bool nested_write_motorPosition(yarp::os::idl::WireWriter& writer);
  bool write_motorPosition_isValid(yarp::os::idl::WireWriter& writer);
  bool nested_write_motorPosition_isValid(yarp::os::idl::WireWriter& writer);
  bool write_motorVelocity(yarp::os::idl::WireWriter& writer);
  bool nested_write_motorVelocity(yarp::os::idl::WireWriter& writer);
  bool write_motorVelocity_isValid(yarp::os::idl::WireWriter& writer);
  bool nested_write_motorVelocity_isValid(yarp::os::idl::WireWriter& writer);
  bool write_motorAcceleration(yarp::os::idl::WireWriter& writer);
  bool nested_write_motorAcceleration(yarp::os::idl::WireWriter& writer);
  bool write_motorAcceleration_isValid(yarp::os::idl::WireWriter& writer);
  bool nested_write_motorAcceleration_isValid(yarp::os::idl::WireWriter& writer);
  bool write_torque(yarp::os::idl::WireWriter& writer);
  bool nested_write_torque(yarp::os::idl::WireWriter& writer);
  bool write_torque_isValid(yarp::os::idl::WireWriter& writer);
  bool nested_write_torque_isValid(yarp::os::idl::WireWriter& writer);
  bool write_pidOutput(yarp::os::idl::WireWriter& writer);
  bool nested_write_pidOutput(yarp::os::idl::WireWriter& writer);
  bool write_pidOutput_isValid(yarp::os::idl::WireWriter& writer);
  bool nested_write_pidOutput_isValid(yarp::os::idl::WireWriter& writer);
  bool write_controlMode(yarp::os::idl::WireWriter& writer);
  bool nested_write_controlMode(yarp::os::idl::WireWriter& writer);
  bool write_controlMode_isValid(yarp::os::idl::WireWriter& writer);
  bool nested_write_controlMode_isValid(yarp::os::idl::WireWriter& writer);
  bool write_interactionMode(yarp::os::idl::WireWriter& writer);
  bool nested_write_interactionMode(yarp::os::idl::WireWriter& writer);
  bool write_interactionMode_isValid(yarp::os::idl::WireWriter& writer);
  bool nested_write_interactionMode_isValid(yarp::os::idl::WireWriter& writer);
  bool read_jointPosition(yarp::os::idl::WireReader& reader);
  bool nested_read_jointPosition(yarp::os::idl::WireReader& reader);
  bool read_jointPosition_isValid(yarp::os::idl::WireReader& reader);
  bool nested_read_jointPosition_isValid(yarp::os::idl::WireReader& reader);
  bool read_jointVelocity(yarp::os::idl::WireReader& reader);
  bool nested_read_jointVelocity(yarp::os::idl::WireReader& reader);
  bool read_jointVelocity_isValid(yarp::os::idl::WireReader& reader);
  bool nested_read_jointVelocity_isValid(yarp::os::idl::WireReader& reader);
  bool read_jointAcceleration(yarp::os::idl::WireReader& reader);
  bool nested_read_jointAcceleration(yarp::os::idl::WireReader& reader);
  bool read_jointAcceleration_isValid(yarp::os::idl::WireReader& reader);
  bool nested_read_jointAcceleration_isValid(yarp::os::idl::WireReader& reader);
  bool read_motorPosition(yarp::os::idl::WireReader& reader);
  bool nested_read_motorPosition(yarp::os::idl::WireReader& reader);
  bool read_motorPosition_isValid(yarp::os::idl::WireReader& reader);
  bool nested_read_motorPosition_isValid(yarp::os::idl::WireReader& reader);
  bool read_motorVelocity(yarp::os::idl::WireReader& reader);
  bool nested_read_motorVelocity(yarp::os::idl::WireReader& reader);
  bool read_motorVelocity_isValid(yarp::os::idl::WireReader& reader);
  bool nested_read_motorVelocity_isValid(yarp::os::idl::WireReader& reader);
  bool read_motorAcceleration(yarp::os::idl::WireReader& reader);
  bool nested_read_motorAcceleration(yarp::os::idl::WireReader& reader);
  bool read_motorAcceleration_isValid(yarp::os::idl::WireReader& reader);
  bool nested_read_motorAcceleration_isValid(yarp::os::idl::WireReader& reader);
  bool read_torque(yarp::os::idl::WireReader& reader);
  bool nested_read_torque(yarp::os::idl::WireReader& reader);
  bool read_torque_isValid(yarp::os::idl::WireReader& reader);
  bool nested_read_torque_isValid(yarp::os::idl::WireReader& reader);
  bool read_pidOutput(yarp::os::idl::WireReader& reader);
  bool nested_read_pidOutput(yarp::os::idl::WireReader& reader);
  bool read_pidOutput_isValid(yarp::os::idl::WireReader& reader);
  bool nested_read_pidOutput_isValid(yarp::os::idl::WireReader& reader);
  bool read_controlMode(yarp::os::idl::WireReader& reader);
  bool nested_read_controlMode(yarp::os::idl::WireReader& reader);
  bool read_controlMode_isValid(yarp::os::idl::WireReader& reader);
  bool nested_read_controlMode_isValid(yarp::os::idl::WireReader& reader);
  bool read_interactionMode(yarp::os::idl::WireReader& reader);
  bool nested_read_interactionMode(yarp::os::idl::WireReader& reader);
  bool read_interactionMode_isValid(yarp::os::idl::WireReader& reader);
  bool nested_read_interactionMode_isValid(yarp::os::idl::WireReader& reader);

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
    void set_jointPosition_isValid(const bool jointPosition_isValid) {
      will_set_jointPosition_isValid();
      obj->jointPosition_isValid = jointPosition_isValid;
      mark_dirty_jointPosition_isValid();
      communicate();
      did_set_jointPosition_isValid();
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
    void set_jointVelocity_isValid(const bool jointVelocity_isValid) {
      will_set_jointVelocity_isValid();
      obj->jointVelocity_isValid = jointVelocity_isValid;
      mark_dirty_jointVelocity_isValid();
      communicate();
      did_set_jointVelocity_isValid();
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
    void set_jointAcceleration_isValid(const bool jointAcceleration_isValid) {
      will_set_jointAcceleration_isValid();
      obj->jointAcceleration_isValid = jointAcceleration_isValid;
      mark_dirty_jointAcceleration_isValid();
      communicate();
      did_set_jointAcceleration_isValid();
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
    void set_motorPosition_isValid(const bool motorPosition_isValid) {
      will_set_motorPosition_isValid();
      obj->motorPosition_isValid = motorPosition_isValid;
      mark_dirty_motorPosition_isValid();
      communicate();
      did_set_motorPosition_isValid();
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
    void set_motorVelocity_isValid(const bool motorVelocity_isValid) {
      will_set_motorVelocity_isValid();
      obj->motorVelocity_isValid = motorVelocity_isValid;
      mark_dirty_motorVelocity_isValid();
      communicate();
      did_set_motorVelocity_isValid();
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
    void set_motorAcceleration_isValid(const bool motorAcceleration_isValid) {
      will_set_motorAcceleration_isValid();
      obj->motorAcceleration_isValid = motorAcceleration_isValid;
      mark_dirty_motorAcceleration_isValid();
      communicate();
      did_set_motorAcceleration_isValid();
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
    void set_torque_isValid(const bool torque_isValid) {
      will_set_torque_isValid();
      obj->torque_isValid = torque_isValid;
      mark_dirty_torque_isValid();
      communicate();
      did_set_torque_isValid();
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
    void set_pidOutput_isValid(const bool pidOutput_isValid) {
      will_set_pidOutput_isValid();
      obj->pidOutput_isValid = pidOutput_isValid;
      mark_dirty_pidOutput_isValid();
      communicate();
      did_set_pidOutput_isValid();
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
    void set_controlMode_isValid(const bool controlMode_isValid) {
      will_set_controlMode_isValid();
      obj->controlMode_isValid = controlMode_isValid;
      mark_dirty_controlMode_isValid();
      communicate();
      did_set_controlMode_isValid();
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
    void set_interactionMode_isValid(const bool interactionMode_isValid) {
      will_set_interactionMode_isValid();
      obj->interactionMode_isValid = interactionMode_isValid;
      mark_dirty_interactionMode_isValid();
      communicate();
      did_set_interactionMode_isValid();
    }
    const std::vector<double> & get_jointPosition() {
      return obj->jointPosition;
    }
    bool get_jointPosition_isValid() {
      return obj->jointPosition_isValid;
    }
    const std::vector<double> & get_jointVelocity() {
      return obj->jointVelocity;
    }
    bool get_jointVelocity_isValid() {
      return obj->jointVelocity_isValid;
    }
    const std::vector<double> & get_jointAcceleration() {
      return obj->jointAcceleration;
    }
    bool get_jointAcceleration_isValid() {
      return obj->jointAcceleration_isValid;
    }
    const std::vector<double> & get_motorPosition() {
      return obj->motorPosition;
    }
    bool get_motorPosition_isValid() {
      return obj->motorPosition_isValid;
    }
    const std::vector<double> & get_motorVelocity() {
      return obj->motorVelocity;
    }
    bool get_motorVelocity_isValid() {
      return obj->motorVelocity_isValid;
    }
    const std::vector<double> & get_motorAcceleration() {
      return obj->motorAcceleration;
    }
    bool get_motorAcceleration_isValid() {
      return obj->motorAcceleration_isValid;
    }
    const std::vector<double> & get_torque() {
      return obj->torque;
    }
    bool get_torque_isValid() {
      return obj->torque_isValid;
    }
    const std::vector<double> & get_pidOutput() {
      return obj->pidOutput;
    }
    bool get_pidOutput_isValid() {
      return obj->pidOutput_isValid;
    }
    const std::vector<int32_t> & get_controlMode() {
      return obj->controlMode;
    }
    bool get_controlMode_isValid() {
      return obj->controlMode_isValid;
    }
    const std::vector<int32_t> & get_interactionMode() {
      return obj->interactionMode;
    }
    bool get_interactionMode_isValid() {
      return obj->interactionMode_isValid;
    }
    virtual bool will_set_jointPosition() { return true; }
    virtual bool will_set_jointPosition_isValid() { return true; }
    virtual bool will_set_jointVelocity() { return true; }
    virtual bool will_set_jointVelocity_isValid() { return true; }
    virtual bool will_set_jointAcceleration() { return true; }
    virtual bool will_set_jointAcceleration_isValid() { return true; }
    virtual bool will_set_motorPosition() { return true; }
    virtual bool will_set_motorPosition_isValid() { return true; }
    virtual bool will_set_motorVelocity() { return true; }
    virtual bool will_set_motorVelocity_isValid() { return true; }
    virtual bool will_set_motorAcceleration() { return true; }
    virtual bool will_set_motorAcceleration_isValid() { return true; }
    virtual bool will_set_torque() { return true; }
    virtual bool will_set_torque_isValid() { return true; }
    virtual bool will_set_pidOutput() { return true; }
    virtual bool will_set_pidOutput_isValid() { return true; }
    virtual bool will_set_controlMode() { return true; }
    virtual bool will_set_controlMode_isValid() { return true; }
    virtual bool will_set_interactionMode() { return true; }
    virtual bool will_set_interactionMode_isValid() { return true; }
    virtual bool did_set_jointPosition() { return true; }
    virtual bool did_set_jointPosition_isValid() { return true; }
    virtual bool did_set_jointVelocity() { return true; }
    virtual bool did_set_jointVelocity_isValid() { return true; }
    virtual bool did_set_jointAcceleration() { return true; }
    virtual bool did_set_jointAcceleration_isValid() { return true; }
    virtual bool did_set_motorPosition() { return true; }
    virtual bool did_set_motorPosition_isValid() { return true; }
    virtual bool did_set_motorVelocity() { return true; }
    virtual bool did_set_motorVelocity_isValid() { return true; }
    virtual bool did_set_motorAcceleration() { return true; }
    virtual bool did_set_motorAcceleration_isValid() { return true; }
    virtual bool did_set_torque() { return true; }
    virtual bool did_set_torque_isValid() { return true; }
    virtual bool did_set_pidOutput() { return true; }
    virtual bool did_set_pidOutput_isValid() { return true; }
    virtual bool did_set_controlMode() { return true; }
    virtual bool did_set_controlMode_isValid() { return true; }
    virtual bool did_set_interactionMode() { return true; }
    virtual bool did_set_interactionMode_isValid() { return true; }
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
    void mark_dirty_jointPosition_isValid() {
      if (is_dirty_jointPosition_isValid) return;
      dirty_count++;
      is_dirty_jointPosition_isValid = true;
      mark_dirty();
    }
    void mark_dirty_jointVelocity() {
      if (is_dirty_jointVelocity) return;
      dirty_count++;
      is_dirty_jointVelocity = true;
      mark_dirty();
    }
    void mark_dirty_jointVelocity_isValid() {
      if (is_dirty_jointVelocity_isValid) return;
      dirty_count++;
      is_dirty_jointVelocity_isValid = true;
      mark_dirty();
    }
    void mark_dirty_jointAcceleration() {
      if (is_dirty_jointAcceleration) return;
      dirty_count++;
      is_dirty_jointAcceleration = true;
      mark_dirty();
    }
    void mark_dirty_jointAcceleration_isValid() {
      if (is_dirty_jointAcceleration_isValid) return;
      dirty_count++;
      is_dirty_jointAcceleration_isValid = true;
      mark_dirty();
    }
    void mark_dirty_motorPosition() {
      if (is_dirty_motorPosition) return;
      dirty_count++;
      is_dirty_motorPosition = true;
      mark_dirty();
    }
    void mark_dirty_motorPosition_isValid() {
      if (is_dirty_motorPosition_isValid) return;
      dirty_count++;
      is_dirty_motorPosition_isValid = true;
      mark_dirty();
    }
    void mark_dirty_motorVelocity() {
      if (is_dirty_motorVelocity) return;
      dirty_count++;
      is_dirty_motorVelocity = true;
      mark_dirty();
    }
    void mark_dirty_motorVelocity_isValid() {
      if (is_dirty_motorVelocity_isValid) return;
      dirty_count++;
      is_dirty_motorVelocity_isValid = true;
      mark_dirty();
    }
    void mark_dirty_motorAcceleration() {
      if (is_dirty_motorAcceleration) return;
      dirty_count++;
      is_dirty_motorAcceleration = true;
      mark_dirty();
    }
    void mark_dirty_motorAcceleration_isValid() {
      if (is_dirty_motorAcceleration_isValid) return;
      dirty_count++;
      is_dirty_motorAcceleration_isValid = true;
      mark_dirty();
    }
    void mark_dirty_torque() {
      if (is_dirty_torque) return;
      dirty_count++;
      is_dirty_torque = true;
      mark_dirty();
    }
    void mark_dirty_torque_isValid() {
      if (is_dirty_torque_isValid) return;
      dirty_count++;
      is_dirty_torque_isValid = true;
      mark_dirty();
    }
    void mark_dirty_pidOutput() {
      if (is_dirty_pidOutput) return;
      dirty_count++;
      is_dirty_pidOutput = true;
      mark_dirty();
    }
    void mark_dirty_pidOutput_isValid() {
      if (is_dirty_pidOutput_isValid) return;
      dirty_count++;
      is_dirty_pidOutput_isValid = true;
      mark_dirty();
    }
    void mark_dirty_controlMode() {
      if (is_dirty_controlMode) return;
      dirty_count++;
      is_dirty_controlMode = true;
      mark_dirty();
    }
    void mark_dirty_controlMode_isValid() {
      if (is_dirty_controlMode_isValid) return;
      dirty_count++;
      is_dirty_controlMode_isValid = true;
      mark_dirty();
    }
    void mark_dirty_interactionMode() {
      if (is_dirty_interactionMode) return;
      dirty_count++;
      is_dirty_interactionMode = true;
      mark_dirty();
    }
    void mark_dirty_interactionMode_isValid() {
      if (is_dirty_interactionMode_isValid) return;
      dirty_count++;
      is_dirty_interactionMode_isValid = true;
      mark_dirty();
    }
    void dirty_flags(bool flag) {
      is_dirty = flag;
      is_dirty_jointPosition = flag;
      is_dirty_jointPosition_isValid = flag;
      is_dirty_jointVelocity = flag;
      is_dirty_jointVelocity_isValid = flag;
      is_dirty_jointAcceleration = flag;
      is_dirty_jointAcceleration_isValid = flag;
      is_dirty_motorPosition = flag;
      is_dirty_motorPosition_isValid = flag;
      is_dirty_motorVelocity = flag;
      is_dirty_motorVelocity_isValid = flag;
      is_dirty_motorAcceleration = flag;
      is_dirty_motorAcceleration_isValid = flag;
      is_dirty_torque = flag;
      is_dirty_torque_isValid = flag;
      is_dirty_pidOutput = flag;
      is_dirty_pidOutput_isValid = flag;
      is_dirty_controlMode = flag;
      is_dirty_controlMode_isValid = flag;
      is_dirty_interactionMode = flag;
      is_dirty_interactionMode_isValid = flag;
      dirty_count = flag ? 20 : 0;
    }
    bool is_dirty;
    int dirty_count;
    bool is_dirty_jointPosition;
    bool is_dirty_jointPosition_isValid;
    bool is_dirty_jointVelocity;
    bool is_dirty_jointVelocity_isValid;
    bool is_dirty_jointAcceleration;
    bool is_dirty_jointAcceleration_isValid;
    bool is_dirty_motorPosition;
    bool is_dirty_motorPosition_isValid;
    bool is_dirty_motorVelocity;
    bool is_dirty_motorVelocity_isValid;
    bool is_dirty_motorAcceleration;
    bool is_dirty_motorAcceleration_isValid;
    bool is_dirty_torque;
    bool is_dirty_torque_isValid;
    bool is_dirty_pidOutput;
    bool is_dirty_pidOutput_isValid;
    bool is_dirty_controlMode;
    bool is_dirty_controlMode_isValid;
    bool is_dirty_interactionMode;
    bool is_dirty_interactionMode_isValid;
  };
};

#endif

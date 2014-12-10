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
  std::vector<double>  position;
  std::vector<double>  velocity;
  std::vector<double>  acceleration;
  std::vector<double>  torque;
  std::vector<double>  pidOutput;
  std::vector<int32_t>  controlMode;
  std::vector<int32_t>  interactionMode;
  
  // Default constructor
  jointData() {
  }
  
  // Constructor with field values
  jointData(const std::vector<double> & position,const std::vector<double> & velocity,const std::vector<double> & acceleration,const std::vector<double> & torque,const std::vector<double> & pidOutput,const std::vector<int32_t> & controlMode,const std::vector<int32_t> & interactionMode) : position(position), velocity(velocity), acceleration(acceleration), torque(torque), pidOutput(pidOutput), controlMode(controlMode), interactionMode(interactionMode) {
  }
  
  // Copy constructor
  jointData(const jointData& __alt) {
    this->position = __alt.position;
    this->velocity = __alt.velocity;
    this->acceleration = __alt.acceleration;
    this->torque = __alt.torque;
    this->pidOutput = __alt.pidOutput;
    this->controlMode = __alt.controlMode;
    this->interactionMode = __alt.interactionMode;
  }
  
  // Assignment operator
  const jointData& operator = (const jointData& __alt) {
    this->position = __alt.position;
    this->velocity = __alt.velocity;
    this->acceleration = __alt.acceleration;
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
  bool write_position(yarp::os::idl::WireWriter& writer);
  bool nested_write_position(yarp::os::idl::WireWriter& writer);
  bool write_velocity(yarp::os::idl::WireWriter& writer);
  bool nested_write_velocity(yarp::os::idl::WireWriter& writer);
  bool write_acceleration(yarp::os::idl::WireWriter& writer);
  bool nested_write_acceleration(yarp::os::idl::WireWriter& writer);
  bool write_torque(yarp::os::idl::WireWriter& writer);
  bool nested_write_torque(yarp::os::idl::WireWriter& writer);
  bool write_pidOutput(yarp::os::idl::WireWriter& writer);
  bool nested_write_pidOutput(yarp::os::idl::WireWriter& writer);
  bool write_controlMode(yarp::os::idl::WireWriter& writer);
  bool nested_write_controlMode(yarp::os::idl::WireWriter& writer);
  bool write_interactionMode(yarp::os::idl::WireWriter& writer);
  bool nested_write_interactionMode(yarp::os::idl::WireWriter& writer);
  bool read_position(yarp::os::idl::WireReader& reader);
  bool nested_read_position(yarp::os::idl::WireReader& reader);
  bool read_velocity(yarp::os::idl::WireReader& reader);
  bool nested_read_velocity(yarp::os::idl::WireReader& reader);
  bool read_acceleration(yarp::os::idl::WireReader& reader);
  bool nested_read_acceleration(yarp::os::idl::WireReader& reader);
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
    void set_position(const std::vector<double> & position) {
      will_set_position();
      obj->position = position;
      mark_dirty_position();
      communicate();
      did_set_position();
    }
    void set_position(int index, const double elem) {
      will_set_position();
      obj->position[index] = elem;
      mark_dirty_position();
      communicate();
      did_set_position();
    }
    void set_velocity(const std::vector<double> & velocity) {
      will_set_velocity();
      obj->velocity = velocity;
      mark_dirty_velocity();
      communicate();
      did_set_velocity();
    }
    void set_velocity(int index, const double elem) {
      will_set_velocity();
      obj->velocity[index] = elem;
      mark_dirty_velocity();
      communicate();
      did_set_velocity();
    }
    void set_acceleration(const std::vector<double> & acceleration) {
      will_set_acceleration();
      obj->acceleration = acceleration;
      mark_dirty_acceleration();
      communicate();
      did_set_acceleration();
    }
    void set_acceleration(int index, const double elem) {
      will_set_acceleration();
      obj->acceleration[index] = elem;
      mark_dirty_acceleration();
      communicate();
      did_set_acceleration();
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
    const std::vector<double> & get_position() {
      return obj->position;
    }
    const std::vector<double> & get_velocity() {
      return obj->velocity;
    }
    const std::vector<double> & get_acceleration() {
      return obj->acceleration;
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
    virtual bool will_set_position() { return true; }
    virtual bool will_set_velocity() { return true; }
    virtual bool will_set_acceleration() { return true; }
    virtual bool will_set_torque() { return true; }
    virtual bool will_set_pidOutput() { return true; }
    virtual bool will_set_controlMode() { return true; }
    virtual bool will_set_interactionMode() { return true; }
    virtual bool did_set_position() { return true; }
    virtual bool did_set_velocity() { return true; }
    virtual bool did_set_acceleration() { return true; }
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
      if (yarp().canWrite()) {
        yarp().write(*this);
        clean();
      }
    }
    void mark_dirty() {
      is_dirty = true;
    }
    void mark_dirty_position() {
      if (is_dirty_position) return;
      dirty_count++;
      is_dirty_position = true;
      mark_dirty();
    }
    void mark_dirty_velocity() {
      if (is_dirty_velocity) return;
      dirty_count++;
      is_dirty_velocity = true;
      mark_dirty();
    }
    void mark_dirty_acceleration() {
      if (is_dirty_acceleration) return;
      dirty_count++;
      is_dirty_acceleration = true;
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
      is_dirty_position = flag;
      is_dirty_velocity = flag;
      is_dirty_acceleration = flag;
      is_dirty_torque = flag;
      is_dirty_pidOutput = flag;
      is_dirty_controlMode = flag;
      is_dirty_interactionMode = flag;
      dirty_count = flag ? 7 : 0;
    }
    bool is_dirty;
    int dirty_count;
    bool is_dirty_position;
    bool is_dirty_velocity;
    bool is_dirty_acceleration;
    bool is_dirty_torque;
    bool is_dirty_pidOutput;
    bool is_dirty_controlMode;
    bool is_dirty_interactionMode;
  };
};

#endif


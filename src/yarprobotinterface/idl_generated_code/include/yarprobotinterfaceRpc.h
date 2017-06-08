// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_yarprobotinterfaceRpc
#define YARP_THRIFT_GENERATOR_yarprobotinterfaceRpc

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>

class yarprobotinterfaceRpc;


class yarprobotinterfaceRpc : public yarp::os::Wire {
public:
  yarprobotinterfaceRpc();
  /**
   * Returns current phase.
   */
  virtual std::string get_phase();
  /**
   * Returns current level.
   */
  virtual int32_t get_level();
  /**
   * Returns robot name.
   */
  virtual std::string get_robot();
  /**
   * Returns true if yarprobotinterface is ready (all startup actions
   * performed and no interrupt called).
   */
  virtual bool is_ready();
  /**
   * Closes yarprobotinterface.
   */
  virtual std::string quit();
  /**
   * Closes yarprobotinterface.
   */
  virtual std::string bye();
  /**
   * Closes yarprobotinterface.
   */
  virtual std::string exit();
  virtual bool read(yarp::os::ConnectionReader& connection);
  virtual std::vector<std::string> help(const std::string& functionName="--all");
};

#endif

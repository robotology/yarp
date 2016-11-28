// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_yarpdataplayer_IDL
#define YARP_THRIFT_GENERATOR_yarpdataplayer_IDL

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>

class yarpdataplayer_IDL;


/**
 * yarpdataplayer_IDL
 * Interface.
 */
class yarpdataplayer_IDL : public yarp::os::Wire {
public:
  yarpdataplayer_IDL();
  /**
   * Steps the player once. The player will be stepped
   * until all parts have sent data
   * @return true/false on success/failure
   */
  virtual bool step();
  /**
   * Sets the frame number to the user desired frame.
   * @param name specifies the name of the loaded data
   * @param frameNum specifies the frame number the user
   *  would like to skip to
   * @return true/false on success/failure
   */
  virtual bool setFrame(const std::string& name, const int32_t frameNum);
  /**
   * Gets the frame number the user is requesting
   * @param name specifies the name of the data to modify
   *  would like to skip to
   * @return i32 returns the current frame index
   */
  virtual int32_t getFrame(const std::string& name);
  /**
   * Loads a dataset from a path
   * @return true/false on success/failure
   */
  virtual bool load(const std::string& path);
  /**
   * Plays the dataSets
   * @return true/false on success/failure
   */
  virtual bool play();
  /**
   * Pauses the dataSets
   * @return true/false on success/failure
   */
  virtual bool pause();
  /**
   * Stops the dataSets
   * @return true/false on success/failure
   */
  virtual bool stop();
  /**
   * Quit the module.
   * @return true/false on success/failure
   */
  virtual bool quit();
  virtual bool read(yarp::os::ConnectionReader& connection);
  virtual std::vector<std::string> help(const std::string& functionName="--all");
};

#endif

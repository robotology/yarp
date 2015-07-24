// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_ClockServer
#define YARP_THRIFT_GENERATOR_ClockServer

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>

namespace testing {
  class ClockServer;
}


class testing::ClockServer : public yarp::os::Wire {
public:
  ClockServer();
  virtual void pauseSimulation();
  virtual void continueSimulation();
  virtual void stepSimulation(const int32_t numberOfSteps = 1);
  virtual bool read(yarp::os::ConnectionReader& connection);
  virtual std::vector<std::string> help(const std::string& functionName="--all");
};

#endif


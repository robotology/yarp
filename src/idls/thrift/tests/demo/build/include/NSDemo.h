// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_NSDemo
#define YARP_THRIFT_GENERATOR_NSDemo

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>

namespace DemoWithNamespace {
  class NSDemo;
}


class DemoWithNamespace::NSDemo : public yarp::os::Wire {
public:
  NSDemo();
  virtual int32_t get_answer();
  virtual bool read(yarp::os::ConnectionReader& connection);
  virtual std::vector<std::string> help(const std::string& functionName="--all");
};

#endif


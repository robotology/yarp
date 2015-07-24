// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_Wrapping
#define YARP_THRIFT_GENERATOR_Wrapping

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Value.h>

class Wrapping;


class Wrapping : public yarp::os::Wire {
public:
  Wrapping();
  virtual int32_t check(const yarp::os::Value& param);
  virtual yarp::os::Bottle getBottle();
  virtual bool read(yarp::os::ConnectionReader& connection);
  virtual std::vector<std::string> help(const std::string& functionName="--all");
};

#endif


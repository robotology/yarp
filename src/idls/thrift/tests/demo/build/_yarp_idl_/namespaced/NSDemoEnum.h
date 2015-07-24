// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_ENUM_NSDemoEnum
#define YARP_THRIFT_GENERATOR_ENUM_NSDemoEnum

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>

namespace DemoWithNamespace {
  enum NSDemoEnum {
    ENUM1 = 1,
    ENUM2 = 2
  };

  class NSDemoEnumVocab;
}

class DemoWithNamespace::NSDemoEnumVocab : public yarp::os::idl::WireVocab {
public:
  virtual int fromString(const std::string& input);
  virtual std::string toString(int input);
};


#endif

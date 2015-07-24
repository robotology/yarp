// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_ENUM_DemoEnum
#define YARP_THRIFT_GENERATOR_ENUM_DemoEnum

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>

/**
 * Documentation for enumerator
 */
enum DemoEnum {
  /**
   * thing 1
   */
  ENUM1 = 1,
  /**
   * thing 2
   */
  ENUM2 = 2,
  ENUM3 = 3
};

class DemoEnumVocab;

class DemoEnumVocab : public yarp::os::idl::WireVocab {
public:
  virtual int fromString(const std::string& input);
  virtual std::string toString(int input);
};


#endif

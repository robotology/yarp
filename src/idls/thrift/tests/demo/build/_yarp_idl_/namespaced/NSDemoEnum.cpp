// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <NSDemoEnum.h>

namespace DemoWithNamespace {


int NSDemoEnumVocab::fromString(const std::string& input) {
  // definitely needs optimizing :-)
  if (input=="ENUM1") return (int)ENUM1;
  if (input=="ENUM2") return (int)ENUM2;
  return -1;
}
std::string NSDemoEnumVocab::toString(int input) {
  switch((NSDemoEnum)input) {
  case ENUM1:
    return "ENUM1";
  case ENUM2:
    return "ENUM2";
  }
  return "";
}
} // namespace



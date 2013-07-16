// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_STRUCT_PointDLists
#define YARP_THRIFT_GENERATOR_STRUCT_PointDLists

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <firstInterface/PointD.h>

namespace yarp {
  namespace test {
    class PointDLists;
  }
}


class yarp::test::PointDLists : public yarp::os::idl::WirePortable {
public:
  std::string name;
  std::vector<PointD>  firstList;
  std::vector<PointD>  secondList;
  PointDLists() : name("pointLists") {
  }
  PointDLists(const std::string& name,const std::vector<PointD> & firstList,const std::vector<PointD> & secondList) : name(name), firstList(firstList), secondList(secondList) {
  }
  bool read(yarp::os::idl::WireReader& reader);
  bool read(yarp::os::ConnectionReader& connection);
  bool write(yarp::os::idl::WireWriter& writer);
  bool write(yarp::os::ConnectionWriter& connection);
};

#endif


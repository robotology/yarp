// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <yarp/os/Property.h>

#include "TestList.h"

using namespace yarp;
using namespace yarp::os;

class PropertyTest : public UnitTest {
public:
  virtual String getName() { return "PropertyTest"; }

  void checkPutGet() {
    report(0,"checking puts and gets");
    Property p;
    p.put("hello","there");
    p.put("hello","friend");
    p.put("x","y");
    checkTrue(p.check("hello"), "key 1 exists");
    checkTrue(p.check("x"), "key 2 exists");
    checkTrue(!(p.check("y")), "other key should not exist");
    checkEqual(p.get("hello").c_str(),"friend", "key 1 has good value");
    checkEqual(p.get("x").c_str(),"y", "key 2 has good value");
  }

  virtual void runTests() {
    checkPutGet();
  }
};

static PropertyTest thePropertyTest;

UnitTest& getPropertyTest() {
  return thePropertyTest;
}

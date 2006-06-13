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
    checkEqual(p.get("hello").toString().c_str(),"friend", 
               "key 1 has good value");
    checkEqual(p.get("x").toString().c_str(),"y", 
               "key 2 has good value");
  }

  void checkExternal() {
    report(0,"checking external forms");
    Property p;
    p.fromString("(foo 12) (testing left right)");
    checkEqual(p.getString("foo").c_str(),"12","good key 1");
    checkEqual(p.getString("testing").c_str(),"left","good key 2");
    checkEqual(p.getList("testing")->toString().c_str(),
               "testing left right","good key 2 (more)");

    Property p2;
    p2.fromString(p.toString().c_str());
    checkEqual(p.getString("testing").c_str(),"left","good key after copy");
  }

  virtual void runTests() {
    checkPutGet();
    checkExternal();
  }
};

static PropertyTest thePropertyTest;

UnitTest& getPropertyTest() {
  return thePropertyTest;
}

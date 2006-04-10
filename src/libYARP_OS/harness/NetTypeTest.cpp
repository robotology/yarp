#include <yarp/NetType.h>

#include "TestList.h"

using namespace yarp;

class NetTypeTest : public UnitTest {
public:
  virtual String getName() { return "NetTypeTest"; }

  void checkCrc() {
    report(0,"checking cyclic redundancy check is plausible");

    char buf[] = { 1, 2, 3, 4, 5 };
    char buf2[] = { 1, 2, 3, 4, 5 };
    int len = 5;
    
    unsigned long ct1 = NetType::getCrc(buf,len);
    unsigned long ct2 = NetType::getCrc(buf2,len);
    checkEqual(ct1,ct2,"two identical sequences");
    
    buf[0] = 4;
    ct1 = NetType::getCrc(buf,len);
    checkTrue(ct1!=ct2,"two different sequences");

    buf2[0] = 4;
    ct2 = NetType::getCrc(buf2,len);
    checkTrue(ct1==ct2,"two identical sequences again");
  }

  virtual void runTests() {
    checkCrc();
  }
};

static NetTypeTest theNetTypeTest;

UnitTest& getNetTypeTest() {
  return theNetTypeTest;
}

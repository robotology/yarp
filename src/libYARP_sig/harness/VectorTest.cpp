
#include <yarp/sig/Vector.h>

#include "TestList.h"

using namespace yarp;
using namespace yarp::sig;

class VectorTest : public UnitTest {
public:
  virtual String getName() { return "VectorTest"; }

  void checkFormat() {
    report(0,"check vector format conforms to network standard...");
    report(0,"(paul will write this soon, he claims)");
  }

  virtual void runTests() {
    checkFormat();
  }
};

static VectorTest theVectorTest;

UnitTest& getVectorTest() {
  return theVectorTest;
}

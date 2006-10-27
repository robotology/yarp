#include <yarp/os/Stamp.h>
#include <yarp/os/all.h>

#include "TestList.h"

using namespace yarp;
using namespace yarp::os;

class StampTest : public UnitTest {
public:
  virtual String getName() { return "StampTest"; }

  void checkEnvelope() {
    report(0, "checking envelopes work...");

    BufferedPort<Bottle> in;
    BufferedPort<Bottle> out;

    in.setStrict();
    in.open("/in");
    out.open("/out");
    Network::connect("/out","/in");

    Bottle& outBot1 = out.prepare();   // Get the object
    outBot1.fromString("hello world"); // Set it up the way we want
    printf("Writing bottle 1 (%s)\n", outBot1.toString().c_str());
    Stamp stamp(55,1.0);
    out.setEnvelope(stamp);
    out.write();                       // Now send it on its way

    Bottle& outBot2 = out.prepare();
    outBot2.fromString("2 3 5 7 11");
    printf("Writing bottle 2 (%s)\n", outBot2.toString().c_str());
    out.writeStrict();                 // writeStrict() will wait for any

    // Read the first object
    Bottle *inBot1 = in.read();
    printf("Bottle 1 is: %s\n", inBot1->toString().c_str());

    // Read the second object
    Bottle *inBot2 = in.read();
    printf("Bottle 2 is: %s\n", inBot2->toString().c_str());
  }

  virtual void runTests() {
    // add tests here
    checkEnvelope();
  }
};

static StampTest theStampTest;

UnitTest& getStampTest() {
  return theStampTest;
}

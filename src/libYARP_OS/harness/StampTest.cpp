#include <math.h>

#include <yarp/String.h>

#include <yarp/os/Stamp.h>
#include <yarp/os/all.h>

#include <yarp/BufferedConnectionWriter.h>
#include <yarp/StreamConnectionReader.h>

#include "TestList.h"

using namespace yarp;
using namespace yarp::os;

class StampTest : public UnitTest {
public:
  virtual String getName() { return "StampTest"; }

  void checkFormat() {
    report(0, "checking Stamp can serialize ok...");

    for (int i=0; i<=1; i++) {
      bool textMode = (i==0);
      if (textMode) {
	report(0, "checking in text mode");
      } else {
	report(0, "checking in binary mode");
      }

      Stamp stamp(55,1.0);
      
      BufferedConnectionWriter writer(textMode);
      stamp.write(writer);
      String s = writer.toString();
      Bottle bot;
      if (textMode) {
	bot.fromString(s.c_str());
      } else {
	bot.fromBinary(s.c_str(),s.length());    
      }
      
      checkEqual(bot.get(0).asInt(),55,"sequence number write");
      checkTrue(fabs(bot.get(1).asDouble()-1)<0.0001,"time stamp write");
      
      StringInputStream sis;
      StreamConnectionReader sbr;
      s = writer.toString();
      sis.add(s);
      Route route;
      sbr.reset(sis,NULL,route,s.length(),textMode);
      
      Stamp outStamp;
      outStamp.read(sbr);
      
      checkEqual(outStamp.getCount(),55,"sequence number read");
      checkTrue(fabs(outStamp.getTime()-1)<0.0001,"time stamp read");
    }

  }

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
    Stamp stamp(55,1.0);
    out.setEnvelope(stamp);
    out.write();                       // Now send it on its way

    Bottle& outBot2 = out.prepare();
    outBot2.fromString("2 3 5 7 11");
    Stamp stamp2(55,4.0);
    out.setEnvelope(stamp);
    out.writeStrict();                 // writeStrict() will wait for any

    // Read the first object
    Bottle *inBot1 = in.read();
    Stamp inStamp;
    in.getEnvelope(inStamp);
    checkTrue(fabs(inStamp.getTime()-1)<0.0001,"time stamp 1 read");

    // Read the second object
    Bottle *inBot2 = in.read();
    in.getEnvelope(inStamp);
    checkTrue(fabs(inStamp.getTime()-1)<0.0001,"time stamp 2 read");
  }

  virtual void runTests() {
    // add tests here
    Network::setLocalMode(true);
    checkFormat();
    checkEnvelope();
    Network::setLocalMode(false);
  }
};

static StampTest theStampTest;

UnitTest& getStampTest() {
  return theStampTest;
}

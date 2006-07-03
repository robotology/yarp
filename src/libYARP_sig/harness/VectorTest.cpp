
#include <yarp/sig/Vector.h>
#include <yarp/BufferedConnectionWriter.h>
#include <yarp/os/Bottle.h>

#include "TestList.h"

using namespace yarp;
using namespace yarp::os;
using namespace yarp::sig;

class VectorTest : public UnitTest {
public:
  virtual String getName() { return "VectorTest"; }

  void checkFormat() {
    report(0,"check vector format conforms to network standard...");
    Vector v(4,0.0);
    for (unsigned int i=0; i<v.size(); i++ ){
      v[i] = i;
    }
    BufferedConnectionWriter writer;
    v.write(writer);
    String s = writer.toString();
    Bottle bot;
    bot.fromBinary(s.c_str(),s.length());
    checkEqual(bot.size(),v.size(),"size matches");
    for (int i=0; i<bot.size(); i++) {
      checkTrue(bot.get(i).asDouble()>i-0.25,"bounded below");
      checkTrue(bot.get(i).asDouble()<i+0.25,"bounded above");
    }
  }

  virtual void runTests() {
    checkFormat();
  }
};

static VectorTest theVectorTest;

UnitTest& getVectorTest() {
  return theVectorTest;
}

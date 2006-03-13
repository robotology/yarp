
/**
 *
 * This is a special set of tests, to see how well we support
 * the "Classic" YARP interface
 *
 */

//#include <yarp/HeaderCompatibility.h>

#include <yarp/YARPPort.h>
#include <yarp/YARPTime.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPThread.h>

#include <yarp/NameClient.h>
#include <yarp/os/Time.h>

#include "TestList.h"

using namespace yarp;

class HeaderCompatibilityTest : public UnitTest {
public:
  virtual String getName() { return "HeaderCompatibilityTest"; }

  void testPorts() {
    report(0,"checking YARPPort...");

    NameClient& nic = NameClient::getNameClient();

    YARPInputPortOf<int> in;
    YARPOutputPortOf<int> out;

    checkEqual(nic.queryName("/hct/read").isValid(),false,"YARPPort pre reg");
    checkEqual(nic.queryName("/hct/write").isValid(),false,"YARPPort pre reg");
    in.Register("/hct/read");
    out.Register("/hct/write");
    YARPPort::Connect("/hct/write","/hct/read");
    checkEqual(nic.queryName("/hct/read").isValid(),true,"YARPPort post reg");
    checkEqual(nic.queryName("/hct/write").isValid(),true,"YARPPort post reg");
    checkEqual((&(out.Content())==NULL),false,"output content exists...");
    out.Content() = 15;
    checkEqual((int)(out.Content()),15,"output content settable");
    out.Write();
    checkEqual(in.Read()!=0,true,"read something");
    checkEqual((int)(in.Content()),15,"got right value at input port");
  }

  virtual void testTime() {
    report(0,"testing YARPTime (there will be a short pause)...");
    double target = 0.5;
    double t1 = YARPTime::GetTimeAsSeconds();
    YARPTime::DelayInSeconds(target);
    double t2 = YARPTime::GetTimeAsSeconds();
    double dt = t2-t1-target;
    double limit = 0.1; // don't be too picky, there is a lot of undefined slop
    bool inLimits = (-limit<dt)&&(dt<limit);
    checkEqual(true,inLimits,"delay for 0.5 seconds");    
  }

  virtual void testSema() {
    report(0,"very soft test of YARPSemaphore...");
    YARPSemaphore sema(2);
    checkTrue(sema.PollingWait(),"down one");
    checkTrue(sema.PollingWait(),"down two");
    checkFalse(sema.PollingWait(),"and done");
  }

  class ThreadTest : public YARPThread {
  public:
    int v;
    ThreadTest() : v(0) {}
    void Body() {
      YARPTime::DelayInSeconds(1);
      v = 1;
    }
  };

  virtual void testThread() {
    report(0,"very soft test of YARPThread...");
    ThreadTest tt;
    checkEqual(tt.v,0,"starting value");
    tt.Begin();
    checkEqual(tt.v,0,"starting value repeat");
    tt.Join();
    checkEqual(tt.v,1,"ending value");
  }

  virtual void runTests() {
    NameClient& nic = NameClient::getNameClient();
    nic.setFakeMode(true);

    testPorts();
    testTime();
    testSema();
    testThread();

    nic.setFakeMode(false);
  }
};

static HeaderCompatibilityTest theHeaderCompatibilityTest;

UnitTest& getHeaderCompatibilityTest() {
  return theHeaderCompatibilityTest;
}


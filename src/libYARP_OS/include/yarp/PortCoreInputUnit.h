#ifndef _YARP2_PORTCOREINPUTUNIT_
#define _YARP2_PORTCOREINPUTUNIT_

#include <yarp/PortCore.h>
#include <yarp/PortCoreUnit.h>
#include <yarp/Logger.h>
#include <yarp/InputProtocol.h>

namespace yarp {
  class PortCoreInputUnit;
}

/**
 * Manager for a single input to a port.  Associated
 * with a PortCore object.
 */
class yarp::PortCoreInputUnit : public PortCoreUnit {
public:
  // specifically for managing input connections

  PortCoreInputUnit(PortCore& owner, InputProtocol *ip, 
		    bool autoHandshake) : 
    PortCoreUnit(owner), ip(ip), phase(1), autoHandshake(autoHandshake) {

    YARP_ASSERT(ip!=NULL);
    closing = false;
    finished = false;
    running = false;
    name = owner.getName();
  }

  virtual ~PortCoreInputUnit() {
    closeMain();
  }

  virtual bool start();

  virtual void run();

  virtual bool isInput() {
    return true;
  }

  // just for testing
  virtual void runSimulation();

  virtual void close() {
    closeMain();
  }

  virtual bool isFinished() {
    return finished;
  }

  const String& getName() {
    return name;
  }

  virtual Route getRoute();


private:
  InputProtocol *ip;
  SemaphoreImpl phase;
  bool autoHandshake;
  bool closing, finished, running;
  String name;

  void closeMain();
};

#endif


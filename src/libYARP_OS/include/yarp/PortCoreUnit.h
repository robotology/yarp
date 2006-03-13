#ifndef _YARP2_PORTCOREUNIT_
#define _YARP2_PORTCOREUNIT_

#include <yarp/PortCore.h>
#include <yarp/ThreadImpl.h>

namespace yarp {
  class PortCoreUnit;
}

/**
 * This is a helper for the core port class, PortCore.
 * It manages a single threaded resource related to a single
 * connection.
 */

class yarp::PortCoreUnit : public ThreadImpl {
public:
  PortCoreUnit(PortCore& owner) : owner(owner) {
    doomed = false;
  }

  virtual ~PortCoreUnit() {
  }

  virtual bool isInput() {
    return false;
  }

  virtual bool isOutput() {
    return false;
  }

  virtual bool isFinished() {
    return false;
  }

  virtual Route getRoute() {
    return Route("null","null","null");
  }

  bool isDoomed() {
    return doomed;
  }

  void setDoomed(bool flag = true) {
    doomed = flag;
  }

  virtual void send(Writable& writer) {
    // do nothing
  }

protected:
  PortCore& getOwner() { return owner; }

private:
  PortCore& owner;
  bool doomed;
};

#endif


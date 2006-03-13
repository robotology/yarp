#ifndef _YARP2_FALLBACKNAMECLIENT_
#define _YARP2_FALLBACKNAMECLIENT_

#include <yarp/ThreadImpl.h>
#include <yarp/Address.h>
#include <yarp/DgramTwoWayStream.h>

namespace yarp {
  class FallbackNameClient;
}

/**
 * A client for the FallbackNameServer class.  Provides a last-resort
 * means of searching for the name server.
 */
class yarp::FallbackNameClient : public ThreadImpl {
public:
  FallbackNameClient() {
    closed = false;
  }

  virtual void run();
  virtual void close();

  Address getAddress();

  static Address seek();

private:
  Address address;
  DgramTwoWayStream listen;
  bool closed;
};

#endif


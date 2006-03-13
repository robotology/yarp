#ifndef _YARP2_FALLBACKNAMESERVER_
#define _YARP2_FALLBACKNAMESERVER_

#include <yarp/ThreadImpl.h>
#include <yarp/Address.h>
#include <yarp/DgramTwoWayStream.h>

namespace yarp {
  class FallbackNameServer;
  class NameServer;
}

/**
 * Multi-cast server, for last resort information sharing about
 * name information -- when config files are missing or wrong
 */
class yarp::FallbackNameServer : public ThreadImpl {
public:
  FallbackNameServer(NameServer& owner) : owner(owner) {
    closed = false;
  }

  virtual void run();
  virtual void close();

  static const Address& getAddress() {
    return mcastLastResort;
  }

private:
  NameServer& owner;
  static const Address mcastLastResort;
  DgramTwoWayStream listen;
  bool closed;
};

#endif


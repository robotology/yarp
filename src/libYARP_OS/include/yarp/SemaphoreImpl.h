#ifndef _YARP2_SEMAPHOREIMPL_
#define _YARP2_SEMAPHOREIMPL_

#include <ace/Synch.h>

namespace yarp {
  class SemaphoreImpl;
}

/**
 * A semaphore abstraction for mutual exclusion and resource management.
 */
class yarp::SemaphoreImpl {
public:
  SemaphoreImpl(int initialCount = 1) : sema(initialCount) {
  }

  virtual ~SemaphoreImpl() {}

  // blocking wait
  void wait() {
    sema.acquire();
  }

  // polling wait
  bool check() {
    return (sema.tryacquire()<0)?0:1;
  }

  // increment
  void post() {
    sema.release();
  }

private:
  ACE_Semaphore sema;
};

#endif


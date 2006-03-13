#ifndef _YARP2_THREADIMPL_
#define _YARP2_THREADIMPL_

#include <yarp/Runnable.h>
#include <yarp/SemaphoreImpl.h>

#include <ace/Thread.h>


namespace yarp {
  class ThreadImpl;
}

/**
 * An abstraction for a thread of execution.
 */
class yarp::ThreadImpl : public Runnable {
public:
  ThreadImpl();
  ThreadImpl(Runnable *target);

  virtual ~ThreadImpl();

  int join(double seconds = -1);
  virtual void run();
  virtual void close();

  // should throw if no success
  virtual bool start();

  bool isClosing();

  virtual void beforeStart();
  virtual void afterStart(bool success);

  // call before start
  void setOptions(int stackSize = 0);

  static int getCount();

  // won't be public for long...
  static void changeCount(int delta);

  // get a unique key
  long int getKey();

private:
  int stackSize;
  ACE_hthread_t hid;
  ACE_thread_t id;
  bool active;
  bool closing;
  Runnable *delegate;

  static int threadCount;
  static SemaphoreImpl threadMutex;
};

#endif


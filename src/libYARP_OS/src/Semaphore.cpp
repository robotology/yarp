
#include <yarp/os/Semaphore.h>
#include <yarp/Logger.h>
#include <yarp/SemaphoreImpl.h>

using namespace yarp;
using namespace yarp::os;

Semaphore::Semaphore(int initialCount) {
  implementation = new SemaphoreImpl(initialCount);
  YARP_ASSERT(implementation!=NULL);
}

Semaphore::~Semaphore() {
  if (implementation!=NULL) {
    delete ((SemaphoreImpl*)implementation);
    implementation = NULL;
  }
}

void Semaphore::wait() {
  ((SemaphoreImpl*)implementation)->wait();
}

bool Semaphore::check() {
  return ((SemaphoreImpl*)implementation)->check();
}

void Semaphore::post() {
  ((SemaphoreImpl*)implementation)->post();
}


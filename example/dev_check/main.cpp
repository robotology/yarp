
#include <stdio.h>

#include <yarp/dev/Drivers.h>
#include <yarp/dev/FrameGrabberInterfaces.h>

using namespace yarp::dev;

int main() {
  DeviceDriver *dd = Drivers::factory().create("dragonfly");
  if (dd!=NULL) {
    printf("Dragonfly created\n");
    dd->close();
    delete dd;
  } else {
    printf("Dragonfly not available\n");
  }
  return 0;
}




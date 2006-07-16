#include <stdio.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/Drivers.h>
#include "FakeFrameGrabber.h"
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

int main() {
  Network::init();

  // give YARP a factory for creating instances of FakeFrameGrabber
  DriverCreator *fakey_factory = 
    new DriverCreatorOf<FakeFrameGrabber>("fakey",
					  "grabber",
					  "FakeFrameGrabber");
  Drivers::factory().add(fakey_factory); // hand factory over to YARP

  // use YARP to create and configure a networked of FakeFrameGrabber
  Property config;
  config.fromString("(device grabber) (name /fakey) (subdevice fakey) (w 200) (h 200)");
  PolyDriver dd(config);
  if (!dd.isValid()) {
    printf("Failed to create and configure a device\n");
    exit(1);
  }

  // snooze while the network device operates
  while (true) {
    printf("Network device is active...\n");
    Time::delay(5);
  }

  Network::fini();
  return 0;
}

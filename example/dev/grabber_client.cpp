#include <stdio.h>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/Drivers.h>
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

int main() {
  Network::init();

  Property config;
  config.fromString("(device remote_grabber) (local /client) (remote /fakey)");
  PolyDriver dd(config);
  if (!dd.isValid()) {
    printf("Failed to create and configure device\n");
    exit(1);
  }
  IFrameGrabberImage *grabberInterface;
  if (!dd.view(grabberInterface)) {
    printf("Failed to view device through IFrameGrabberImage interface\n");
    exit(1);
  }

  ImageOf<PixelRgb> img;
  grabberInterface->getImage(img);
  printf("Got a %dx%d image\n", img.width(), img.height());

  dd.close();

  Network::fini();
  return 0;
}

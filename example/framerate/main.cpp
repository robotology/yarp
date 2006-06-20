
#include <stdio.h>

#include <yarp/sig/Image.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Property.h>
#include <yarp/os/Network.h>
#include <yarp/os/Time.h>
#include <yarp/String.h>
#include <yarp/os/Network.h>

using namespace yarp;
using namespace yarp::os;
using namespace yarp::sig;

int main(int argc, char *argv[]) {
  Network::init();
  if (argc==1) {
    printf("This program checks the framerate of an image output port\n");
    printf("Call as:\n");
    printf("   framerate --remote /image_port_name --local /local_name\n");
    exit(0);
  }

  BufferedPort<ImageOf<PixelRgb> > port;

  // get options
  Property opt;
  opt.fromCommand(argc,argv);

  // name port
  BottleBit *val;
  String name = "/get_image";
  if (opt.check("local",val)) {
    name = val->asString().c_str();
  }
  port.open(name.c_str());

  // connect port
  if (opt.check("remote",val)) {
    Network::connect(val->asString(),name.c_str());
  }

  // read
  double first = Time::now();
  double prev = 0;
  int ct = 0;
  bool spoke = false;
  while (true) {
    ImageOf<PixelRgb> *img = port.read(true);
    double now = Time::now()-first;
    ct++;
    if (now-prev>=2) {
      double period = (now-prev)/ct;
      printf("Period is %g ms per frame, freq is %g (%d images in %g seconds)\n",
	     period*1000, 1/period, ct, now-prev);
      ct = 0;
      prev = now;
      spoke = false;
    }
    if (img!=NULL) {
      if (!spoke) {
	printf("Got a %dx%d image\n", img->width(), img->height());
	spoke = true;
      }
    }
  }
  Network::fini();
  return 0;
}


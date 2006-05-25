// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifdef USE_EXPERIMENTAL

#include <yarp/dev/Drivers.h>
#include <yarp/String.h>
#include <yarp/Logger.h>

#include <ace/OS.h>


// just testing for now, list all active drivers here
// and add creator below
#include <yarp/DragonflyDeviceDriver.h>

using namespace yarp;
using namespace yarp::os;
using namespace yarp::dev;


Drivers Drivers::instance;

Drivers::Drivers() {
  implementation = NULL;
}


Drivers::~Drivers() {
}

yarp::os::ConstString Drivers::toString() {
  return ConstString("no description yet");
}


DeviceDriver *Drivers::create(const yarp::os::Property& prop) {
  String str = prop.get("name").c_str();
  if (str == "dragonfly") {
    DragonflyDeviceDriver *driver = new DragonflyDeviceDriver();
    YARP_ASSERT(driver!=NULL);
    DragonflyOpenParameters par;
	par._size_x = 640;
	par._size_y = 480;
    driver->open(par);
    return driver;
  }
  
  return NULL;
}


#endif

/*
 * Copyright (C) 2010 Philipp Robbel
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 * Wraps KinectDeviceDriver for Yarp network access, generating local
 * timestamps.
 *
 * Uses the following parameters:
 * "name": [string] defines yarpdev name as well as kinect port name
 *                  as <name>_new_kinect.
 *                  Make sure to start name with the '/' character.
 * XXX ASSUMED TRUE FOR NOW:
 * "stamp": [bool] defines whether to timestamp the packages on the server side.
 *                 (Default: 0)
 */

#ifndef SERVERKINECT_H
#define SERVERKINECT_H

#include <yarp/dev/PolyDriver.h>
#include <yarp/os/Time.h>
#include <yarp/os/Thread.h>
#include <yarp/sig/Image.h>
#include <yarp/os/PortablePair.h>
#include <yarp/os/Stamp.h>
using namespace yarp::os;
using namespace yarp::sig;

#include "KinectDeviceDriver.h"

namespace yarp {
  namespace dev {
    class ServerKinect;
  }
}

#define VOCAB_SET VOCAB3('s','e','t')
#define VOCAB_GET VOCAB3('g','e','t')
#define VOCAB_IS VOCAB2('i','s')
#define VOCAB_WIDTH VOCAB1('w')
#define VOCAB_HEIGHT VOCAB1('h')
#define VOCAB_FAILED VOCAB4('f','a','i','l')
#define VOCAB_OK VOCAB2('o','k')

class yarp::dev::ServerKinect : public DeviceDriver,
  public DeviceResponder,
  private Thread {
public:
  ServerKinect() : kinect(0), addStamp(false) { }

  virtual ~ServerKinect() {
    close();
  }

  virtual bool open(Searchable & prop) {
    bool verb = prop.check("verbose","if present, give detailed output");
    if (verb)
      printf("running with verbose output\n");

    // enable rpc commands
    p.setReader(*this);

    Value *name;
    if(prop.check("subdevice",name,"name of specific control device to wrap")) {
      printf("Subdevice %s\n", name->toString().c_str());
      if (name->isString()) {
        // maybe user isn't doing nested configuration
        Property p;
        p.setMonitor(prop.getMonitor(), "subdevice"); // pass on any monitoring
        p.fromString(prop.toString());
        p.put("device",name->toString());
        poly.open(p);
      } else {
        Bottle subdevice = prop.findGroup("subdevice").tail();
        poly.open(subdevice);
      }
      if (!poly.isValid()) {
        printf("cannot make <%s>\n", name->toString().c_str());
      }
    } else {
      printf("\"--subdevice <name>\" not set for server_serial\n");
      return false;
    }

    if (!poly.isValid()) {
      return false;
    }

    addStamp = prop.check("stamp","if present, add timestamps to data");
    ConstString robotName = prop.check("name",Value("/MDS"), "prefix for port names").asString().c_str();

    writer.attach(p);

    char sr_portName[1024];
    sprintf(sr_portName, "%s_new_kinect",robotName.c_str());
    p.open(sr_portName);

    if (poly.isValid())
      poly.view(kinect);

    // enable rpc interface
    DeviceResponder::makeUsage();
    addUsage("[get] [w]", "get width of image");
    addUsage("[get] [h]", "get height of image");

    // start thread
    if(kinect != 0) {
      start();
      return true;
    }

    printf("subdevice <%s> doesn't look like a kinect device (no appropriate interfaces were acquired)\n", name->toString().c_str());

    return false;
  }

  virtual bool close() {
    return closeMain();
  }

  bool getImagePair(PortablePair<ImageOf<PixelMono16>, ImageOf<PixelRgb> > & pair) {
    if(kinect != 0) {
      kinect->getImagePair(pair);
      return true;
    } else
      return false;
  }

  // DeviceResponder interface
  virtual bool respond(const Bottle& cmd, Bottle& response) {
    int code = cmd.get(0).asVocab();
    bool rec = false; // command recognized
    bool ok = false; // return value

    switch (code) {
      case VOCAB_SET:
        printf("set command received\n");
        break;
      case VOCAB_GET:
        printf("get command received\n");
        {
          response.addVocab(VOCAB_IS);
          response.add(cmd.get(1));
          switch(cmd.get(1).asVocab())
          {
            case VOCAB_WIDTH:
              ok = true;
              response.addInt(kinect->width());
              rec = true;
              break;
            case VOCAB_HEIGHT:
              ok = true;
              response.addInt(kinect->height());
              rec = true;
              break;
          }
        }
        break;
    }

    if (!rec)
      return DeviceResponder::respond(cmd,response);
    if (!ok) {
      // failed thus send only a VOCAB back.
      response.clear();
      response.addVocab(VOCAB_FAILED);
    } else
      response.addVocab(VOCAB_OK);

    return ok;
  }

  /**
   * The thread main loop deals with writing on ports here.
   */
  virtual void run() {
    printf("Kinect server starting\n");
    // double before, now; // XXX could add this to an envelope, aka ServerInertial.h
    while (!isStopping()) {
      // before = SystemClock::nowSystem();
      PortablePair<ImageOf<PixelMono16>, ImageOf<PixelRgb> > & l = writer.get();
      getImagePair(l); // XXX shouldn't we check for true/false return? But ServerSerial.h says that this can create memory leak (if write is not called for obtained object?!). XXX BETTER ERROR CHECKING (check other Yarp wrapper implementations for ideas)
      stamp.update();
      p.setEnvelope(stamp);
      writer.write();
      // now = SystemClock::nowSystem();
      // give other threads the chance to run
      SystemClock::delaySystem(0.010);
    }
    printf("Kinect server stopping\n");
  }

private:
  PolyDriver poly;
  KinectDeviceDriver * kinect;
  Port p;
  PortWriterBuffer<PortablePair<ImageOf<PixelMono16>, ImageOf<PixelRgb> > > writer;
  bool addStamp;
  Stamp stamp;

  bool closeMain() {
    if (Thread::isRunning()) {
      Thread::stop();
    }
    //close the port connections here
    p.close();
    poly.close(); // calls close() on device driver
    return true;
  }
};

#endif

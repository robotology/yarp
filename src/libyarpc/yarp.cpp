// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2008 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <stdio.h>

#include <yarp/os/all.h>

using namespace yarp::os;

#include "yarp.h"

#define YARP_DEFINE(rt) rt

#define _PORT_OK(s) if(s->implementation==NULL) return -1;
#define _PORT(s) (*((Port*)(s->implementation)))

YARP_DEFINE(int) yarpPortOpen(yarpPortPtr port, const char *name) {
  _PORT_OK(port);
  bool ok = _PORT(port).open(name);
  return ok?0:-1;
}

YARP_DEFINE(int) yarpPortClose(yarpPortPtr port) {
  _PORT_OK(port);
  _PORT(port).close();
  return 0;
}


YARP_DECLARE(yarpPortPtr) yarpPortCreate() {
  yarpPortPtr port = new yarpPort;
  if (port!=NULL) {
    port->implementation = new Port();
    if (port->implementation==NULL) {
      delete port;
      port = NULL;
    }
  }
  return port;
}


YARP_DECLARE(void) yarpPortDestroy(yarpPortPtr port) {
  if (port!=NULL) {
    if (port->implementation!=NULL) {
      delete (Port*)(port->implementation);
      port->implementation = NULL;
    }
    delete port;
  }
}





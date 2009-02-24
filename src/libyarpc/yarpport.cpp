// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2008, 2009 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <stdio.h>

#include "yarp.h"
#include "yarpimpl.h"

YARP_DEFINE(yarpPortPtr) yarpPortCreate(yarpNetworkPtr network) {
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

YARP_DEFINE(void) yarpPortFree(yarpPortPtr port) {
  if (port!=NULL) {
    if (port->implementation!=NULL) {
      delete (Port*)(port->implementation);
      port->implementation = NULL;
    }
    delete port;
  }
}

YARP_DEFINE(int) yarpPortOpen(yarpPortPtr port, yarpContactPtr contact) {
  YARP_OK(port);
  YARP_OK(contact);
  bool ok = YARP_PORT(port).open(YARP_CONTACT(contact));
  return ok?0:-1;
}

YARP_DEFINE(int) yarpPortClose(yarpPortPtr port) {
  YARP_OK(port);
  YARP_PORT(port).close();
  return 0;
}



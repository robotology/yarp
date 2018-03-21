/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/all.h>
#include <stdio.h>
using namespace yarp::os;

int main(int argc, char *argv[]) {

  // Set up YARP
  Network yarp;

  // Make two ports called /hello/in and /hello/out
  // We'll send "Bottles" (a simple nested list container) between these ports
  BufferedPort<Bottle> inPort, outPort;
  bool ok = inPort.open("/hello/in");
  ok = ok && outPort.open("/hello/out");
  if (!ok) {
    fprintf(stderr, "Failed to create ports.\n");
    fprintf(stderr, "Maybe you need to start a nameserver (run 'yarpserver')\n");
    return 1;
  }

  // Make a connection between the output port and the input port
  yarp.connect(outPort.getName(),inPort.getName());

  for (int i=0; i<10; i++) {

    // prepare a message to send
    Bottle&out = outPort.prepare();
    out.clear();
    out.addString("Hello");
    out.addInt(i);
    printf("Sending %s\n", out.toString().c_str());

    // send the message
    outPort.write(true);

    // read the message
    Bottle *in = inPort.read();
    if (in==NULL) {
      fprintf(stderr, "Failed to read message\n");
      return 1;
    }
    printf("Received %s\n", in->toString().c_str());
  }

  return 0;
}

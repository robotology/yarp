/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <stdio.h>
#include <stdlib.h>

#include <yarp/os/all.h>

using namespace yarp::os;

class Porter : public Thread {
public:
  ConstString name;
  BufferedPort<Bottle> port;

  virtual void run() {
    Network::connect(name,"/reader","mcast");
    for (int i=0; i<50; i++) {
      Bottle& b = port.prepare();
      b.clear();
      b.addString(name);
      port.write();
      Time::delay(0.1);
    }
  }
};

class Reader : public Portable {
public:
  virtual bool write(ConnectionWriter& con) {
    return false;
  }

  virtual bool read(ConnectionReader& con) {
    Bottle b;
    if (!b.read(con)) {
      return false;
    }
    ConstString src = con.getRemoteContact().getName();
    ConstString txt = b.get(0).asString();
    printf("Reading [%s] from [%s]\n", txt.c_str(), src.c_str());
    if (src!=txt) {
      printf("YIKES!!!\n");
      exit(1);
    }
    return true;
  }
};

#define N (10)

int main() {
  Network yarp;

  for (int k=0; k<100; k++) {

    printf("\n\n\n==========================================\n");
    printf("Start up round %d\n", k);

    {
      Reader reader;
      Port p;
      p.setReader(reader);
      p.open("/reader");
      Porter writer[N];
      for (int i=0; i<N; i++) {
	ConstString name = ConstString("/writer/") + ConstString::toString(i);
	writer[i].name = name;
	writer[i].port.open(name);
      }
      for (int i=0; i<N; i++) {
	writer[i].start();
      }
      Time::delay(20);
      printf("Shutting down\n");
      for (int i=0; i<N; i++) {
	writer[i].stop();
      }
    }

    printf("Shut down %d\n", k);
  }

  return 0;
}


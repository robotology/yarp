
/*
 * Copyright: (C) 2012 IITRBCS
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/all.h>
#include <yarp/name/all.h>

#include <stdlib.h>

using namespace yarp::os;
using namespace yarp::name;

class ProxyNameService : public yarp::name::NameService {
public:
  void appendEntry(yarp::os::Bottle& reply, const Contact& c) {
    Bottle& info = reply.addList();
    info.addString("registration");
    info.addString("name");
    info.addString(c.getName().c_str());
    info.addString("ip");
    info.addString(c.getHost().c_str());
    info.addString("port");
    info.addInt(c.getPort());
    info.addString("type");
    info.addString(c.getCarrier().c_str());
  }

  virtual bool cmdQuery(yarp::os::Bottle& cmd,
                        yarp::os::Bottle& reply,
                        yarp::os::Contact& remote) {
    reply.addString("old");
    ConstString name = cmd.get(1).asString();
    Contact c = Network::queryName(name);
    if (c.isValid()) {
      appendEntry(reply,c);
    }
    return true;
  }

  virtual bool cmdList(yarp::os::Bottle& cmd,
                       yarp::os::Bottle& reply,
                       yarp::os::Contact& remote) {
    return false;
  }

  virtual bool cmdUnregister(yarp::os::Bottle& cmd,
                             yarp::os::Bottle& reply,
                             yarp::os::Contact& remote) {
    return false;
  }

  virtual bool cmdRegister(yarp::os::Bottle& cmd,
                           yarp::os::Bottle& reply,
                           yarp::os::Contact& remote) {
    return false;
  }

  virtual bool apply(yarp::os::Bottle& cmd,
                     yarp::os::Bottle& reply,
                     yarp::os::Bottle& event,
                     yarp::os::Contact& remote) {
    bool ok = false;
    printf(" + %s\n", cmd.toString().c_str());
    ConstString tag = cmd.get(0).asString();
    if (tag=="register") {
      ok = cmdRegister(cmd,reply,remote);
    } else if (tag=="unregister") {
      ok = cmdUnregister(cmd,reply,remote);
    } else if (tag=="query") {
      ok = cmdQuery(cmd,reply,remote);
    } else if (tag=="list") {
      ok = cmdList(cmd,reply,remote);
    } else {
      reply.addString("old");
      reply.addString("I have no idea what you are talking about");
    }
    return ok;
  }
};


int main(int argc, char *argv[]) {
  printf("Proxy basic name server requests.\n");
  printf("In fact, ONLY name queries proxied right now.\n");
  Property config;
  config.fromCommand(argc,argv);
  if (!config.check("socket")) {
    fprintf(stderr,"Please supply a --socket NNNN option\n");
    exit(1);
  }
  int socket = config.find("socket").asInt();

  Network yarp;
  Contact contact(config.check("name", Value("/name/proxy")).asString(),
                  "tcp",
                  "localhost",
                  socket);

  ProxyNameService proxy;
  NameServerManager manager(proxy);
  Port server;
  manager.setPort(server);
  server.setReaderCreator(manager);
  bool ok = server.open(contact,false);
  if (!ok) {
    fprintf(stderr, "Name server failed to open\n");
    return 1;
  }
  while (true) {
    Time::delay(600);
    printf("Name server running happily\n");
  }

  return 0;
}

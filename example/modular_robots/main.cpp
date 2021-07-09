/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/all.h>
#include <yarp/math/Rand.h>
#include <yarp/name/all.h>

#include <map>
#include <set>
#include <string>

using namespace yarp::os;
using namespace yarp::math;
using namespace yarp::name;
using namespace std;

#define DEFAULT_NAME_PORT_NUMBER 10000
#define DEFAULT_NAME_PORT_RANGE 10000

class Entry {
public:
  string name;
  string carrier;
  string machine;
  int portNumber;

  Entry() {
    portNumber = 0;
  }
};

class WideNameService : public yarp::name::NameService {
private:
  std::mutex mutex;
  map<string,Entry> names;
  map<int,int> numbers;
  int lastNumber;
  int firstNumber;

  // Very conservative algorithm, to avoid having to
  // recognize the local machine by (possibly one of many,
  // and changing over time) IP address.
  // Note: be careful if changing this to think about
  // race conditions.
  int allocatePortNumber() {
    for (int i=0; i<DEFAULT_NAME_PORT_RANGE; i++) {
      int num = firstNumber +
        (lastNumber+i+1-firstNumber)%DEFAULT_NAME_PORT_RANGE;
      if (numbers.find(num)==numbers.end()) {
        return num;
      }
    }
    return 0;
  }

public:
  WideNameService() : mutex() {
    firstNumber = lastNumber =  DEFAULT_NAME_PORT_NUMBER;
  }

  void appendEntry(yarp::os::Bottle& reply, const Entry& e) {
    Bottle& info = reply.addList();
    info.addString("registration");
    info.addString("name");
    info.addString(e.name.c_str());
    info.addString("ip");
    info.addString(e.machine.c_str());
    info.addString("port");
    info.addInt32(e.portNumber);
    info.addString("type");
    info.addString(e.carrier.c_str());
  }

  virtual bool cmdQuery(yarp::os::Bottle& cmd,
                        yarp::os::Bottle& reply,
                        yarp::os::Contact& remote) {
    reply.addString("old");
    string name = cmd.get(1).asString().c_str();
    map<string,Entry>::iterator it = names.find(name);
    if (it==names.end()) {
      return true;
    }
    Entry& e = it->second;
    appendEntry(reply,e);
    return true;
  }

  virtual bool cmdList(yarp::os::Bottle& cmd,
                       yarp::os::Bottle& reply,
                       yarp::os::Contact& remote) {
    reply.addString("old");
    for (map<string,Entry>::iterator it = names.begin();
         it!=names.end();
         it++) {
      appendEntry(reply,it->second);
    }
    return true;
  }

  virtual bool cmdUnregister(yarp::os::Bottle& cmd,
                             yarp::os::Bottle& reply,
                             yarp::os::Contact& remote) {
    std::string name = cmd.get(1).asString();
    map<string,Entry>::iterator it = names.find(name.c_str());
    if (it!=names.end()) {
      Entry& entry = it->second;
      int number = entry.portNumber;
      if (numbers.find(number)!=numbers.end()) {
        numbers[number]--;
        if (numbers[number]==0) {
          numbers.erase(number);
        }
      }
      names.erase(it);
    }
    return cmdQuery(cmd,reply,remote);
  }

  virtual bool cmdRegister(yarp::os::Bottle& cmd,
                           yarp::os::Bottle& reply,
                           yarp::os::Contact& remote) {
    std::string name = cmd.get(1).asString();
    std::string carrier = cmd.get(2).asString();
    std::string machine = cmd.get(3).asString();
    int number = cmd.get(4).asInt32();
    if (name=="...") {
      name = "/tmp/";
      for (int i=0; i<20; i++) {
        double x = Rand::scalar('a','z'+1);
        name = name + (char)x;
      }
    }
    if (carrier==""||carrier=="...") {
      carrier = "tcp";
    }
    if (machine==""||machine=="...") {
      machine = remote.getHost();
    }
    if (number==0) {
      number = allocatePortNumber();
      if (number==0) {
        printf("Out of port numbers!\n");
      } else {
        lastNumber = number;
      }
    }
    if (numbers.find(number)==numbers.end()) {
      numbers[number] = 1;
    } else {
      numbers[number]++;
    }
    Entry entry;
    entry.name = name.c_str();
    entry.carrier = carrier.c_str();
    entry.machine = machine.c_str();
    entry.portNumber = number;
    names[name.c_str()] = entry;
    Bottle cmd2;
    cmd2.add("query");
    cmd2.add(name.c_str());
    return cmdQuery(cmd2,reply,remote);
  }

  virtual bool apply(yarp::os::Bottle& cmd,
                     yarp::os::Bottle& reply,
                     yarp::os::Bottle& event,
                     yarp::os::Contact& remote) {
    bool ok = false;
    mutex.lock();
    printf(" + %s\n", cmd.toString().c_str());
    reply.clear();
    std::string tag = cmd.get(0).asString();
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
    mutex.unlock();
    return ok;
  }
};


int main(int argc, char *argv[]) {
  Network yarp;
  Rand::init();
  yarp.setLocalMode(true);
  Property config;
  config.fromCommand(argc,argv);

  Contact contact = Contact("/root", "tcp", "localhost", DEFAULT_NAME_PORT_NUMBER);

  WideNameService wide;
  NameServerManager manager(wide);
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

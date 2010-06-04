#include <stdio.h>
#include <yarp/os/all.h>

#include <string>

using namespace yarp::os;
using namespace std;

#define ROSCORE_PORT "/roscore"

void usage(const char *action,
	   const char *msg,
	   const char *example = NULL) {
  printf("\n  yarpros %s\n", action);
  printf("  -- %s\n", msg);
  if (example!=NULL) {
    printf("  -- example: yarpros %s\n", example);
  }
}

bool rpc(const char *target,
	 const char *carrier,
	 PortWriter& writer,
	 PortReader& reader) {
  Port p;
  Network::setVerbosity(-1);
  p.open("...");
  bool ok = Network::connect(p.getName(),target,carrier);
  if (ok) {
    ok = p.write(writer,reader);
  }
  p.close();
  Network::setVerbosity(0);
  return ok;
}

bool register_port(const char *name,
		   const char *hostname,
		   int portnum,
		   PortReader& reply) {
    Bottle req;
    req.addString("register");
    req.addString(name);
    req.addString("tcp");
    req.addString(hostname);
    req.addInt(portnum);
    return Network::write(Network::getNameServerContact(),
			  req,
			  reply);
}

int main(int argc, char *argv[]) {
  if (argc<=1) {
    printf("Hello, good evening, and welcome to yarpros\n");
    printf("Here are some things you can do:\n");
    usage("roscore <hostname> <port number>","tell yarp how to reach the ros master","roscore 192.168.0.1 11311");
    usage("import <name>","import a ROS name into YARP","import /talker");
    return 0;
  }

  Network yarp;

  Bottle cmd;
  for (int i=1; i<argc; i++) {
    Value v;
    v.fromString(argv[i]);
    cmd.add(v);
  }
  
  ConstString tag = cmd.get(0).asString();

  if (tag=="roscore") {
    if (!(cmd.get(1).isString()&&cmd.get(2).isInt())) {
      fprintf(stderr,"wrong syntax, run with no arguments for help\n");
      return 1;
    }
    Bottle reply;
    register_port(ROSCORE_PORT, cmd.get(1).asString(), cmd.get(2).asInt(), 
		  reply);
    printf("%s\n", reply.toString().c_str());
    return 0;
  } else if (tag=="import") {
    if (!cmd.get(1).isString()) {
      fprintf(stderr,"wrong syntax, run with no arguments for help\n");
      return 1;
    }
    Bottle req, reply;
    req.addString("lookupNode");
    req.addString("dummy_id");
    req.add(cmd.get(1));
    rpc(ROSCORE_PORT, "xmlrpc", req, reply);
    if (reply.get(0).asInt()!=1) {
      fprintf(stderr, "Failure: %s\n", reply.toString().c_str());
      return 1;
    }
    string url = reply.get(2).asString().c_str();
    size_t break1 = url.find_first_of("://",0);
    if (break1==string::npos) {
      fprintf(stderr, "url not understood: %s\n", url.c_str());
      return 1;
    }
    size_t break2 = url.find_first_of(":",break1+3);
    if (break2==string::npos) {
      fprintf(stderr, "url not understood: %s\n", url.c_str());
      return 1;
    }
    size_t break3 = url.find_first_of("/",break2+1);
    if (break3==string::npos) {
      fprintf(stderr, "url not understood: %s\n", url.c_str());
      return 1;
    }
    string hostname = url.substr(break1+3,break2-break1-3);
    Value portnum;
    portnum.fromString(url.substr(break2+1,break3-break2-1).c_str());
    register_port(cmd.get(1).asString().c_str(),
		  hostname.c_str(),
		  portnum.asInt(),
		  reply);
    printf("%s\n", reply.toString().c_str());
  }
  
  return 0;
}


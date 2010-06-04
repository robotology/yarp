#include <stdio.h>
#include <yarp/os/all.h>

#include <string>

using namespace yarp::os;
using namespace std;

#define ROSCORE_PORT "/roscore"

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


class RosLookup {
public:
  bool valid;
  string hostname;
  int portnum;
  string protocol;

  RosLookup() {
    valid = false;
  }

  bool lookupCore(const char *name);

  bool lookupTopic(const char *name);

  string toString() {
    char buf[1000];
    sprintf(buf,"/%s:%d/", hostname.c_str(), portnum);
    return buf;
  }

};


bool RosLookup::lookupCore(const char *name) {
    Bottle req, reply;
    req.addString("lookupNode");
    req.addString("dummy_id");
    req.addString(name);
    rpc(ROSCORE_PORT, "xmlrpc", req, reply);
    if (reply.get(0).asInt()!=1) {
      fprintf(stderr, "Failure: %s\n", reply.toString().c_str());
      return false;
    }
    string url = reply.get(2).asString().c_str();
    size_t break1 = url.find_first_of("://",0);
    if (break1==string::npos) {
      fprintf(stderr, "url not understood: %s\n", url.c_str());
      return false;
    }
    size_t break2 = url.find_first_of(":",break1+3);
    if (break2==string::npos) {
      fprintf(stderr, "url not understood: %s\n", url.c_str());
      return false;
    }
    size_t break3 = url.find_first_of("/",break2+1);
    if (break3==string::npos) {
      fprintf(stderr, "url not understood: %s\n", url.c_str());
      return false;
    }
    hostname = url.substr(break1+3,break2-break1-3);
    Value vportnum;
    vportnum.fromString(url.substr(break2+1,break3-break2-1).c_str());
    portnum = vportnum.asInt();
    printf("%s\n", reply.toString().c_str());
    valid = (portnum!=0);
    rpc(ROSCORE_PORT, "xmlrpc", req, reply);
    return valid;
}


bool RosLookup::lookupTopic(const char *name) {
  if (!valid) {
    fprintf(stderr, "Need a node\n");
    return false;
  }
  Bottle req, reply;
  req.addString("requestTopic");
  req.addString("dummy_id");
  req.addString(name);
  Bottle& lst = req.addList();
  Bottle& sublst = lst.addList();
  sublst.addString("TCPROS");
  rpc(toString().c_str(),"xmlrpc",req,reply);
  if (reply.get(0).asInt()!=1) {
    printf("Failure looking up topic %s: %s\n", name, reply.toString().c_str());
    return false;
  }
  Bottle *pref = reply.get(2).asList();
  if (pref==NULL) {
    printf("Failure looking up topic %s: expected list of protocols\n", name);
    return false;
  }
  if (pref->get(0).asString()!="TCPROS") {
    printf("Failure looking up topic %s: unsupported protocol %s\n", name,
	   pref->get(0).asString().c_str());
    return false;
  }
  Value hostname2 = pref->get(1);
  Value portnum2 = pref->get(2);
  hostname = hostname2.asString().c_str();
  portnum = portnum2.asInt();
  protocol = "tcpros";
  printf("topic %s available at %s:%d\n", name, hostname.c_str(), portnum);
  return true;
}


void usage(const char *action,
	   const char *msg,
	   const char *example = NULL) {
  printf("\n  yarpros %s\n", action);
  printf("  -- %s\n", msg);
  if (example!=NULL) {
    printf("  -- example: yarpros %s\n", example);
  }
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
    usage("read <yarpname> <nodename> <topicname>","read to a YARP port from a ROS node's contribution to a topic","read /read /talker /chatter");
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
    RosLookup lookup;
    bool ok = lookup.lookupCore(cmd.get(1).asString());
    if (ok) {
      register_port(cmd.get(1).asString().c_str(),
		    lookup.hostname.c_str(),
		    lookup.portnum,
		    reply);
      printf("%s\n",reply.toString().c_str());
    }
    return ok?0:1;
  } else if (tag=="read") {
    if (!cmd.size()==4) {
      fprintf(stderr,"wrong syntax, run with no arguments for help\n");
      return 1;
    }
    ConstString yarp_port = cmd.get(1).asString();
    ConstString ros_port = cmd.get(2).asString();
    ConstString topic = cmd.get(3).asString();
    RosLookup lookup;
    bool ok = lookup.lookupCore(ros_port.c_str());
    if (!ok) return 1;
    ok = lookup.lookupTopic(topic.c_str());
    if (!ok) return 1;
    yarp.connect(yarp_port.c_str(),
		 lookup.toString().c_str(),
		 (string("tcpros+topic.")+topic.c_str()).c_str());
    return ok?0:1;
  } else {
      fprintf(stderr,"unknown command, run with no arguments for help\n");
      return 1;
  }
  
  return 0;
}


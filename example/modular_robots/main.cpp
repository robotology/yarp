#include <yarp/os/all.h>

using namespace yarp::os;

#define DEFAULT_NAME_PORT_NUMBER 10000

class YarpNeighbors {
private:
  ConstString prefix;
public:
  
};

int main(int argc, char *argv[]) {
  Network yarp;
  yarp.setLocalMode(true);
  Property config;
  config.fromCommand(argc,argv);

  Contact c = Contact::bySocket("tcp","localhost",DEFAULT_NAME_PORT_NUMBER);
  c = c.addName("/root");
  Port p;
  p.open(c,false);
  Time::delay(10);

  return 0;
}


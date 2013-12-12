#include <stdio.h>
#include <yarp/os/all.h>

using namespace yarp::os;

int main(int argc, char *argv[]) {
  Network yarp;
  Port port;
  port.setReadOnly();
  if (!port.open("/chatter@/yarp/listener")) {
    fprintf(stderr,"Failed to open port\n");
    return 1;
  }

  while (true) {
    Bottle msg;
    if (!port.read(msg)) {
      fprintf(stderr,"Failed to read msg\n");
      continue;
    }
    printf("Got [%s]\n", msg.get(0).asString().c_str());
  }
  
  return 0;
}

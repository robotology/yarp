#include <stdio.h>
#include <yarp/os/all.h>

using namespace yarp::os;

int main(int argc, char *argv[]) {
  Network yarp;
  Port port;
  port.setWriteOnly();
  if (!port.open("/chatter@/yarp/talker")) {
    fprintf(stderr,"Failed to open port\n");
    return 1;
  }

  for (int i=0; i<1000; i++) {
    char buf[256];
    sprintf(buf,"hello ros %d", i);
    Bottle msg;
    msg.addString(buf);
    port.write(msg);
    printf("Wrote: [%s]\n", buf);
    Time::delay(1);
  }

  return 0;
}

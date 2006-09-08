#include <stdio.h>
#include <math.h>

#include <yarp/os/all.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/AudioGrabberInterfaces.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

int main(int argc, char *argv[]) {
  Network::init();

  // Get an audio device.

  Property p;
  if (argc>1) {
    p.fromCommand(argc,argv);
  }
  if (!p.check("device")) {
    p.put("device","portaudio");
    p.put("write",1);
    p.put("delay",1);
  }
  PolyDriver poly(p);
  if (!poly.isValid()) {
    printf("cannot open driver\n");
    return 1;
  }


  // Make sure we can write sound
  IAudioRender *put;
  poly.view(put);
  if (put==NULL) {
    printf("cannot open interface\n");
    return 1;
  }


  // send to speaker
  BufferedPort<Sound> port;
  port.open(p.check("name",Value("/yarphear")).asString());

  int ct = 0;
  while (true) {
    Sound *snd = port.read();
    if (snd==NULL) continue;
    put->renderSound(*snd);
    ct++;
    //printf("count is %d\n", ct);
  }

  Network::fini();

  return 0;
}


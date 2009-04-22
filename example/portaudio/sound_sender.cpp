#include <stdio.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/AudioGrabberInterfaces.h>
#include <yarp/os/Property.h>

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

int main(int argc, char *argv[]) {

    // Open the network
    Network yarp;
    BufferedPort<Sound> p;
    p.open("/sender");

    // Get an audio read device.
    Property conf;
    conf.put("device","portaudio");
    conf.put("read", 1);
    PolyDriver poly(conf);
    IAudioGrabberSound *get;

    // Make sure we can read sound
    poly.view(get);
    if (get==NULL) {
        printf("cannot open interface\n");
        return 1;
    }

    //Grab and send
    while (true)
      {
	Sound& s = p.prepare();
	get->getSound(s);
	p.write();
      }
    return 0;
}


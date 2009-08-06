#include <stdio.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/AudioGrabberInterfaces.h>
 
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

int main(int argc, char *argv[]) {

    // Get an audio device.
    PolyDriver poly("portaudio");
    IAudioGrabberSound *get;
    IAudioRender *put;

    // Make sure we can both read and write sound
    poly.view(get);
    poly.view(put);
    if (get==NULL&&put==NULL) {
        printf("cannot open interface\n");
        return 1;
    }

    //Grab and render 
    Sound s;
    while (true)
      {
	get->getSound(s);
	put->renderSound(s);
      }
    return 0;
}


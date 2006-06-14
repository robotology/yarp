// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <stdio.h>
#include <stdlib.h>

#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Time.h>
#include <yarp/os/Port.h>
#include <yarp/sig/Image.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/ControlBoardInterfaces.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

// a test remote frame grabber - provides blank images.
// only supports streaming out, no requests.
class FakeFrameGrabber : public Thread {
private:
    Port p;
public:
    virtual void beforeStart() {
        p.open("/server");
    }

    virtual void run() {
        printf("Fake framegrabber starting\n");
        while (!isStopping()) {
            ImageOf<PixelRgb> img;
            img.resize(40,20);
            img.zero();
            printf("Fake framegrabber wrote an image...\n");
            p.write(img);
            Time::delay(1);
        }
        printf("Fake framegrabber stopping\n");
    }
};


int main() {
    Network::setLocalMode(true);

    FakeFrameGrabber fake;
    fake.start();
    
    PolyDriver dd("(device remote_framegrabber) (local /client) (remote /server)");
    
    if (!dd.isValid()) {
        printf("Device not available\n");
        printf("(If this is June 2006, you need to delete cache and run cmake again, sorry)\n");
        exit(1);
    }
    
    printf("*** Device created\n");
    
    IFrameGrabberImage *grabber;
    dd.view(grabber);
    
    if (grabber!=NULL) {
        printf("*** It can supply images\n");
        ImageOf<PixelRgb> img;
        if (grabber->getImage(img)) {
            printf("*** Got a %dx%d image\n", img.width(), img.height());
        } else {
            printf("*** Failed to actually read an image\n");
        }
    } else {
        printf("*** It can <<<<<NOT>>>>> supply images\n");
    }
    
    IPidControl *pid;
    dd.view(pid);
    
    if (pid!=NULL) {
        printf("*** It can do PID control\n");
    } else {
        printf("*** It can <<<<<NOT>>>>> do PID control\n");
    }

    return 0;
}




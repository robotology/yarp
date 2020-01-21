/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/all.h>
#include <yarp/sig/all.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::sig::draw;

/*
  This example adds a moving circle to an image stream.
  It is the same as image_process.cpp, except it is built
  as a Yarp "Module".  This makes it a bit cleaner to start/stop.


  Suppose we have an image source on port /source such as:
    yarpdev --device test_grabber --name /source --mode line --framerate 10

  And suppose we have an image viewer on port /view:
    yarpview --name /view

  Then we can hook this program up in between as follows:
    ./image_process --name /worker
    yarp connect /source /worker
    yarp connect /worker /view

  You should see the normal scrolling line of test_grabber, with a moving
  circle overlaid.

 */


class ImageProcessModule : public RFModule {
private:
    // Make a port for reading and writing images
    BufferedPort<ImageOf<PixelRgb> > port;
    Port cmdPort;
    int ct;
public:
    bool open(Searchable& config) {
        ct = 0;
        port.open(getName());
        cmdPort.open(getName("cmd")); // optional command port
        attach(cmdPort); // cmdPort will work just like terminal
        return true;
    }

    // try to interrupt any communications or resource usage
    bool interruptModule() {
        port.interrupt();
        return true;
    }

    bool updateModule() {
        ImageOf<PixelRgb> *img = port.read();
        if (img==NULL) return false;;

        // add a blue circle
        PixelRgb blue(0,0,255);
        addCircle(*img,blue,ct,50,10);
        ct = (ct+5)%img->width();

        // output the image
        port.prepare() = *img;
        port.write();

        return true;
    }
};


int main(int argc, char *argv[]) {
    // Initialize the yarp network
    Network yarp;

    /* prepare and configure the resource finder */
    ResourceFinder rf;
    rf.configure(argc, argv);
    rf.setVerbose(true);

    // Create and run our module
    ImageProcessModule module;
    module.setName("/worker");
    return module.runModule(rf);
}

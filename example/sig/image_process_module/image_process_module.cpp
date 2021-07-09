/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/ResourceFinder.h>

#include <yarp/sig/Image.h>
#include <yarp/sig/ImageDraw.h>

using yarp::os::BufferedPort;
using yarp::os::Network;
using yarp::os::Port;
using yarp::os::ResourceFinder;
using yarp::os::RFModule;
using yarp::os::Searchable;

using yarp::sig::ImageOf;
using yarp::sig::PixelRgb;
using yarp::sig::draw::addCircle;

/*
  This example adds a moving circle to an image stream.
  It is the same as image_process.cpp, except it is built
  as a YARP "Module".  This makes it a bit cleaner to start/stop.


  Suppose we have an image source on port /source such as:
    yarpdev --device fakeFrameGrabber --name /source --mode line --framerate 10

  And suppose we have an image viewer on port /view:
    yarpview --name /view

  Then we can hook this program up in between as follows:
    ./image_process --name /worker
    yarp connect /source /worker
    yarp connect /worker /view

  You should see the normal scrolling line of fakeFrameGrabber, with a moving
  circle overlaid.

 */


class ImageProcessModule : public RFModule
{
private:
    // Make a port for reading and writing images
    BufferedPort<ImageOf<PixelRgb>> port;
    Port cmdPort;
    size_t ct;

public:
    bool open(Searchable& config)
    {
        YARP_UNUSED(config);

        ct = 0;
        port.open(getName());
        cmdPort.open(getName("cmd")); // optional command port
        attach(cmdPort);              // cmdPort will work just like terminal
        return true;
    }

    // try to interrupt any communications or resource usage
    bool interruptModule() override
    {
        port.interrupt();
        return true;
    }

    bool updateModule() override
    {
        ImageOf<PixelRgb>* img = port.read();
        if (img == nullptr) {
            return false;
        }

        // add a blue circle
        PixelRgb blue{0, 0, 255};
        addCircle(*img, blue, ct, 50, 10);
        ct = (ct + 5) % img->width();

        // output the image
        port.prepare() = *img;
        port.write();

        return true;
    }
};


int main(int argc, char* argv[])
{
    // Initialize the yarp network
    Network yarp;

    /* prepare and configure the resource finder */
    auto& rf = ResourceFinder::getResourceFinderSingleton();
    rf.configure(argc, argv);

    // Create and run our module
    ImageProcessModule module;
    module.setName("/worker");
    return module.runModule(rf);
}

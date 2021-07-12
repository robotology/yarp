/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/Value.h>

#include <yarp/sig/Image.h>
#include <yarp/sig/ImageDraw.h>

using yarp::os::BufferedPort;
using yarp::os::Network;
using yarp::os::Property;
using yarp::os::Value;
using yarp::sig::ImageOf;
using yarp::sig::PixelRgb;
using yarp::sig::draw::addCircle;

/*
  This example adds a moving circle to an image stream.

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

int main(int argc, char* argv[])
{
    // Initialize network
    Network yarp;

    // Make a port for reading and writing images
    BufferedPort<ImageOf<PixelRgb>> port;

    // Get command line options
    Property options;
    options.fromCommand(argc, argv);

    // Set the name of the port (use "/worker" if there is no --name option)
    std::string portName = options.check("name", Value("/worker")).asString();
    port.open(portName);

    size_t ct = 0;
    while (true) {
        // read an image from the port
        ImageOf<PixelRgb>* img = port.read();
        if (img == nullptr) {
            continue;
        }

        // add a blue circle
        PixelRgb blue{0, 0, 255};
        addCircle(*img, blue, ct, 50, 10);
        ct = (ct + 5) % img->width();

        // output the image
        port.prepare() = *img;
        port.write();
    }

    return 0;
}

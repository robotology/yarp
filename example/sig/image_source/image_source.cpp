/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Time.h>
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

int main(int argc, char* argv[])
{
    Network yarp;
    BufferedPort<ImageOf<PixelRgb>> port;

    Property options;
    options.fromCommand(argc, argv);
    port.open(options.check("name", Value("/image")).asString());

    size_t ct = 0;
    while (true) {
        ImageOf<PixelRgb>& img = port.prepare();
        img.resize(100, 100);
        img.zero();
        PixelRgb blue{0, 0, 255};
        addCircle(img, blue, ct, 50, 10);
        ct = (ct + 5) % 100;
        port.write();

        constexpr double loop_delay = 0.25;
        yarp::os::Time::delay(loop_delay);
    }

    return 0;
}

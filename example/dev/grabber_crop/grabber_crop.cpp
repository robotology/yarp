/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Property.h>
#include <yarp/sig/Image.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IFrameGrabberImage.h>


int main(int argc, char* argv[])
{
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);
    using ImageType = yarp::sig::ImageOf<yarp::sig::PixelRgb>;

    yarp::os::Network yarp;

    yarp::os::BufferedPort<ImageType> out;
    out.open("/test_yarp_crop");

    yarp::dev::PolyDriver dd;
    yarp::os::Property p;
    p.put("device", "fakeFrameGrabber");
    p.put("mode", "grid");
    dd.open(p);
    yarp::dev::IFrameGrabberImage *grabber = nullptr;
    dd.view(grabber);

    const yarp::sig::VectorOf<std::pair< int, int>> vertices {{50,50}, {149, 149}};

    while (true) {
        auto& crop = out.prepare();
        grabber->getImageCrop(YARP_CROP_RECT, vertices, crop);
        out.write();
    }

    return 0;
}

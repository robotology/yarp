/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/dev/IFrameGrabberImageRaw.h>
#include <yarp/sig/ImageUtils.h>
#include <yarp/os/LogComponent.h>

namespace {
YARP_LOG_COMPONENT(IFRAMEGRABBERIMAGERAW, "yarp.dev.IFrameGrabberImageRaw")
}

yarp::dev::IFrameGrabberImageRaw::~IFrameGrabberImageRaw() = default;

bool yarp::dev::IFrameGrabberImageRaw::getImageCrop(cropType_id_t cropType,
                                                    yarp::sig::VectorOf<std::pair<int, int>> vertices,
                                                    yarp::sig::ImageOf<yarp::sig::PixelMono>& image)
{
    if (cropType == YARP_CROP_RECT) {
        if (vertices.size() != 2) {
            yCError(IFRAMEGRABBERIMAGERAW, "GetImageCrop failed: RECT mode requires 2 vertices");
            return false;
        }
        yarp::sig::ImageOf<yarp::sig::PixelMono> full;
        getImage(full);

        if (!yarp::sig::utils::cropRect(full, vertices[0], vertices[1], image)) {
            yCError(IFRAMEGRABBERIMAGERAW, "GetImageCrop failed: utils::cropRect error");
            return false;
        }
    } else if(cropType == YARP_CROP_LIST) {
        yCError(IFRAMEGRABBERIMAGERAW, "List type not yet implemented");
        return false;
    }

    return true;
}

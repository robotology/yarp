/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/dev/IFrameGrabberImage.h>
#include <yarp/sig/ImageUtils.h>
#include <yarp/os/LogComponent.h>

namespace {
YARP_LOG_COMPONENT(IFRAMEGRABBERIMAGE, "yarp.dev.IFrameGrabberImage")
}

yarp::dev::IFrameGrabberImage::~IFrameGrabberImage() = default;

bool yarp::dev::IFrameGrabberImage::getImageCrop(cropType_id_t cropType,
                                                 yarp::sig::VectorOf<std::pair<int, int>> vertices,
                                                 yarp::sig::ImageOf<yarp::sig::PixelRgb>& image)
{
    if (cropType == YARP_CROP_RECT) {
        if (vertices.size() != 2) {
            yCError(IFRAMEGRABBERIMAGE, "GetImageCrop failed: RECT mode requires 2 vertices");
            return false;
        }
        yarp::sig::ImageOf<yarp::sig::PixelRgb> full;
        getImage(full);

        if (!yarp::sig::utils::cropRect(full, vertices[0], vertices[1], image)) {
            yCError(IFRAMEGRABBERIMAGE, "GetImageCrop failed: utils::cropRect error");
            return false;
        }
    } else if(cropType == YARP_CROP_LIST) {
        yCError(IFRAMEGRABBERIMAGE, "List type not yet implemented");
        return false;
    }

    return true;
}

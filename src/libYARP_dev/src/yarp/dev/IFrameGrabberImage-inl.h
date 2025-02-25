/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IFRAMEGRABBERIMAGE_INL_H
#define YARP_DEV_IFRAMEGRABBERIMAGE_INL_H

#include <yarp/sig/ImageUtils.h>
#include <yarp/os/LogComponent.h>

YARP_DECLARE_LOG_COMPONENT(IFRAMEGRABBEROF)

namespace yarp::dev {

template <typename ImageType>
yarp::dev::ReturnValue IFrameGrabberOf<ImageType>::getImageCrop(cropType_id_t cropType,
                                              yarp::sig::VectorOf<std::pair<int, int>> vertices,
                                              ImageType& image)
{
    if (cropType == YARP_CROP_RECT) {
        if (vertices.size() != 2) {
            yCError(IFRAMEGRABBEROF, "GetImageCrop failed: RECT mode requires 2 vertices");
            return ReturnValue::return_code::return_value_error_method_failed;
        }
        ImageType full;
        yarp::dev::ReturnValue b = getImage(full);
        if (!b || full.width() == 0 || full.height() == 0)
        {
            yCError(IFRAMEGRABBEROF, "GetImageCrop failed: No image received");
            return ReturnValue::return_code::return_value_error_generic;
        }

        if (!yarp::sig::utils::cropRect(full, vertices[0], vertices[1], image)) {
            yCError(IFRAMEGRABBEROF, "GetImageCrop failed: utils::cropRect error: (%d, %d) (%d, %d)",
                    vertices[0].first,
                    vertices[0].second,
                    vertices[1].first,
                    vertices[1].second);
            return ReturnValue::return_code::return_value_error_method_failed;
        }
    } else if(cropType == YARP_CROP_LIST) {
        yCError(IFRAMEGRABBEROF, "List type not yet implemented");
        return ReturnValue::return_code::return_value_error_not_implemented_by_device;
    }

    return ReturnValue_ok;
}

} // namespace yarp::dev

#endif // YARP_DEV_IFRAMEGRABBERIMAGE_INL_H

/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */


#ifndef YARP_DEV_IFRAMESET_H
#define YARP_DEV_IFRAMESET_H

#include "api.h"
#include <yarp/math/FrameTransform.h>
#include <string>
#include <vector>

namespace yarp
{
    namespace dev
    {
        class IFrameSet;
    }
}

class YARP_dev_API yarp::dev::IFrameSet
{
public:
    /**
     Removes all the registered transforms.
    * @return true if there where some frame, false otherwise
    */
    virtual bool clear () = 0;

    /**
     Register a FrameTransform.
    * @param frame the FrameTransform to set.
    * @return false if the frame existed, true otherwise.
    */
    virtual bool setTransform (const yarp::math::FrameTransform& frame) = 0;

    /**
     Register multiple FrameTransforms.
    * @param frames the FrameTransform vector
    * @param append set it to true to append frames to previously created frames. WARNING no concistency check will be performed in this case.
    */
    virtual void setTransforms (const std::vector<yarp::math::FrameTransform>& frames, bool append = false) = 0;

    /**
     Deletes a transform between a frame and his parent.
    * @param frame_id the name of the frame
    * @return true if the frame existed, false otherwise
    */
    virtual bool deleteTransform (const std::string &frame_id) = 0;
};
#endif //YARP_DEV_IFRAMESET_H

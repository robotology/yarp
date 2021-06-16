/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_FRAMETRANSFORMREPEATER_H
#define YARP_DEV_FRAMETRANSFORMREPEATER_H


#include <yarp/os/Network.h>
#include <yarp/dev/IFrameTransformStorage.h>
#include <frameTransformUtils.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/DeviceDriver.h>
#include <mutex>
#include <map>

class FrameTransformRepeater :
    public yarp::dev::DeviceDriver,
    public yarp::dev::IFrameTransformStorageSet,
    public yarp::dev::IFrameTransformStorageGet
{
private:
    mutable std::mutex       m_trf_mutex;
    FrameTransformContainer  m_ftContainer;

public:
    FrameTransformRepeater();
    ~FrameTransformRepeater() {}

    //DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    //IFrameTransformStorageSet interface
    bool setTransforms(const std::vector<yarp::math::FrameTransform>& transforms) override;
    bool setTransform(const yarp::math::FrameTransform& transform) override;

    //IFrameTransformStorageGet interface
    bool getTransforms(std::vector<yarp::math::FrameTransform>& transforms) const override;

};

#endif // YARP_DEV_FRAMETRANSFORMREPEATER_H

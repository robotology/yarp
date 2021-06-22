/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

/*
 * \section FrameTransformSetMultiplexer_device_parameters Description of input parameters
 * This device is a simple multiplexer of devices that implements the IFrameTransformStorageSet interface.
 * It can be attached to multiple devices and when the functions setTransform or setTransforms are called
 * they are forwarded to all the devices attached.
 * For how to attach the various devices see FrameTransformClient and FrameTransformServer.
 */

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IFrameTransformStorage.h>
#include <yarp/dev/WrapperMultiple.h>
#include <yarp/dev/PolyDriver.h>

#ifndef YARP_DEV_FRAMETRANSFORMSETMULTIPLEXER_H
#define YARP_DEV_FRAMETRANSFORMSETMULTIPLEXER_H

class FrameTransformSetMultiplexer:
    public yarp::dev::DeviceDriver,
    public yarp::dev::IFrameTransformStorageSet,
    public yarp::dev::WrapperMultiple
{
public:
    FrameTransformSetMultiplexer() = default;
    FrameTransformSetMultiplexer(const FrameTransformSetMultiplexer&) = delete;
    FrameTransformSetMultiplexer(FrameTransformSetMultiplexer&&) = delete;
    FrameTransformSetMultiplexer& operator=(const FrameTransformSetMultiplexer&) = delete;
    FrameTransformSetMultiplexer& operator=(FrameTransformSetMultiplexer&&) = delete;

    // yarp::dev::DeviceDriver
    bool open (yarp::os::Searchable &config) override;
    bool close () override;

    // yarp::dev::IMultipleWrapper
    bool attachAll(const yarp::dev::PolyDriverList& devices2attach) override;
    bool detachAll() override;

    // yarp::dev::IFrameTransformStorageSet
    bool setTransform(const yarp::math::FrameTransform& transform) override;
    bool setTransforms(const std::vector<yarp::math::FrameTransform>& transforms) override;

private:
    int    m_verbose{4};
    std::vector<IFrameTransformStorageSet*> m_iFrameTransformStorageSetList;
    std::vector<std::vector<yarp::math::FrameTransform>> m_transformVector;
    std::vector<std::mutex> m_mutexSettingFromAttachedDevices;
};


#endif // YARP_DEV_FRAMETRANSFORMSETMULTIPLEXER_H

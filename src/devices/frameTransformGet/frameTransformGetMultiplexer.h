/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

/*
 * \section FrameTransformGetMultiplexer_device_parameters Description of input parameters
 * This device is a simple multiplexer of devices that implements the IFrameTransformStorageGet interface.
 * It can be attached to multiple devices and when the function getTransforms is called it returns a
 * vector containing all the transforms taken from the attached devices.
 * For how to attach the various devices see FrameTransformClient and FrameTransformServer.
 */

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IFrameTransformStorage.h>
#include <yarp/dev/WrapperMultiple.h>
#include <yarp/dev/PolyDriver.h>

#ifndef YARP_DEV_FRAMETRANSFORMGETMULTIPLEXER_H
#define YARP_DEV_FRAMETRANSFORMGETMULTIPLEXER_H

class FrameTransformGetMultiplexer:
    public yarp::dev::DeviceDriver,
    public yarp::dev::IFrameTransformStorageGet,
    public yarp::dev::WrapperMultiple
{
public:
    FrameTransformGetMultiplexer() = default;
    FrameTransformGetMultiplexer(const FrameTransformGetMultiplexer&) = delete;
    FrameTransformGetMultiplexer(FrameTransformGetMultiplexer&&) = delete;
    FrameTransformGetMultiplexer& operator=(const FrameTransformGetMultiplexer&) = delete;
    FrameTransformGetMultiplexer& operator=(FrameTransformGetMultiplexer&&) = delete;

    // yarp::dev::DeviceDriver
    bool open (yarp::os::Searchable &config) override;
    bool close () override;

    // yarp::dev::IMultipleWrapper
    bool attachAll(const yarp::dev::PolyDriverList& devices2attach) override;
    bool detachAll() override;

    // yarp::dev::IFrameTransformStorageGet
    bool getTransforms(std::vector<yarp::math::FrameTransform>& transforms) const override;

    // yarp::os::PeriodicThread

private:
    int    m_verbose{4};
    std::vector<IFrameTransformStorageGet*> m_iFrameTransformStorageGetList;
    std::vector<std::vector<yarp::math::FrameTransform>> m_transformVector;
    std::vector<std::mutex> m_mutexGettingFromAttachedDevices;
};


#endif // YARP_DEV_FRAMETRANSFORMGETMULTIPLEXER_H

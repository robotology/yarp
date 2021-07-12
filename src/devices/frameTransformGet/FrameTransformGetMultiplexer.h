/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @brief A multiplexer which collapses the output of many IFrameTransformStorageGet interfaces into one.
 *
 * This device is a multiplexer of devices that implements the IFrameTransformStorageGet interface.
 * It can be attached to multiple devices. When the function getTransforms is called, it returns a
 * vector containing all the transforms taken from the attached devices.
 * For further information see \subpage FrameTransform.
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

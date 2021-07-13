/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @brief A multiplexer which forwards the input of one IFrameTransformStorageSet interfaces to many.
 *
 * This device is a simple multiplexer of devices that implements the IFrameTransformStorageSet interface.
 * It can be attached to multiple devices and when the functions setTransform or setTransforms are called
 * they are forwarded to all the devices attached.\n
 * For further information see \subpage FrameTransform.
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
    virtual bool deleteTransform(std::string t1, std::string t2) override;
    virtual bool clearAll() override;

private:
    int    m_verbose{4};
    std::vector<IFrameTransformStorageSet*> m_iFrameTransformStorageSetList;
    std::vector<std::vector<yarp::math::FrameTransform>> m_transformVector;
    std::vector<std::mutex> m_mutexSettingFromAttachedDevices;
};


#endif // YARP_DEV_FRAMETRANSFORMSETMULTIPLEXER_H

/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_FRAMETRANSFORM_UTILS_H
#define YARP_DEV_FRAMETRANSFORM_UTILS_H


#include <yarp/os/Network.h>
#include <yarp/dev/IFrameTransformStorage.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IMultipleWrapper.h>
#include <mutex>
#include <map>

/**
 *  @ingroup dev_impl_other
 *
 * \brief `FrameTransformContainer`: A class that contains a vector of frame transformations and exposes
 *  \ref yarp::dev::IFrameTransformStorageSet and \ref yarp::dev::IFrameTransformStorageGet
 *  interfaces in order to allow external access to it.
 *
 */

class FrameTransformContainer :
    public yarp::dev::IFrameTransformStorageSet,
    public yarp::dev::IFrameTransformStorageGet
{
    using ContainerType = std::vector<yarp::math::FrameTransform>;

protected:
    ContainerType m_transforms;

public:
    mutable std::recursive_mutex  m_trf_mutex;

public:
    //non-static transforms older than value (seconds) will be removed by method checkAndRemoveExpired()
    double m_timeout = 0.2;

public:
    FrameTransformContainer() {}
    ~FrameTransformContainer() {}

    //IFrameTransformStorageSet interface
    bool setTransforms(const std::vector<yarp::math::FrameTransform>& transforms) override;
    bool setTransform(const yarp::math::FrameTransform& transform) override;

    //IFrameTransformStorageGet interface
    bool getTransforms(std::vector<yarp::math::FrameTransform>& transforms) const override;

    //IFrameTransformStorageUtils interface
    bool deleteTransform(std::string t1, std::string t2) override;
    bool clearAll() override;

    bool size(size_t& size) const;

public:
    //other
    bool checkAndRemoveExpired();
    bool checkAndRemoveExpired() const;
    ContainerType::iterator begin() {return m_transforms.begin();}
    ContainerType::iterator end()   {return m_transforms.end();}

    //yarp::math::FrameTransform& operator[]   (std::size_t idx) { return m_transforms[idx]; }
    //bool     delete_transform(int id);
};

#endif // YARP_DEV_FRAMETRANSFORM_UTILS_H

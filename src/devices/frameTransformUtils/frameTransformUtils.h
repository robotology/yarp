/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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

class FrameTransformContainer :
    public yarp::dev::IFrameTransformStorageSet,
    public yarp::dev::IFrameTransformStorageGet,
    public yarp::dev::IFrameTransformStorageUtils
{
protected:
    std::vector<yarp::math::FrameTransform> m_transforms;
    mutable std::mutex  m_trf_mutex;

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
    bool size(size_t& size) const override;
    bool clear() override;

    //other
    //yarp::math::FrameTransform& operator[]   (std::size_t idx) { return m_transforms[idx]; }
    //bool     delete_transform(int id);

};

#endif // YARP_DEV_FRAMETRANSFORM_UTILS_H

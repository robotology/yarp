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

#ifndef YARP_DEV_FRAMETRANSFORMSTORAGE_H
#define YARP_DEV_FRAMETRANSFORMSTORAGE_H


#include <yarp/os/Network.h>
#include <yarp/dev/IFrameTransformStorage.h>
#include <frameTransformContainer.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IMultipleWrapper.h>
#include <mutex>
#include <map>

class FrameTransformStorage :
    public yarp::dev::DeviceDriver,
    public yarp::dev::IFrameTransformStorageSet,
    public yarp::dev::IFrameTransformStorageGet,
    public yarp::dev::IFrameTransformStorageUtils,
    public yarp::os::PeriodicThread,
    public yarp::dev::IMultipleWrapper
{
protected:
    FrameTransformContainer m_tf_container;

public:
    FrameTransformStorage(double tperiod=0.010) : PeriodicThread (tperiod) {}
    ~FrameTransformStorage() {}

    //DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    //IFrameTransformStorageSet interface
    bool setTransforms(const std::vector<yarp::math::FrameTransform>& transforms) override;
    bool setTransform(const yarp::math::FrameTransform& transform) override;

    //IFrameTransformStorageGet interface
    bool getTransforms(std::vector<yarp::math::FrameTransform>& transforms) const override;

    //IFrameTransformStorageUtils interface
    bool deleteTransform(std::string t1, std::string t2) override;
    bool size(size_t& size) const override;
    bool clear() override;
    bool getInternalContainer(FrameTransformContainer*&  container) override;

    //wrapper and interfaces
    mutable std::mutex  m_pd_mutex;
    bool attachAll(const yarp::dev::PolyDriverList& p) override;
    bool detachAll() override;
    yarp::dev::PolyDriverList pDriverList;
    std::vector<IFrameTransformStorageGet*> iGetIf;

    //periodicThread
    void     run() override;
};

#endif // YARP_DEV_FRAMETRANSFORMSTORAGE_H

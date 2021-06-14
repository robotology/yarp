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

#ifndef YARP_DEV_FRAMETRANSFORMSETNWCYARP_H
#define YARP_DEV_FRAMETRANSFORMSETNWCYARP_H


#include <yarp/os/Network.h>
#include <yarp/dev/IFrameTransformStorage.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/PolyDriver.h>
#include "FrameTransformStorageSetRPC.h"
#include <mutex>
#include <map>

class FrameTransformSet_nwc_yarp :
    public yarp::dev::DeviceDriver,
    public yarp::dev::IFrameTransformStorageSet
{
protected:
    mutable std::mutex  m_trf_mutex;

public:
    FrameTransformSet_nwc_yarp();
    ~FrameTransformSet_nwc_yarp() {}

    //DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    //FrameTransformStorageSetRPC functions
    virtual bool setTransforms(const std::vector<yarp::math::FrameTransform>& transforms) override;
    virtual bool setTransform(const yarp::math::FrameTransform& transform) override;

private:
    mutable std::mutex  m_pd_mutex;
    std::string m_thriftPortName;
    yarp::os::Port m_thriftPort;
    FrameTransformStorageSetRPC m_setRPC;
};

#endif // YARP_DEV_FRAMETRANSFORMSETNWCYARP_H

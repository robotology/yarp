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

#ifndef YARP_DEV_FRAMETRANSFORMREPEATER_H
#define YARP_DEV_FRAMETRANSFORMREPEATER_H
#define SIMPLE_PASS


#include <yarp/os/Network.h>
#include <yarp/dev/IFrameTransformStorage.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/DeviceDriver.h>
#include <mutex>
#include <map>

class FrameTransformRepeater :
    public yarp::dev::DeviceDriver,
    public yarp::dev::IFrameTransformStorageSet,
    public yarp::dev::IFrameTransformStorageGet,
    public yarp::os::PeriodicThread
{
private:
    std::vector<yarp::math::FrameTransform> m_transforms;
    mutable std::mutex                      m_trf_mutex;

public:
    FrameTransformRepeater(double tperiod=0.010) : PeriodicThread (tperiod) {}
    ~FrameTransformRepeater() {}

    //DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    //IFrameTransformStorageSet interface
    bool setTransforms(const std::vector<yarp::math::FrameTransform>& transforms) override;
    bool setTransform(const yarp::math::FrameTransform& transform) override;

    //IFrameTransformStorageGet interface
    bool getTransforms(std::vector<yarp::math::FrameTransform>& transforms) const override;

    //periodicThread
    void run() override;
};

#endif // YARP_DEV_FRAMETRANSFORMREPEATER_H

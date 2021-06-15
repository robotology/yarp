/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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

/*
 * \section FrameTransformSet_nwc_yarp_device_parameters Description of input parameters
 * This device is paired with its server called FrameTransformSet_nws_yarp.
 *
 *   Parameters required by this device are:
 * | Parameter name | SubParameter            | Type    | Units          | Default Value             | Required     | Description                            |
 * |:--------------:|:-----------------------:|:-------:|:--------------:|:-------------------------:|:-----------: |:--------------------------------------:|
 * | GENERAL        |      -                  | group   | -              | -                         | No           |                                        |
 * | -              | rpc_port                | string  | -              | /frameTransformSet/rpc    | No           | port on which rpc calls should be made |
 *
 * Some example of configuration files:
 *
 * Example of configuration file using .ini format.
 *
 * \code{.unparsed}
 * device frameTransformSet_nwc_yarp
 * [GENERAL]
 * rpc_port /frameTransformSet/rpc
 * \endcode
 */

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

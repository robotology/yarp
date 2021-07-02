/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_FRAMETRANSFORMSETNWSYARP_H
#define YARP_DEV_FRAMETRANSFORMSETNWSYARP_H


#include <yarp/os/Network.h>
#include <yarp/dev/IFrameTransformStorage.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include "FrameTransformStorageSetRPC.h"
#include <mutex>
#include <map>

/*
 * \section FrameTransformSet_nws_yarp_device_parameters Description of input parameters
 * This device is paired with its server called FrameTransformSet_nwc_yarp.
 *
 *   Parameters required by this device are:
 * | Parameter name  | SubParameter            | Type    | Units          | Default Value                   | Required     | Description                            |
 * |:---------------:|:-----------------------:|:-------:|:--------------:|:-------------------------------:|:-----------: |:--------------------------------------:|
 * | rpc_port_server |      -                  | string  | -              |   /frameTransformGet/serverRPC  | No           | port on which rpc calls should be made |
 *
 * Some example of configuration files:
 *
 * Example of configuration file using .ini format.
 *
 * \code{.unparsed}
 * device frameTransformSet_nws_yarp
 * rpc_port_server /frameTransformGet/serverRPC
 * \endcode
 */

class FrameTransformSet_nws_yarp :
    public yarp::dev::DeviceDriver,
    public FrameTransformStorageSetRPC,
    public yarp::dev::WrapperSingle
{

public:
    FrameTransformSet_nws_yarp()=default;

    //DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    //wrapper and interfaces
    bool attach(yarp::dev::PolyDriver* device2attach) override;
    bool detach() override;

    //FrameTransformStorageSetRPC functions
    bool setTransforms(const std::vector<yarp::math::FrameTransform>& transforms) override;
    bool setTransform(const yarp::math::FrameTransform& transform) override;

private:
    mutable std::mutex                    m_pd_mutex;
    mutable std::mutex                    m_trf_mutex;
    yarp::dev::PolyDriver*                m_pDriver{nullptr};
    std::string                           m_thriftPortName{"/frameTransformGet/serverRPC"};
    yarp::os::Port                        m_thriftPort;
    yarp::dev::IFrameTransformStorageSet* m_iSetIf;
};

#endif // YARP_DEV_FRAMETRANSFORMSETNWSYARP_H

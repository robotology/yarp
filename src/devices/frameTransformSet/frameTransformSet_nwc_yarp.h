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
 * | Parameter name  | SubParameter            | Type    | Units          | Default Value                   | Required     | Description                            |
 * |:---------------:|:-----------------------:|:-------:|:--------------:|:-------------------------------:|:-----------: |:--------------------------------------:|
 * | rpc_port_client |      -                  | string  | -              |   /frameTransformGet/clientRPC  | No           | port on which rpc calls should be made |
 * | rpc_port_server |      -                  | string  | -              |   /frameTransformGet/serverRPC  | No           | port on which rpc calls should be made |
 *
 * Some example of configuration files:
 *
 * Example of configuration file using .ini format.
 *
 * \code{.unparsed}
 * device FrameTransformGet_nwc_yarp
 * rpc_port_client /frameTransformGet/clientRPC
 * rpc_port_server /frameTransformGet/clientRPC
 * \endcode
 */

class FrameTransformSet_nwc_yarp :
    public yarp::dev::DeviceDriver,
    public yarp::dev::IFrameTransformStorageSet
{

public:
    FrameTransformSet_nwc_yarp()=default;

    //DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    //FrameTransformStorageSetRPC functions
    bool setTransforms(const std::vector<yarp::math::FrameTransform>& transforms) override;
    bool setTransform(const yarp::math::FrameTransform& transform) override;

private:
    mutable std::mutex          m_trf_mutex;
    mutable std::mutex          m_pd_mutex;
    std::string                 m_thriftPortName{"/frameTransformSet/clientRPC"};
    yarp::os::Port              m_thriftPort;
    std::string                 m_thrift_server_rpcPort_Name{"/frameTransformSet/serverRPC"};
    FrameTransformStorageSetRPC m_setRPC;
};

#endif // YARP_DEV_FRAMETRANSFORMSETNWCYARP_H

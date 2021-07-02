/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_FRAMETRANSFORMGET_NWS_YARP_H
#define YARP_DEV_FRAMETRANSFORMGET_NWS_YARP_H

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/IFrameTransformStorage.h>


#include <FrameTransformStorageGetRPC.h>

// TODO FIXME STE need to check subdevice option
/*
 * \section FrameTransformGet_nws_yarp_device_parameters Description of input parameters
 * this device listens on a FrameTransformStorageGetRPC interface for an RPC and then forwards the request to an IFrameTransformStorageGet interface (device attached)
 * This device is paired with its client called FrameTransformGet_nwc_yarp.
 * It receives a rpc from a port (the port is declared in thrift and is FrameTransformStorageGetRPC)
 * and it sends on the port all the transforms taken from the attached device.
 * The attached device must implement an IFrameTransformStorageGet interface.
 * For how to attach the various devices see FrameTransformServer.
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
 * device FrameTransformGet_nws_yarp
 * rpc_port_server /frameTransformGet/serverRPC
 * \endcode
 */

class FrameTransformGet_nws_yarp :
        public yarp::dev::DeviceDriver,
        public FrameTransformStorageGetRPC,
        public yarp::dev::WrapperSingle
{

public:
    FrameTransformGet_nws_yarp();
    FrameTransformGet_nws_yarp(const FrameTransformGet_nws_yarp&) = delete;
    FrameTransformGet_nws_yarp(FrameTransformGet_nws_yarp&&) = delete;
    FrameTransformGet_nws_yarp& operator=(const FrameTransformGet_nws_yarp&) = delete;
    FrameTransformGet_nws_yarp& operator=(FrameTransformGet_nws_yarp&&) = delete;
    ~FrameTransformGet_nws_yarp() = default;

    // yarp::dev::DeviceDriver
    bool  open(yarp::os::Searchable &params) override;
    bool  fromConfig(yarp::os::Searchable &params);
    bool  close() override;

    // yarp::dev::IWrapper
    bool  attach(yarp::dev::PolyDriver* deviceToAttach) override;
    bool  detach() override;

    // FrameTransformStorageGetRPC
    return_getAllTransforms getTransforms() override;

private:
    // mutable std::vector<std::mutex> m_PolyDriver_mutex;
    // double m_period{const_default_thread_period};
    int    m_verbose{4};

    // for requesting the transforms to FrameTransformStorageGetMultiplexer
    yarp::dev::IFrameTransformStorageGet* m_iFrameTransformStorageGet;

    // for the RPC with the NWC
    yarp::os::Port      m_thrift_rpcPort;
    std::string         m_thrift_rpcPort_Name{"/frameTransformGet/serverRPC"};
};

#endif   // YARP_DEV_FRAMETRANSFORMGET_NWS_YARP_H

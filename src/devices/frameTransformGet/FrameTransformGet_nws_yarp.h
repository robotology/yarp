/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_FRAMETRANSFORMGET_NWS_YARP_H
#define YARP_DEV_FRAMETRANSFORMGET_NWS_YARP_H

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/IFrameTransformStorage.h>

#include <yarp/os/PeriodicThread.h>
#include <FrameTransformStorageGetRPC.h>

// TODO FIXME STE need to check subdevice option
/**
 * @ingroup dev_impl_nws_yarp
 *
 * @brief `frameTransformGet_nws_yarp`: A network wrapper client which converts the input retrieved from an IFrameTransformStorageGet interface to a FrameTransformStorageGetRPC thrift interface.
 *
 * \section FrameTransformGet_nws_yarp_device_parameters Parameters
 * this device listens on a FrameTransformStorageGetRPC interface for an RPC and then forwards the request to an IFrameTransformStorageGet interface (device attached)
 * This device is paired with its client called FrameTransformGet_nwc_yarp.
 * It receives a rpc from a port (the port is declared in thrift and is FrameTransformStorageGetRPC).
 * After that it sends on the port all the transforms taken from the attached device.
 * The attached device must implement an IFrameTransformStorageGet interface.
 * For further information see \subpage FrameTransform.
 *
 *   Parameters required by this device are:
 * | Parameter name  | SubParameter            | Type    | Units          | Default Value                   | Required     | Description                            |
 * |:---------------:|:-----------------------:|:-------:|:--------------:|:-------------------------------:|:-----------: |:--------------------------------------:|
 * | rpc_port_server |      -                  | string  | -              |   /frameTransformGet/serverRPC  | No           | name of the port on which rpc calls should be made |
 * | streaming_port_server |      -            | string  | -              |   /frameTransformGet/tf:o       | No           | name of the port on which the tfs are published periodically |
 * | streaming_enabled |      -                | bool    | -              |   true                          | No           | enable/disable the tf publishing on the streaming port |
 * | period |                                  | float   | s              |   0.010                         | No           | It affects the period of thread publishing transforms on the streaming port |
 *
 * \section FrameTransformGet_nws_yarp_device_configuration Example of configuration file using .ini format.
 *
 * \code{.unparsed}
 * device FrameTransformGet_nws_yarp
 * rpc_port_server /frameTransformGet/serverRPC
 * \endcode
 */

class FrameTransformGet_nws_yarp :
        public yarp::dev::DeviceDriver,
        public FrameTransformStorageGetRPC,
        public yarp::dev::WrapperSingle,
        public yarp::os::PeriodicThread
{

public:
    FrameTransformGet_nws_yarp();
    FrameTransformGet_nws_yarp(const FrameTransformGet_nws_yarp&) = delete;
    FrameTransformGet_nws_yarp(FrameTransformGet_nws_yarp&&) = delete;
    FrameTransformGet_nws_yarp& operator=(const FrameTransformGet_nws_yarp&) = delete;
    FrameTransformGet_nws_yarp& operator=(FrameTransformGet_nws_yarp&&) = delete;
    ~FrameTransformGet_nws_yarp() = default;

    // yarp::os::PeriodicThread
    bool threadInit() override;
    void threadRelease() override;
    void run() override;
    bool m_streaming_port_enabled = true;
    yarp::os::Port m_streaming_port;
    std::string    m_streaming_port_name{ "/frameTransformGet/tf:o" };

    // yarp::dev::DeviceDriver
    bool  open(yarp::os::Searchable &params) override;
    bool  close() override;

    // yarp::dev::IWrapper
    bool  attach(yarp::dev::PolyDriver* deviceToAttach) override;
    bool  detach() override;

    // FrameTransformStorageGetRPC
    return_getAllTransforms getTransformsRPC() override;

private:
    // mutable std::vector<std::mutex> m_PolyDriver_mutex;
    // double m_period{const_default_thread_period};
    int    m_verbose{4};

    // for requesting the transforms to FrameTransformStorageGetMultiplexer
    yarp::dev::IFrameTransformStorageGet* m_iFrameTransformStorageGet = nullptr;

    // for the RPC with the NWC
    yarp::os::Port      m_thrift_rpcPort;
    std::string         m_thrift_rpcPort_Name{"/frameTransformGet/serverRPC"};
};

#endif   // YARP_DEV_FRAMETRANSFORMGET_NWS_YARP_H

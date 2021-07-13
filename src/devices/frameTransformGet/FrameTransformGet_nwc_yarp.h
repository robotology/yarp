/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_FRAMETRANSFORMGET_NWC_YARP_H
#define YARP_DEV_FRAMETRANSFORMGET_NWC_YARP_H

#include <yarp/os/PeriodicThread.h>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IFrameTransformStorage.h>


#include <FrameTransformStorageGetRPC.h>

/**
 * @ingroup dev_impl_nwc_yarp
 *
 * @brief `frameTransformGet_nwc_yarp`: A network wrapper client which converts the input retrieved from a FrameTransformStorageGetRPC thrift interface to an IFrameTransformStorageGet interface.
 *
 * \section FrameTransformGet_nwc_yarp_device_parameters Parameters
 * This device is paired with its server called FrameTransformGet_nws_yarp.
 * It is attached to a device with an IFrameTransformStorageGet interface.
 * When it a method of the interface is called, the request is forwarded on a RPC port (which uses the thrift FrameTransformStorageGetRPC interface) and it returns all the transforms received
 * from the port to the caller.
 * For further information see \subpage FrameTransform.

 *   Parameters required by this device are:
 * | Parameter name  | SubParameter            | Type    | Units          | Default Value                   | Required     | Description                            |
 * |:---------------:|:-----------------------:|:-------:|:--------------:|:-------------------------------:|:-----------: |:--------------------------------------:|
 * | rpc_port_client |      -                  | string  | -              |   /frameTransformGet/clientRPC  | No           | name of the port on which rpc calls should be made |
 * | rpc_port_server |      -                  | string  | -              |   /frameTransformGet/serverRPC  | No           | name of the port on which rpc calls should be made |
 * | streaming_port_client |      -            | string  | -              |   /frameTransformGet/tf:i       | No           | name of the port on which the tfs are received (if the port is enabled) |
 * | streaming_enabled |      -                | bool    | -              |   false                         | No           | if enabled, tfs are received from the streaming port instead of using RPCs |
 *
 * \section FrameTransformGet_nwc_yarp_device_configuration Example of configuration file using .ini format.
 *
 * \code{.unparsed}
 * device FrameTransformGet_nwc_yarp
 * rpc_port_client /frameTransformGet/clientRPC
 * rpc_port_server /frameTransformGet/clientRPC
 * \endcode
 */
class FrameTransformGet_nwc_yarp:
    public yarp::dev::DeviceDriver,
    public yarp::dev::IFrameTransformStorageGet
{
    class DataReader : public yarp::os::BufferedPort<return_getAllTransforms>
    {
        std::mutex m_mutex;
        return_getAllTransforms m_Transforms;

        public:
        DataReader() = default;

        using               yarp::os::BufferedPort<return_getAllTransforms>::onRead;
        virtual void        onRead(return_getAllTransforms& v) override;
        bool                getData(return_getAllTransforms& data);
    };

public:
    FrameTransformGet_nwc_yarp() = default;
    FrameTransformGet_nwc_yarp(const FrameTransformGet_nwc_yarp&) = delete;
    FrameTransformGet_nwc_yarp(FrameTransformGet_nwc_yarp&&) = delete;
    FrameTransformGet_nwc_yarp& operator=(const FrameTransformGet_nwc_yarp&) = delete;
    FrameTransformGet_nwc_yarp& operator=(FrameTransformGet_nwc_yarp&&) = delete;
    ~FrameTransformGet_nwc_yarp() override = default;

    // yarp::dev::DeviceDriver
    bool  open(yarp::os::Searchable &params) override;
    bool  close() override;

    // yarp::os::PeriodicThread
    bool m_streaming_port_enabled = false;
    std::string    m_streaming_port_name{ "/frameTransformGet/tf:i" };
    DataReader*    m_dataReader =nullptr;

    // yarp::dev::IFrameTransformStorageGet
    bool getTransforms(std::vector<yarp::math::FrameTransform>& transforms) const override;

private:
    int    m_verbose{4};

    // for the RPC with the NWS
    yarp::os::Port      m_thrift_rpcPort;
    std::string         m_thrift_rpcPort_Name{"/frameTransformGet/clientRPC"};
    std::string         m_thrift_server_rpcPort_Name{"/frameTransformGet/serverRPC"};
    mutable FrameTransformStorageGetRPC m_frameTransformStorageGetRPC;

};


#endif // YARP_DEV_FRAMETRANSFORMGET_NWC_YARP_H

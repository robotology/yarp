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
#include <yarp/dev/IMultipleWrapper.h>
#include <yarp/dev/IFrameTransformStorage.h>

#include <yarp/os/PeriodicThread.h>

#include <FrameTransformStorageGetRPC.h>

constexpr double DEFAULT_THREAD_PERIOD = 0.01; // 10 millisecond
// TODO FIXME STE need to check subdevice option
/*
 * \section FrameTransformGet_nws_yarp_device_parameters Description of input parameters
 * this device listens on a FrameTransformStorageGetRPC interface for an RPC and then forwards the request to an IFrameTransformStorageGet interface (device attached)
 * This device is paired with its client called FrameTransformGet_nwc_yarp.
 *
 *   Parameters required by this device are:
 * | Parameter name | SubParameter            | Type    | Units          | Default Value             | Required     | Description                                    | Notes        |
 * |:--------------:|:-----------------------:|:-------:|:--------------:|:-------------------------:|:-----------: |:----------------------------------------------:|:------------:|
 * | period         |      -                  | double  | sec            |   0.01                    | No           | refresh period of the broadcasted values in ms | default 10ms |
 * | rpc_port       |      -                  | string  | -              |   /frameTransformGet/rpc  | No           | port opened by the device for rpc calls        |              |
 *
 * Some example of configuration files:
 *
 * Example of configuration file using .ini format.
 *
 * \code{.unparsed}
 * device FrameTransformGet_nws_yarp
 * period 0.01
 * rpc_port /frameTransformGet/rpc
 * \endcode
 */

class FrameTransformGet_nws_yarp :
        public yarp::dev::DeviceDriver,
        public FrameTransformStorageGetRPC,
        public yarp::dev::IMultipleWrapper,
        public yarp::os::PeriodicThread
{

public:
    FrameTransformGet_nws_yarp();
    FrameTransformGet_nws_yarp(const FrameTransformGet_nws_yarp&) = delete;
    FrameTransformGet_nws_yarp(FrameTransformGet_nws_yarp&&) = delete;
    FrameTransformGet_nws_yarp& operator=(const FrameTransformGet_nws_yarp&) = delete;
    FrameTransformGet_nws_yarp& operator=(FrameTransformGet_nws_yarp&&) = delete;
    ~FrameTransformGet_nws_yarp() override = default;

    // yarp::dev::DeviceDriver
    bool  open(yarp::os::Searchable &params) override;
    bool  fromConfig(yarp::os::Searchable &params);
    bool  close() override;

    // yarp::dev::IWrapper
    bool  attachAll(const yarp::dev::PolyDriverList& devices2attach) override;
    bool  detachAll() override;

    // yarp::os::PeriodicThread
    void  run() override;

    // FrameTransformStorageGetRPC
    return_getAllTransforms getTransforms() override;

private:
    mutable std::vector<std::mutex>                    m_PolyDriver_mutex;
    double m_period{DEFAULT_THREAD_PERIOD};
    int    m_verbose{4};

    // for requesting the transforms to FrameTransformStorageGetMultiplexer
    std::vector<yarp::dev::IFrameTransformStorageGet*> m_iFrameTransformStorageGetList;

    // to read all the transforms
    typedef yarp::os::BufferedPort<return_getAllTransforms> FrameTransformPortType;

    // for the RPC with the NWC
    yarp::os::Port      m_thrift_rpcPort;
    std::string         m_thrift_rpcPort_Name;

    // this gives the parameters to configure the device
    yarp::os::Property    m_configurationProperty;

    // to manage the various parameters read from the configuration file
    template <class T>
    struct param
    {
        param(T& inVar, std::string inName)
        {
            var          = &inVar;
            parname      = inName;
        }
        T*              var;
        std::string     parname;
    };
};

#endif   // YARP_DEV_FRAMETRANSFORMGET_NWS_YARP_H

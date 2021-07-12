/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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

/**
 * @ingroup dev_impl_nws_yarp
 *
 * @brief `frameTransformSet_nws_yarp`: A network wrapper client which publishes the transforms received on the yarp::dev::IFrameTransformStorageSet interface to thrift interface FrameTransformStorageSetRPC interface.
 *
 * \section FrameTransformSet_nws_yarp_device_parameters Parameters
 * This device is paired with its server called FrameTransformSet_nwc_yarp.
 *
 *   Parameters required by this device are:
 * | Parameter name  | SubParameter            | Type    | Units          | Default Value                   | Required     | Description                            |
 * |:---------------:|:-----------------------:|:-------:|:--------------:|:-------------------------------:|:-----------: |:--------------------------------------:|
 * | rpc_port_server |      -                  | string  | -              |   /frameTransformGet/serverRPC  | No           | port on which rpc calls should be made |
 *
 * \section FrameTransformSet_nws_yarp_example Example of configuration file using .ini format.
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

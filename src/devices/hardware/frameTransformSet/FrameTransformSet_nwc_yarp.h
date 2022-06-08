/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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

/**
 * @ingroup dev_impl_nwc_yarp
 *
 * @brief `frameTransformSet_nwc_yarp`: A network wrapper client which publishes the transforms received on the thrift interface FrameTransformStorageSetRPC to yarp::dev::IFrameTransformStorageSet interface.
 *
 * \section FrameTransformSet_nwc_yarp_device_parameters Parameters
 * This device is paired with its server called FrameTransformSet_nws_yarp.
 *
 *   Parameters required by this device are:
 * | Parameter name               | SubParameter | Type    | Units  | Default Value  | Required  | Description                                                                                                                                                                        |
 * |:----------------------------:|:-------------|:-------:|:------:|:--------------:|:------- -:|:----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------:|
 * | default-client               | -            | bool    | -      | true           | No        | tells whether or not the nwc is instantiated by the frameTransformClient device. If true, "/frameTransformClient" will appended to the port name prefix                            |
 * | default-server               | -            | bool    | -      | true           | No        | tells whether or not the nws is instantiated by the frameTransformServer device. If true, "/frameTransformServer/frameTransformSet_nws_yarp" will appended to the port name prefix |
 * | nwc_thrift_port_prefix       | -            | string  | -      | ""             | No        | a prefix for the nwc thrift rpc port name                                                                                                                                          |
 * | nws_thrift_port_prefix       | -            | string  | -      | ""             | No        | a prefix for the nws thrift rpc port name                                                                                                                                          |
 *
 * \section FrameTransformGet_nwc_yarp_port_example Port names examples
 * Here follow some examples of port names obtained with different parameters configurations
 * -# With
 *      - default-client = true
 *      - nwc_thrift_port_prefix = ""\n
 *        The frameTransformSet_nwc_yarp thrift port name will be: `/frameTransformClient/frameTransformSet_nwc_yarp/thrift`
 * -# With
 *      - default-client = true
 *      - nwc_thrift_port_prefix = "/cer"\n
 *        The frameTransformSet_nwc_yarp thrift port name will be: `/cer/frameTransformClient/frameTransformSet_nwc_yarp/thrift`
 * -# With
 *      - default-client = false
 *      - nwc_thrift_port_prefix = "/cer"\n
 *        The frameTransformSet_nwc_yarp thrift port name will be: `/cer/frameTransformSet_nwc_yarp/thrift`
 * -# With
 *      - default-server = true
 *      - nws_thrift_port_prefix = ""\n
 *       The name of the thrift port the frameTransformSet_nwc_yarp thrift port will connect to will be: `/frameTransformServer/frameTransformSet_nws_yarp/thrift`
 * -# With
 *      - default-server = true
 *      - nws_thrift_port_prefix = "/cer"\n
 *        The name of the thrift port the frameTransformSet_nwc_yarp thrift port will connect to will be: `/cer/frameTransformServer/frameTransformSet_nws_yarp/thrift`
 * -# With
 *      - default-server = false
 *      - nws_thrift_port_prefix = "/cer"\n
 *        The name of the thrift port the frameTransformSet_nwc_yarp thrift port will connect to will be: `/cer/thrift`
 *
 * \section FrameTransformSet_nwc_yarp_example Example of configuration file using .ini format.
 *
 * \code{.unparsed}
 * device frameTransformSet_nwc_yarp
 * nwc_thrift_port_prefix /local
 * nws_thrift_port_prefix /cer
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
    bool deleteTransform(std::string t1, std::string t2) override;
    bool clearAll() override;

private:
    mutable std::mutex          m_trf_mutex;
    mutable std::mutex          m_pd_mutex;
    std::string                 m_deviceName{"frameTransformSet_nwc_yarp"};
    std::string                 m_defaultConfigPrefix{"/frameTransformClient"};
    std::string                 m_defaultServerPrefix{"/frameTransformServer/frameTransformSet_nws_yarp"};
    std::string                 m_thriftPortName;
    std::string                 m_thrift_server_rpcPort_Name;
    yarp::os::Port              m_thriftPort;
    FrameTransformStorageSetRPC m_setRPC;
};

#endif // YARP_DEV_FRAMETRANSFORMSETNWCYARP_H
